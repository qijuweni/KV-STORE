#ifndef BACK_LIST
#define BACK_LIST

struct SLNode;
class CLLogger;

class CLBackList
{
public:
	CLBackList(SLNode **pHeadNodeAddr, CLLogger *pLogger, unsigned int minSizeInList, void *begin, void *end);
	virtual ~CLBackList();
	bool AddNode(SLNode *pNode);
	bool DeleteNode(SLNode *pNode);
	SLNode* GetSLNode(unsigned int size);

	int Traverse();
protected:
	void Merge(SLNode *pNode);
	SLNode* FindFitNode(unsigned int size);

private:
	unsigned int m_minSizeInList;
	SLNode **m_pHeadNodeAddr;
	CLLogger *m_pLogger;
	void *m_begin;
	void *m_end;
};

#endif
