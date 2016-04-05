#ifndef SLAB_MANAGER
#define SLAB_MANAGER

struct SLNode;
class CLLogger;


class CLSlabManager
{
public:
	CLSlabManager(void *pBeginPos, unsigned int slabSize, int nodeSize, bool isRecover, CLLogger *pLogger);
	virtual ~CLSlabManager();	
	bool Format();
	bool AddNextSlabManager(void *pNextSlabBeginPos);
	SLNode * Request();
	bool AddFreeNode(SLNode *pNode);	
	void *GetNextSlabAddr();
	int Traverse();
private:
	void *m_pBeginPos;
	unsigned int m_slabSize;
	int m_nowCountFreeNode;
	int m_numFreeNode;
	SLNode *m_pHead;
	int m_nodeSize;
	CLLogger *m_pLogger;	
};

#endif
