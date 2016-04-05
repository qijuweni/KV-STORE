#include <pthread.h>

#define MAXJOB_PER_QUEUE 20

class CLJobQueue;
class CLThreadPool;

struct SLThreadFunPara
{
	int index;
	CLThreadPool *pPool;
};

class CLThreadPool
{
public:
	CLThreadPool(int threadNum, CLJobQueue **pJobQueuesArray);
	virtual ~CLThreadPool();
	bool InitThreadPool();

	static void* threadpool_function(void* p);

private:
	int m_threadNum;
	pthread_t *m_pthreads;
	CLJobQueue **m_pJobQueuesArray;
};
