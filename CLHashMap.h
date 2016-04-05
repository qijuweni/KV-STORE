#include <event2/bufferevent.h>

class CLStoreManager;
class CLLogger;

#define KEY_LEN 16

struct SLbucktNode
{
	char key[KEY_LEN];
	char *value;
	SLbucktNode *pNodeNext;
};

struct SLProtocolPackage;
class CLThreadPool;
class CLJobQueue;

struct SLTask
{
	SLProtocolPackage *pPackage;
	bufferevent * bev;
};

class CLHashMap
{
public:
	CLHashMap(int threadNum, CLStoreManager **pStoreManager, void* pbucketHeadArray, int arrayCapcity, CLLogger *pLogger, int pipeWriteFd);
	virtual ~CLHashMap();
	bool AddTask(SLTask *pTask);

	static void* Insert(void *ptr);
	static void* Find(void *pKey);
	static void* Delete(void* pKey);
	
protected:
	unsigned int Hash(char* key);
	static SLbucktNode* FindNode(char *key, SLbucktNode **pNodePreAddr, CLHashMap *pMap);
private:
	CLLogger *m_pLogger;
	int m_arrayCapacity;
	SLbucktNode **m_pbucketHeadArray;
	CLStoreManager **m_pStoreManager;
	CLJobQueue **m_pJobQueuesArray;
	CLThreadPool *m_pPool;
	int m_pipeWriteFd;
	int m_threadNum;
};
