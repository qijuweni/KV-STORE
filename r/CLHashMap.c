#include "CLHashMap.h"
#include "CLStoreManager.h"
#include "CLLogger.h"
#include <string.h>
#include <iostream>
#include "CLThreadPool.h"
#include "CLJobQueue.h"
#include "CLProtocol.h"
#include "unistd.h"

using namespace std;

struct SLHashPara
{
	CLHashMap *pMap;
	SLTask *pTask;
	int threadIndex;
};

CLHashMap::CLHashMap( int threadNum, CLStoreManager **pStoreManager, void* pbucketHeadArray, int arrayCapacity, CLLogger *pLogger, int pipeWriteFd)
{
	m_threadNum = threadNum;
	m_pbucketHeadArray = (SLbucktNode**)pbucketHeadArray;
	m_pStoreManager = pStoreManager;
	m_arrayCapacity = arrayCapacity;
	m_pLogger = pLogger;
	m_pipeWriteFd = pipeWriteFd;
	CLJobQueue **pJobQueuesArray = new CLJobQueue*[threadNum];
	m_pJobQueuesArray = pJobQueuesArray;

	for(int i = 0; i < threadNum; i++)
	{
		pJobQueuesArray[i] = new CLJobQueue(MAXJOB_PER_QUEUE);
	}

	m_pPool = new CLThreadPool(threadNum, pJobQueuesArray);
	m_pPool->InitThreadPool();
};

CLHashMap::~CLHashMap()
{

};

unsigned int CLHashMap::Hash(char* key)
{
	unsigned int seed = 131;
	unsigned int hash = 0;
	
	while (*key)
	{
		hash = hash * seed + (*key++);
	}
 
	return (hash & 0x7FFFFFFF) % m_arrayCapacity;	
};

bool CLHashMap::AddTask(SLTask *pTask)
{
	if(pTask == NULL)
		return false;

	int hashIndex = Hash(pTask->pPackage->m_RequestHead.key);	
	int indexNumPerThread = m_arrayCapacity / m_threadNum;
	int threadIndex = hashIndex / indexNumPerThread;
	
	if(threadIndex == m_threadNum)
		threadIndex--;

	SLJob *pJob = new SLJob;
	
	SLHashPara *pArg = new SLHashPara;
	pArg->pTask = pTask;
	pArg->pMap = this;	
	pArg->threadIndex = threadIndex;
	pJob->arg = pArg;

	switch(pTask->pPackage->m_RequestHead.opcade)
	{
	case SET:
		{
			pJob->callback_function = Insert;
			break;
		}
	case GET:
		{
			pJob->callback_function = Find;
			break;
		}
	}

	if(m_pJobQueuesArray[threadIndex]->AddJob(pJob))
		return true;

	return false;
};

/*bool CLHashMap::Delete(char* key)
{
	SLbucktNode *pNodePre;
	SLbucktNode *pNode = FindNode(key, &pNodePre);

	if(pNode == NULL)
		return false;
		
	 unsigned int index = Hash(key) - m_beginIndex;	
	 
	 if(m_pbucketHeadArray[index] != pNode)
	 {
		 m_pLogger->WriteLogger(m_pbucketHeadArray[index], sizeof(SLbucktNode*));
	 }
	 else
	 {
		
	 }
};
*/
void* CLHashMap::Insert(void *ptr)
{
	SLHashPara *pPara = (SLHashPara*)ptr;
	SLbucktNode *pNodePre;
	SLbucktNode *pNode = FindNode(pPara->pTask->pPackage->m_RequestHead.key, &pNodePre, pPara->pMap);
	int valueSize = strlen(pPara->pTask->pPackage->pValue) + 1;
	char *pValueStore = (char*)(pPara->pMap->m_pStoreManager[pPara->threadIndex]->New(valueSize));

	if(pValueStore == NULL)
		return NULL;

	strcpy(pValueStore, pPara->pTask->pPackage->pValue);
	pValueStore[valueSize] = 0;
	
	CLHashMap *pMap = pPara->pMap;

	if(pNode != NULL)
	{
		pMap->m_pLogger->WriteLogger(&pNode->value, sizeof(char*));
		pNode->value = pValueStore;                   ///回退回去value的旧值可能会出现破坏
		pMap->m_pStoreManager[pPara->threadIndex]->Delete(pNode->value);          //是否应该最后释放
	}
	else
	{
		unsigned int index = pMap->Hash(pPara->pTask->pPackage->m_RequestHead.key);
		
		pNode = (SLbucktNode*)(pMap->m_pStoreManager[pPara->threadIndex]->New(sizeof(SLbucktNode)));
		strcpy(pNode->key, pPara->pTask->pPackage->m_RequestHead.key);
		pNode->value = pValueStore;
		pNode->pNodeNext = pMap->m_pbucketHeadArray[index];
		
		pMap->m_pLogger->WriteLogger(&(pMap->m_pbucketHeadArray[index]), sizeof(SLbucktNode*));
		pMap->m_pbucketHeadArray[index] = pNode;
	}

	pMap->m_pLogger->ClearLogger();
	pMap->m_pStoreManager[pPara->threadIndex]->ClearLogger();
	write(pPara->pMap->m_pipeWriteFd, &pPara->pTask, sizeof(SLTask*));
	return NULL;
};

SLbucktNode* CLHashMap::FindNode(char *key, SLbucktNode **pNodePreAddr, CLHashMap *pMap)//没找到返回null;找到了，如果是头节点，则pNodePreAddr为头节点地址，否则为前一个节点地址
{
	unsigned int index = pMap->Hash(key) ;

	SLbucktNode *pNodeHead = pMap->m_pbucketHeadArray[index];
	SLbucktNode *pReturnNode = pNodeHead;
	if(pReturnNode == NULL)
	{
		*pNodePreAddr = NULL;
		return NULL;
	}

	*pNodePreAddr = pReturnNode;

	while(pReturnNode != NULL)
	{
		if(strcmp(key, pReturnNode->key) == 0)
		{
			return pReturnNode; 
		}
		else
		{
			*pNodePreAddr = pReturnNode;
			pReturnNode = pReturnNode->pNodeNext;
		}
	}
	
	*pNodePreAddr = NULL;
	return NULL;
};

void* CLHashMap::Find(void *arg)
{
	SLHashPara *pPara = (SLHashPara*)arg;

	SLbucktNode *pNodePre;
	SLbucktNode *pNode = FindNode(pPara->pTask->pPackage->m_RequestHead.key, &pNodePre, pPara->pMap);

	SLProtocolPackage *pPackage = pPara->pTask->pPackage;

	if(pNode == NULL)
	{
		pPackage->pValue = NULL;
	}
	else
	{
		int valueSize = 0;
		valueSize = strlen(pNode->value) + 1;
		pPackage->pValue = new char[valueSize];
		strcpy(pPackage->pValue, pNode->value);
		pPackage->pValue[valueSize] = 0;
	}

	write(pPara->pMap->m_pipeWriteFd, &(pPara->pTask), sizeof(SLTask*));
	return NULL;	
};
