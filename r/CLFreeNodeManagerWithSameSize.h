#ifndef FREENODE_MANAGER
#define FREENODE_MANAGER

class CLLogger;
class CLSlabManager;
struct SLNode;

#define INITARRAY_CAPICITY 10

class CLFreeNodeManagerWithSameSize
{
public:
	CLFreeNodeManagerWithSameSize(void **pFirstSlabAddr, CLLogger *pLogger, unsigned int nodeSize, unsigned int slabSize);
	~CLFreeNodeManagerWithSameSize();
//	bool AddFreeNode(SLNode *pNode);
	bool AddSlab(void *pNode);
	SLNode* RequestFreeNode();
//	bool IsEmpty();
	bool DeleteFreeNode(SLNode *pNode);
//	int Traverse();	

private:
	CLSlabManager **m_pSlabManagerArray;
	unsigned int m_slabSize;
	unsigned int m_nodeSize;
	int m_countSlab;
	int m_SlabManagerArrayCapicity;
	void **m_pFirstSlabAddr;
	CLLogger *m_pLogger;
};

#endif
