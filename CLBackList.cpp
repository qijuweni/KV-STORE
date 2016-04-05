#include "CLBackList.h"
#include "SLNode.h"
#include "CLLogger.h"
#include <string.h>
#include <iostream>

using namespace std;

CLBackList::CLBackList(SLNode **pHeadNodeAddr, CLLogger *pLogger, unsigned int minSizeInList, void *begin ,void *end)
{
	m_pHeadNodeAddr = pHeadNodeAddr;
	m_minSizeInList = minSizeInList;
	m_pLogger = pLogger;
	m_begin = begin;
	m_end = end;
};

CLBackList::~CLBackList()
{

};

int CLBackList::Traverse()
{
	int i = 1;

	SLNode *pTmpNode = *m_pHeadNodeAddr;
	if(pTmpNode == NULL)
		return 0;

	while(pTmpNode->m_pnodeNext != *m_pHeadNodeAddr)
	{
		cout<<"size "<<pTmpNode->m_size<<endl;
		pTmpNode = pTmpNode->m_pnodeNext;
		i++;
	}

	cout<<"size "<<pTmpNode->m_size<<endl;
	
	return i;
	
};

SLNode* CLBackList::FindFitNode(unsigned int size)
{
	SLNode *pReturnNode = *m_pHeadNodeAddr;
	
	if(pReturnNode == NULL)
		return NULL;	

	while(pReturnNode->m_size < size)
	{
		pReturnNode = pReturnNode->m_pnodeNext;

		if(pReturnNode == *m_pHeadNodeAddr)
		{
			cout<<"backlist not fitnode"<<endl;
			return NULL;
		}
	}

	return pReturnNode;
};

void CLBackList::Merge(SLNode *pNode)
{
	SLNode *pNextInStore = (SLNode*)((char*)pNode + pNode->m_size + sizeof(SLNode));

	while((char*)pNextInStore + pNextInStore->m_size + sizeof(SLNode) < m_end)
	{
		//SLNode *pNextInStore = (SLNode*)((char*)pTmpNode + pTmpNode->m_size + sizeof(SLNode));				
		if(pNextInStore->m_isUsed == false)
		{
			if(pNextInStore->m_size > m_minSizeInList)
			{
				DeleteNode(pNextInStore);	
			}
			else
			{
				break;
			}
		}	
		else
			break;

		SLNode *pNodeToClear = pNextInStore;
		pNextInStore = (SLNode*)((char*)pNextInStore + sizeof(SLNode) + pNextInStore->m_size); 
		m_pLogger->WriteLogger(&pNode->m_size, sizeof(unsigned int));
		pNode->m_size += sizeof(SLNode) + pNodeToClear->m_size;
		m_pLogger->WriteLogger(&pNodeToClear->m_isUsed, sizeof(bool));
		m_pLogger->WriteLogger(&pNodeToClear->m_size, sizeof(unsigned int ));
	//	m_pLogger->WriteLogger(&pNodeToClear->m_pnodePre, sizeof(SLNode*));
	//	m_pLogger->WriteLogger(&pNodeToClear->m_pnodeNext, sizeof(SLNode*));
	}
};

bool CLBackList::AddNode(SLNode *pNode)
{
//	if(pNode->m_isUsed!=true)
//		cout<<"back add not right"<<endl;

	m_pLogger->WriteLogger(&pNode->m_isUsed, sizeof(bool));
	pNode->m_isUsed = false;	
	
	Merge(pNode);

	if(*m_pHeadNodeAddr == NULL)
	{
		m_pLogger->WriteLogger(m_pHeadNodeAddr, sizeof(SLNode*));
		*m_pHeadNodeAddr = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));
		(*m_pHeadNodeAddr)->m_pnodePre = pNode;
		m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*));
		(*m_pHeadNodeAddr)->m_pnodeNext = pNode;
	}
	else
	{
		SLNode *pNodeNext = FindFitNode(pNode->m_size);
		if(pNodeNext == NULL)
		{		
			m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*));
			pNode->m_pnodeNext = (*m_pHeadNodeAddr);
			SLNode *pLastNode = (*m_pHeadNodeAddr)->m_pnodePre;
			m_pLogger->WriteLogger(&((*m_pHeadNodeAddr)->m_pnodePre), sizeof(SLNode*));
			(*m_pHeadNodeAddr)->m_pnodePre = pNode;
			m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));
			pNode->m_pnodePre = pLastNode;
			m_pLogger->WriteLogger(&pLastNode->m_pnodeNext, sizeof(SLNode*));
			pLastNode->m_pnodeNext = pNode;
		}
		else
		{
			SLNode *pNodePre = pNodeNext->m_pnodePre;
	
			m_pLogger->WriteLogger(&pNode->m_pnodeNext, sizeof(SLNode*));
			pNode->m_pnodeNext = pNodeNext;
			m_pLogger->WriteLogger(&pNodeNext->m_pnodePre, sizeof(SLNode*));
			pNodeNext->m_pnodePre = pNode;
			m_pLogger->WriteLogger(&pNode->m_pnodePre, sizeof(SLNode*));
			pNode->m_pnodePre = pNodePre;	
			m_pLogger->WriteLogger(&pNodePre->m_pnodeNext, sizeof(SLNode*));
			pNodePre->m_pnodeNext = pNode;

			if(pNodeNext == *m_pHeadNodeAddr)
			{
				m_pLogger->WriteLogger(m_pHeadNodeAddr, sizeof(SLNode*));
				*m_pHeadNodeAddr = pNode;
			}
		}
	}
	
	return true;
};

SLNode* CLBackList::GetSLNode(unsigned int size)
{
	SLNode *pNodeToDivide = FindFitNode(size);
	if(pNodeToDivide == NULL)
		return NULL;
	else
	{
		if((int)(pNodeToDivide->m_size - sizeof(SLNode) - size ) >(int)m_minSizeInList)
		{
			SLNode *pReturnSLNode = (SLNode*)((char*)pNodeToDivide + pNodeToDivide->m_size - size);
			pReturnSLNode->m_size = size;
			pReturnSLNode->m_isUsed = true;
			m_pLogger->WriteLogger(&pNodeToDivide->m_size, sizeof(unsigned int));
			pNodeToDivide->m_size = pNodeToDivide->m_size - sizeof(SLNode) - size;
			
			if(pNodeToDivide != *m_pHeadNodeAddr)
			{	
				if(pNodeToDivide->m_size < pNodeToDivide->m_pnodePre->m_size)
				{
					DeleteNode(pNodeToDivide);
					AddNode(pNodeToDivide);
				}
			}
					
			return pReturnSLNode;
		}
		else
		{
			if(DeleteNode(pNodeToDivide))
			{
				m_pLogger->WriteLogger(&pNodeToDivide->m_isUsed, sizeof(bool));
				pNodeToDivide->m_isUsed = true;
				return pNodeToDivide;
			}
			else
				return NULL;
		}
	}

};

bool CLBackList::DeleteNode(SLNode *pNode)
{
	if(pNode->m_pnodeNext == NULL || pNode->m_pnodePre == NULL)
		cout<<"back add not right"<<endl;

	if(pNode == NULL)
		return false;
	
	bool IsHead = false;

	if(pNode == *m_pHeadNodeAddr)
	{
		IsHead = true;

		if((*m_pHeadNodeAddr)->m_pnodeNext == *m_pHeadNodeAddr)
		{
			m_pLogger->WriteLogger(m_pHeadNodeAddr, sizeof(SLNode*));
			*m_pHeadNodeAddr = NULL;
			return true;
		}
	};

	SLNode *pNodeNext = pNode->m_pnodeNext;
	SLNode *pNodePre = pNode->m_pnodePre;
	m_pLogger->WriteLogger(&pNodePre->m_pnodeNext, sizeof(SLNode*));
	pNodePre->m_pnodeNext = pNodeNext;
	m_pLogger->WriteLogger(&pNodeNext->m_pnodePre, sizeof(SLNode*));
	pNodeNext->m_pnodePre = pNodePre;

	

	if(IsHead == true)
	{
		m_pLogger->WriteLogger(m_pHeadNodeAddr, sizeof(SLNode*));
		*m_pHeadNodeAddr = pNodeNext;
	}

	return true;
};

