#include "CLKVStore.h"
#include "CLLogger.h"
#include <string.h>
#include <iostream>
#include "CLHashMap.h"
#include "CLStoreManager.h"

CLKVStore::CLKVStore(void *pBeginPos, unsigned long ulSize, int pipeWriteFd)
{
	bool isRecover = true;
	char *pFlag = (char*)pBeginPos;
	if(*((char*)pBeginPos) != 'R')	
	{	
		memset(pBeginPos, 0, ulSize);
		isRecover = false;
	}

	pBeginPos = (char*)pBeginPos + 1;
	CLLogger *pLogger = new CLLogger(pBeginPos, LOG_CAPACITY ,isRecover);
	pBeginPos = (char*)pBeginPos + LOG_CAPACITY;

	unsigned long perStoreManagerSize = (ulSize - 1 -  HASH_BUCKET_NUM * sizeof(SLbucktNode*) - LOG_CAPACITY) / THREAD_NUM; 
	m_pStoreManagerArray = new CLStoreManager*[THREAD_NUM];
	
	for(int i = 0; i < THREAD_NUM; i++)
	{
		m_pStoreManagerArray[i] = new CLStoreManager(DISTANCE_BETWEEN_STORENODEMANAGER, SOTRE_LIST_NUM, pBeginPos, perStoreManagerSize, isRecover);
		pBeginPos = (char*)pBeginPos + perStoreManagerSize;
	}

//	CLLogger *pLogger = new CLLogger(pBeginPos, LOG_CAPACITY ,isRecover);	
//	pBeginPos = (char*)pBeginPos + LOG_CAPACITY;
	
	m_pHashmap = new CLHashMap(THREAD_NUM, m_pStoreManagerArray, pBeginPos, HASH_BUCKET_NUM, pLogger, pipeWriteFd);
	*pFlag = 'R';	
};

CLKVStore::~CLKVStore()
{

};

bool CLKVStore::InsertTask(SLTask *pTask)
{
	return m_pHashmap->AddTask(pTask);	
};

/*bool CLKVStore::FormatStore()
{
	m_pLogger = new CLLogger(m_pBeginpos, LOG_CAPACITY, false);
	void *nowPos =  (char*)m_pBeginpos + LOG_CAPACITY;

	SLHashNode **m_Hashmap = new SLHashNode*[HASH_SLOT_NUM];

	for(int i = 0; i < HASH_SLOT_NUM; i++)
	{
		m_Hashmap[i] = new (nowPos)SLHashNode;
		nowPos = (char*)nowPos + sizeof(SLHashNode);
	}
	
	m_pSotremanager = new CLStoreManager(DISTANCE_BETWEEN_STORENODEMANAGER, SOTRE_LIST_NUM, nowPos, m_size - ((char*)nowPos - (char*)m_pBeginpos));

	return true;
};

bool CLKVStore::Recover()
{
	m_pLogger = new CLLogger(m_pBeginpos, LOG_CAPACITY, true);
	void *nowPos =  (char*)m_pBeginpos + LOG_CAPACITY;

	SLHashNode **m_Hashmap = new SLHashNode*[HASH_SLOT_NUM];
	
	for(int i = 0; i < HASH_SLOT_NUM; i++)
	{
		m_Hashmap[i] = (SLHashNode*)nowPos;
		nowPos = (char*)nowPos + sizeof(SLHashNode);
	}


};*/
