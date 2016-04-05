#include "CLSlabManager.h"
#include "CLLogger.h"
#include "SLNode.h"
#include <iostream>

using namespace std;

CLSlabManager::CLSlabManager(void *pBeginPos, unsigned int slabSize, int nodeSize, bool isRecover, CLLogger *pLogger)
{
	m_pBeginPos = pBeginPos;
	m_slabSize = slabSize;
	m_pHead = NULL;
	m_nowCountFreeNode = -1;
	m_nodeSize = nodeSize;
	m_pLogger = pLogger;
	m_numFreeNode = (m_slabSize - sizeof(void*)) / (sizeof(SLNode) + m_nodeSize);
	
	if(!isRecover)
	{
		Format();
	}
	else
	{
		SLNode *pNodeHead = (SLNode*) ((char*)m_pBeginPos + sizeof(void*));	
		
		while(pNodeHead->m_isUsed == true)
		{
			pNodeHead = (SLNode*)((char*)pNodeHead + sizeof(SLNode) + m_nodeSize);

			if((unsigned long)((char*)pNodeHead + sizeof(SLNode) + m_nodeSize) >= (unsigned long)((char*)m_pBeginPos + m_slabSize))
			{
				m_pHead = NULL;
				m_nowCountFreeNode = 0;
				break;
			}
		}

		if(m_nowCountFreeNode == -1)
		{
			m_pHead = pNodeHead;
			
			pNodeHead = m_pHead->m_pnodeNext;
			m_nowCountFreeNode = 1;

			while(pNodeHead != m_pHead)
			{
				m_nowCountFreeNode++;
				pNodeHead = pNodeHead->m_pnodeNext;
			}
		}
	}
};	

CLSlabManager::~CLSlabManager()
{

};

void *CLSlabManager::GetNextSlabAddr()
{
	return *(void**)m_pBeginPos;
};

bool CLSlabManager::Format()
{
	void ** pNextSlabAddr = (void**)m_pBeginPos;
	*pNextSlabAddr = NULL;
	
	SLNode *pNodeHead = (SLNode*) (pNextSlabAddr + 1);
	
	SLNode *pPreNode = pNodeHead;
	pPreNode->m_isUsed = false;
	pPreNode->m_size = m_nodeSize;

	for(int i = 1; i < m_numFreeNode; i++)
	{
		SLNode *pNextNode =(SLNode*)((char*)pPreNode + sizeof(SLNode) + m_nodeSize);		
		pPreNode->m_pnodeNext = pNextNode;
		pNextNode->m_pnodePre = pPreNode;
		pNextNode->m_size = m_nodeSize;
		pNextNode->m_isUsed = false;
		pPreNode = pNextNode;
	}	

	pNodeHead->m_pnodePre = pPreNode;
	pPreNode->m_pnodeNext = pNodeHead;	
	m_pHead = pNodeHead;
	m_nowCountFreeNode = m_numFreeNode;

	return true;
};


SLNode * CLSlabManager::Request()
{
	if(m_nowCountFreeNode == 0)
		return NULL;

	if(Traverse() != m_nowCountFreeNode)
		cout<<" "<<endl;

	SLNode *pNodeReturn = m_pHead;

	if(m_pHead->m_pnodePre == m_pHead)
	{
		m_pHead = NULL;
		m_nowCountFreeNode--;
		
		m_pLogger->WriteLogger(&pNodeReturn->m_isUsed, sizeof(bool));
		pNodeReturn->m_isUsed = true;
		return pNodeReturn;
	}
	else
	{		
		SLNode *pHeadNext = m_pHead->m_pnodeNext;
		SLNode *pHeadPre = m_pHead->m_pnodePre;
		m_pLogger->WriteLogger(&pHeadPre->m_pnodeNext, sizeof(SLNode*));
		pHeadPre->m_pnodeNext = pHeadNext;
		m_pLogger->WriteLogger(&pHeadNext->m_pnodePre, sizeof(SLNode*));
		pHeadNext->m_pnodePre = pHeadPre;
		m_pHead = pHeadNext;		
		m_nowCountFreeNode--;
		
		m_pLogger->WriteLogger(&pNodeReturn->m_isUsed, sizeof(bool));
		pNodeReturn->m_isUsed = true;
		return pNodeReturn;
	}
};

bool CLSlabManager::AddFreeNode(SLNode *pNode)
{
	if(pNode == NULL)
		return false;

	if(Traverse() != m_nowCountFreeNode)
		cout<<" "<<endl;
	if((unsigned long)pNode <= (unsigned long)m_pBeginPos || (unsigned long)pNode >= (unsigned long)((char*)m_pBeginPos + m_slabSize))
		return false;
	
	if(m_pHead == NULL)
	{
		m_pHead = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));  //是否记日志  一个事务中会不会出现申请一块空间，然后再释放一块空间?
		m_pHead->m_pnodePre = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*)); //
		m_pHead->m_pnodeNext = pNode;
	}
	else
	{
		if(m_pHead == m_pHead->m_pnodePre&&m_nowCountFreeNode != 1)
			cout<<""<<endl;
		m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*));
		pNode->m_pnodeNext = m_pHead;
		SLNode *pLastNode = m_pHead->m_pnodePre;
		m_pLogger->WriteLogger(&m_pHead->m_pnodePre, sizeof(SLNode*));
		m_pHead->m_pnodePre = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));
		pNode->m_pnodePre = pLastNode;
		m_pLogger->WriteLogger(&pLastNode->m_pnodeNext, sizeof(SLNode*));
		pLastNode->m_pnodeNext = pNode;
	}
	
	

	m_pLogger->WriteLogger(&pNode->m_isUsed, sizeof(bool));
	pNode->m_isUsed = false;	
	m_nowCountFreeNode++;
	if(m_nowCountFreeNode > m_numFreeNode)
		cout<<"　　"<<endl;

	return true;
};

int CLSlabManager::Traverse()
{
	SLNode *pNode = m_pHead;
	if(pNode == NULL)
		return 0;
	
	pNode = pNode->m_pnodeNext;
	int i = 1;
//	cout<<"size "<<pNode->m_size<<" in slab"<<endl;

	while(pNode != m_pHead)	
	{
	//	cout<<"addr "<<pNode<<" in slab"<<endl;
		i++;
		if(pNode == pNode->m_pnodeNext)
			cout<<""<<endl;
		pNode = pNode->m_pnodeNext;
	}

	return i;
};

bool CLSlabManager::AddNextSlabManager(void *pNextSlabBeginPos)
{	
	m_pLogger->WriteLogger(m_pBeginPos, sizeof(void*));
	*((void**)m_pBeginPos) = pNextSlabBeginPos;

	return true;
};
