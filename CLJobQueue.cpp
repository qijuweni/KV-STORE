#include "CLJobQueue.h"
#include <iostream>

using namespace std;

CLJobQueue::CLJobQueue(int queueMaxNum)
{
	m_queueMaxNum = queueMaxNum;
	m_pJobsAddr = new SLJob*[m_queueMaxNum];
	m_headIndex = 0;
	m_tailIndex = 0;
	m_queueNowNum =0;

};

CLJobQueue::~CLJobQueue()
{

};

bool CLJobQueue::Init()
{
	if(pthread_mutex_init(&m_mutex, NULL))
	{
		cout<<"failed to init mutex!"<<endl;
		return false;
	}

	if(pthread_cond_init(&m_queue_not_empty, NULL))
        {
		cout<<"failed to init queue_not_empty!"<<endl;
	        return false;
	}		
	
	if(pthread_cond_init(&m_queue_not_full, NULL))
        {
		cout<<"failed to init m_queue_not_full!"<<endl;
	        return false;
	}		
	
	return true;
};

bool CLJobQueue::AddJob(SLJob *pJob)
{
	if(pJob == NULL)
		return false;

	pthread_mutex_lock(&m_mutex);
	
	while (m_queueNowNum  == m_queueMaxNum)	
	{
//		cout<<"full"<<endl;
		pthread_cond_wait(&m_queue_not_full, &m_mutex);
//		cout<<"not full"<<endl;
	}	
	m_pJobsAddr[m_tailIndex] = pJob;
	m_tailIndex = (m_tailIndex + 1) % m_queueMaxNum;

	m_queueNowNum++;

	if(m_queueNowNum == 1)
	{
		pthread_cond_signal(&m_queue_not_empty);
	}

	pthread_mutex_unlock(&m_mutex);
	
	return true;
};

SLJob* CLJobQueue::GetJob()
{
	pthread_mutex_lock(&m_mutex);
	
	while(m_queueNowNum == 0)
	{
//		cout<<"empty "<<endl;
		pthread_cond_wait(&m_queue_not_empty, &m_mutex);
//		cout<<"not empty"<<endl;
	}

	SLJob* pReturn = m_pJobsAddr[m_headIndex];
	m_headIndex = (m_headIndex + 1) % m_queueMaxNum;

	m_queueNowNum--;

	if(m_queueNowNum == m_queueMaxNum - 1)
	{
		pthread_cond_broadcast(&m_queue_not_full);
	}

	pthread_mutex_unlock(&m_mutex);

	return pReturn;
};
