#include "SLNode.h"
#include "CLFreeNodeManagerWithSameSize.h"
#include "CLLogger.h"
#include "CLSlabManager.h"
#include <iostream>

using namespace std;

CLFreeNodeManagerWithSameSize::CLFreeNodeManagerWithSameSize(void **pFirstSlabAddr, CLLogger *pLogger, unsigned int nodeSize, unsigned int slabSize)
{
	m_pFirstSlabAddr = pFirstSlabAddr;
	m_pLogger = pLogger;
	m_SlabManagerArrayCapicity = INITARRAY_CAPICITY;
	m_pSlabManagerArray = new CLSlabManager*[m_SlabManagerArrayCapicity];
	m_nodeSize = nodeSize;
	m_slabSize = slabSize;

	if(*pFirstSlabAddr == NULL)
		m_countSlab = 0;
	else
	{
		void *pSlabBeginPos = *pFirstSlabAddr;
		m_countSlab = 0;
//////bug 
		while(true)
		{
			//如果容量不够了，重新remalloc
			m_pSlabManagerArray[m_countSlab] = new CLSlabManager(pSlabBeginPos, m_slabSize, m_nodeSize, true, pLogger);//bug m_countslab = 0
			pSlabBeginPos = m_pSlabManagerArray[m_countSlab]->GetNextSlabAddr();
			m_countSlab++;	
			
			if(m_countSlab >= INITARRAY_CAPICITY)
			{
				cout<<"engouth"<<endl;
			}

			if(pSlabBeginPos == NULL)
				break;
		}
	}
};	

CLFreeNodeManagerWithSameSize::~CLFreeNodeManagerWithSameSize()
{

};

/*bool CLFreeNodeManagerWithSameSize::AddFreeNode(SLNode *pNode)
{
	if(pNode == NULL)
		return false;
	
	m_pLogger->WriteLogger(&pNode->m_isUsed, sizeof(bool));
	pNode->m_isUsed = false;
	
	if(*m_pnodeHeadAddr == NULL)
	{
		m_pLogger->WriteLogger(m_pnodeHeadAddr, sizeof(SLNode*));
		*m_pnodeHeadAddr = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));
		(*m_pnodeHeadAddr)->m_pnodePre = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*));
		(*m_pnodeHeadAddr)->m_pnodeNext = pNode;
	}
	else
	{
		m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*));
		pNode->m_pnodeNext = *m_pnodeHeadAddr;
		SLNode *pLastNode = (*m_pnodeHeadAddr)->m_pnodePre;
		m_pLogger->WriteLogger(&((*m_pnodeHeadAddr)->m_pnodePre), sizeof(SLNode*));
		(*m_pnodeHeadAddr)->m_pnodePre = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));
		pNode->m_pnodePre = pLastNode;
		m_pLogger->WriteLogger(&pLastNode->m_pnodeNext, sizeof(SLNode*));
		pLastNode->m_pnodeNext = pNode;
	}
	
	m_countFreeNode++;

	return true;
};
*/

bool CLFreeNodeManagerWithSameSize::AddSlab(void *pBeginPos)
{
	if(pBeginPos == NULL)
		return false;

	if(m_countSlab == 0)
	{
		m_pLogger->WriteLogger(m_pFirstSlabAddr, sizeof(void*));		
		m_pSlabManagerArray[m_countSlab] = new CLSlabManager(pBeginPos, m_slabSize, m_nodeSize, false, m_pLogger);
		*m_pFirstSlabAddr = pBeginPos;
	}
	else
	{
		m_pSlabManagerArray[m_countSlab] = new CLSlabManager(pBeginPos, m_slabSize, m_nodeSize, false, m_pLogger);
		m_pSlabManagerArray[m_countSlab - 1]->AddNextSlabManager(pBeginPos);//这个函数里面会记日志
	}
	
	m_countSlab++;

	return true;
};

SLNode* CLFreeNodeManagerWithSameSize::RequestFreeNode()
{
	if(m_countSlab == 0 || *m_pFirstSlabAddr == NULL)
	{
		return NULL;
	}		
	
	SLNode *pNodeReturn = NULL;
	
	for(int i = 0; i < m_countSlab; i++)
	{
		if((pNodeReturn = m_pSlabManagerArray[i]->Request()) != NULL)
			return pNodeReturn;
	}
	
	return NULL;
};

bool CLFreeNodeManagerWithSameSize::DeleteFreeNode(SLNode *pNode)
{
	if(pNode == NULL || pNode->m_size != m_nodeSize)
		return false;
	
	for(int i = 0; i < m_countSlab; i++)
	{
		if(m_pSlabManagerArray[i]->AddFreeNode(pNode))
			return true;
	}

	return false;
};

/*bool CLFreeNodeManagerWithSameSize::IsEmpty()
{
	if(*m_pnodeHeadAddr == NULL)
		return true;
	else
		return false;
};

int CLFreeNodeManagerWithSameSize::Traverse()
{
	int i = 1;

	SLNode *pTmpNode = *m_pnodeHeadAddr;
	if(pTmpNode == NULL)
		return 0;

	while(pTmpNode->m_pnodeNext != *m_pnodeHeadAddr)
	{
		cout<<"size "<<pTmpNode->m_size<<endl;
		pTmpNode = pTmpNode->m_pnodeNext;
		i++;
	}

	cout<<"size "<<pTmpNode->m_size<<endl;
	
	return i;
};*/
