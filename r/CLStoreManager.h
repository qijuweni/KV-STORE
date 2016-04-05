#ifndef STORE_MANAGER
#define STORE_MANAGER

struct SLNode;
class CLFreeNodeManagerWithSameSize;
class CLBackList;
class CLLogger;

#define SLAB_SIZE 1024*1024
#define LOGGER_SIZE 1024*1024*4

class CLStoreManager
{
public:
	CLStoreManager(int distanceBetweenNodeManager, int listNum, void *pBeginPos, unsigned long ulSize, bool isRecover);
	~CLStoreManager();
	
	void * New(int size);   
	void Delete(void *ptr);
	bool ClearLogger();
	bool IsInStore(void *ptr);
//	bool AddFreeNode(SLNode *pNode);
//	bool DeleteFreeNode(SLNode *pNode);

//protected:
private:
	void *m_pBeginPos;
	unsigned long m_ulSize;	
	int m_listNum;
	int m_distanceBetweenNodeManager;
	CLFreeNodeManagerWithSameSize **m_pArrayCLFreeNodeManager;
	CLLogger *m_pLogger;
	CLBackList *m_pBackList;
};

#endif
