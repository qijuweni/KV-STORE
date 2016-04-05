#include "CLStoreManager.h"
#include "CLFreeNodeManagerWithSameSize.h"
#include "SLNode.h"
#include <string.h>
#include <iostream>
#include "CLLogger.h"
#include "CLBackList.h"

using namespace std;

void CLStoreManager::Delete(void *ptr)
{
	if(!IsInStore(ptr))
	{
		cout<<"not in store"<<endl;
	//	exit(-1);
	}
        else
	{
		SLNode *pDeleteNode = (SLNode*)((char*)ptr - sizeof(SLNode));
	
		if(pDeleteNode->m_isUsed == false)
			cout<<"delete false"<<endl;	

		if(pDeleteNode->m_size >= (unsigned int)m_listNum * m_distanceBetweenNodeManager)
		{
			m_pBackList->AddNode(pDeleteNode);
		}
		else if(pDeleteNode->m_size <= 0)
		{
			cout<<"delete node wrong"<<endl;
		}
		else
		{
			int index = -1;
			if(pDeleteNode->m_size % m_distanceBetweenNodeManager == 0)
				index = pDeleteNode->m_size / m_distanceBetweenNodeManager - 1;
			else
				index = pDeleteNode->m_size / m_distanceBetweenNodeManager;

			m_pArrayCLFreeNodeManager[index]->DeleteFreeNode(pDeleteNode);
		}
	}
};

CLStoreManager::CLStoreManager(int distanceBetweenNodeManager, int listNum, void *pBeginPos, unsigned long ulSize, bool isRecover)
{
//	if(*((char*)pBeginPos) != 'q')
//		memset(pBeginPos, 0, ulSize);

//	if(sizeof(SLNode) % distanceBetweenNodeManager != 0)
//		cout<<"distanceBetweenNodeManager is not fit"<<endl;		
	m_distanceBetweenNodeManager = distanceBetweenNodeManager;
	m_listNum = listNum;
    	m_pBeginPos = pBeginPos;
	m_ulSize = ulSize;
	m_pArrayCLFreeNodeManager = new CLFreeNodeManagerWithSameSize*[listNum];
	
	m_pLogger = new CLLogger(pBeginPos, LOGGER_SIZE ,isRecover);
	
	pBeginPos = (char*)pBeginPos + LOGGER_SIZE;

	for(int i = 0; i < listNum; i++)
	{
		
		void **pFirstSlabAddr = ((void**)pBeginPos) + i;
		m_pArrayCLFreeNodeManager[i] = new CLFreeNodeManagerWithSameSize(pFirstSlabAddr, m_pLogger, (i + 1) * m_distanceBetweenNodeManager , SLAB_SIZE);
	}

	SLNode ** pHeadNodeAddr = (SLNode**)((char*)pBeginPos + sizeof(void**) * listNum);
	m_pBackList = new CLBackList(pHeadNodeAddr, m_pLogger, m_listNum * m_distanceBetweenNodeManager, m_pBeginPos , ((char*)m_pBeginPos) + m_ulSize);	//
	
	if(*pHeadNodeAddr == NULL)
	{
      		SLNode *pBigNodeHead = (SLNode*)((char*)pBeginPos + sizeof(void*) * listNum + sizeof(SLNode**));
		pBigNodeHead->m_size = ulSize - sizeof(SLNode) - sizeof(void**) * listNum - sizeof(SLNode**) - LOGGER_SIZE;
		pBigNodeHead->m_isUsed = false;
		pBigNodeHead->m_pnodePre = pBigNodeHead;
		pBigNodeHead->m_pnodeNext = pBigNodeHead;
		m_pBackList->AddNode(pBigNodeHead);//此时崩了。在还未初始化好时
		m_pLogger->ClearLogger();
	}
};

bool CLStoreManager::ClearLogger()
{
	return m_pLogger->ClearLogger();
};

CLStoreManager::~CLStoreManager()
{

};

bool CLStoreManager::IsInStore(void *ptr)
{
	if((unsigned long)ptr < (unsigned long)m_pBeginPos)
		return false;
	
	if((unsigned long)ptr >= (unsigned long)((char*)m_pBeginPos + m_ulSize))
		return false;

	return true;
};

/*bool CLStoreManager::AddFreeNode(SLNode *pNode)
{
	if(pNode == NULL)
		return false;

	unsigned int size = pNode->m_size;
	int indexInArrayCLFreeNodeManager = -1;
	
	if(size > (unsigned int)m_listNum * m_distanceBetweenNodeManager)
		indexInArrayCLFreeNodeManager = 0;
	else if(size < 8)
		return false;
	else
		indexInArrayCLFreeNodeManager = size / m_distanceBetweenNodeManager;

	if(!m_pArrayCLFreeNodeManager[indexInArrayCLFreeNodeManager]->AddFreeNode(pNode))
		return false;
	
	return true;
};	
*/

/*bool CLStoreManager::DeleteFreeNode(SLNode *pNode)
{
	if(pNode == NULL)
		return false;

	unsigned int size = pNode->m_size;
	int indexInArrayCLFreeNodeManager =  -1;
	
	if(size > (unsigned int)m_listNum * m_distanceBetweenNodeManager)
		indexInArrayCLFreeNodeManager = 0;
	else if(size < 8)
		return false;
	else
		indexInArrayCLFreeNodeManager = size / m_distanceBetweenNodeManager;

	if(!m_pArrayCLFreeNodeManager[indexInArrayCLFreeNodeManager]->DeleteFreeNode(pNode))
		return false;
	
	return true;
};*/

void* CLStoreManager::New(int size)
{
	if(size <= 0)
		return NULL;

	if(size <= m_listNum * m_distanceBetweenNodeManager)
	{
		int modRest = size % m_distanceBetweenNodeManager;
		int indexArrayCLFreeNodeManager = -1;

		if(modRest == 0)
			indexArrayCLFreeNodeManager = size / m_distanceBetweenNodeManager - 1;
		else
			indexArrayCLFreeNodeManager = size / m_distanceBetweenNodeManager;

		SLNode *pReturnSLNode = m_pArrayCLFreeNodeManager[indexArrayCLFreeNodeManager]->RequestFreeNode();
		
		if(pReturnSLNode == NULL)
		{
			SLNode *pNode = m_pBackList->GetSLNode(SLAB_SIZE);
			if(pNode == NULL)
			{
				cout<<"new false"<<endl;
				return NULL;
			}
			else
			{
				m_pArrayCLFreeNodeManager[indexArrayCLFreeNodeManager]->AddSlab((char*)pNode + sizeof(SLNode));
			}

			pReturnSLNode = m_pArrayCLFreeNodeManager[indexArrayCLFreeNodeManager]->RequestFreeNode();
		}
		
		return (void*)((char*)pReturnSLNode + sizeof(SLNode));	
	}		
	else
	{
		SLNode *pReturnSLNode = m_pBackList->GetSLNode(size);
		if(pReturnSLNode == NULL)
		{
			cout<<"new fasle."<<endl;
			return NULL;
		}

		return (void*)((char*)pReturnSLNode + sizeof(SLNode));
	}

};
