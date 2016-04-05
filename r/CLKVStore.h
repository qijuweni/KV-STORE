#define LOG_CAPACITY 4096
#define DISTANCE_BETWEEN_STORENODEMANAGER 8
#define SOTRE_LIST_NUM 1024
#define THREAD_NUM 4
#define HASH_BUCKET_NUM 1024

class CLStoreManager;
class CLLogger;
class CLHashMap;

/*struct SLHashNode
{
	void *pKey;
	void *pValue;
	SLHashNode *pNext;
};
*/

struct SLTask;

class CLKVStore
{
public:
	CLKVStore(void *pBeginpos, unsigned long size, int pipeWriteFd);
	virtual ~CLKVStore();
	bool InsertTask(SLTask *pTask);	
//	bool FormatStore();
//	bool Recover();	

private:
	CLStoreManager **m_pStoreManagerArray;
    CLHashMap *m_pHashmap;
//	void *m_pBeginpos;
//	unsigned long m_size;
};
