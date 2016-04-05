#include "CLProtocol.h"
#include <iostream>
#include <string.h>

using namespace std;

CLProtocol::CLProtocol()
{
	m_restHeadSize = sizeof(SLRequestHead);
	m_restValueSize = -1;
	m_pPackage = new SLProtocolPackage;
};

CLProtocol::~CLProtocol()
{

};

SLProtocolPackage* CLProtocol::GetPackage( void *pData, int size ) //目前来看pdata顶多会创建一个完整的数据报
{
	if(m_pPackage == NULL)
		m_pPackage = new SLProtocolPackage;

//	while(size != 0)
//	{
		if(m_restHeadSize != 0)
		{
			void *pNowHeadPos = (char*)(&m_pPackage->m_RequestHead) + sizeof(SLRequestHead) - m_restHeadSize;

			if(size > m_restHeadSize)
			{
				memcpy(pNowHeadPos, pData, m_restHeadSize);
				size = size - m_restHeadSize;
				pData = (char*)pData + m_restHeadSize;
				m_restHeadSize = 0;
			}	
			else
			{
				memcpy(pNowHeadPos, pData, size);
				m_restHeadSize = m_restHeadSize - size;
				
				if(m_restHeadSize == 0 && m_pPackage->m_RequestHead.valueLen == 0)
				{
					SLProtocolPackage *pPackage = m_pPackage;
					m_pPackage = new SLProtocolPackage;

					m_restHeadSize = sizeof(SLRequestHead);

					return pPackage;
				}

				return NULL;
			}
		}

	//	if(size != 0)
	//	{
			if(m_restValueSize == -1)
			{
				m_restValueSize = m_pPackage->m_RequestHead.valueLen;
				m_pPackage->pValue = new char[m_restValueSize];
			}
	
			void *pNowValuePos = m_pPackage->pValue + m_pPackage->m_RequestHead.valueLen - m_restValueSize;

			if(size > m_restValueSize)
			{
				memcpy(pNowValuePos, pData, m_restValueSize);	
				size = size - m_restValueSize;
				m_restValueSize = -1; 
				
				cout<<"data have more one package"<<endl;
				return NULL;
			}
			else
			{
				memcpy(pNowValuePos, pData, size);
				m_restValueSize = m_restValueSize - size;

				if(m_restValueSize == 0)
				{
					m_restHeadSize = sizeof(SLRequestHead);
					m_restValueSize = -1;

					SLProtocolPackage *pPackage = m_pPackage;
					m_pPackage = new SLProtocolPackage;

					return pPackage;
				}

				return NULL;
			}	
//		}
//	}
};

void* CLProtocol::MakePackage(SLProtocolPackage *pPackage)
{
	char *pValue = pPackage->pValue;
	int valueLen = 0;

	if(pValue != NULL)
		valueLen = strlen(pValue) + 1;

	char *pReturn = new char[sizeof(SLResponseHead) + valueLen];
	if(pReturn == NULL)
		return NULL;

	SLResponseHead *pResHead = (SLResponseHead*)pReturn;
	
	pResHead->status = 4;
	pResHead->Opaque = pPackage->m_RequestHead.Opaque;
	pResHead->valueLen = valueLen;

	if(valueLen != 0)
	{
		memcpy((void*)(pResHead + 1), pValue, valueLen);
	}

	return pReturn;
};
