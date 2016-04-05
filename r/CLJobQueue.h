#include <pthread.h>

struct SLJob
{
	void* (*callback_function)(void *arg);
	void *arg;
//	struct job *next;
};

class CLJobQueue
{
public:
	CLJobQueue(int queueMaxNum);
	virtual ~CLJobQueue();
	bool Init();
	bool AddJob(SLJob *pJob);
	SLJob* GetJob();
private:
	int m_queueMaxNum;
	int m_queueNowNum;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_queue_not_empty;
	pthread_cond_t m_queue_not_full;
	SLJob **m_pJobsAddr;
	int m_headIndex;
	int m_tailIndex;
};

