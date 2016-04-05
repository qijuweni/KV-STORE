#include "CLJobQueue.h"
#include "CLThreadPool.h"
#include <iostream>

CLThreadPool::CLThreadPool(int threadNum, CLJobQueue **pJobQueuesArray)
{
	m_threadNum = threadNum;
	m_pJobQueuesArray = pJobQueuesArray;
};

CLThreadPool::~CLThreadPool()
{

};

void* CLThreadPool::threadpool_function(void *p)
{
	SLThreadFunPara *pPara =  (SLThreadFunPara*)p;

	while(1)
	{
		SLJob *pJob = pPara->pPool->m_pJobQueuesArray[pPara->index]->GetJob();
		pJob->callback_function(pJob->arg);
	}

	return NULL;
};

bool CLThreadPool::InitThreadPool()
{
	m_pthreads = new pthread_t[m_threadNum];
	if(m_pthreads == NULL)
		return false;

	for(int i = 0; i < m_threadNum; ++i)
        {
		SLThreadFunPara *pPara = new SLThreadFunPara;
		pPara->index = i;
		pPara->pPool = this;
		m_pJobQueuesArray[i]->Init();
		pthread_create(&m_pthreads[i], NULL, threadpool_function, (void *)pPara);
	}
	
	return true;	
};
