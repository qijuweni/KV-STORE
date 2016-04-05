#include "CLLogger.h"
#include <iostream>
#include <string.h>

using namespace std;

CLLogger::CLLogger(void * pvLoggerBeginPos, int loggerCapacity, bool isRecovering)
{
	m_pvLoggerBeginPos = pvLoggerBeginPos;
//	m_nowCountOfLoggerItem = nowCountOfLoggerItem;
	m_loggerCapacity = loggerCapacity;
	if(!isRecovering)
		*((unsigned long*)pvLoggerBeginPos) = 0xffffffffffffffff;
	else
		Recover();
};

CLLogger::~CLLogger()
{
};

void * CLLogger::FindFirstFreeSpace(int countOfRequestItem)
{
	unsigned long *ulPosFree = (unsigned long*)m_pvLoggerBeginPos;

	char *pcEndofLogger = (char*)m_pvLoggerBeginPos + m_loggerCapacity;

	for(; (unsigned long)(ulPosFree + countOfRequestItem) <= (unsigned long)pcEndofLogger;)
	{
		if(* ulPosFree == 0xffffffffffffffff)
			break;
		
		ulPosFree++;
	}

	if((unsigned long)(ulPosFree + countOfRequestItem) <= (unsigned long)pcEndofLogger)
	{
		if((unsigned long)(ulPosFree + countOfRequestItem * 2) <= (unsigned long)pcEndofLogger)
			*(ulPosFree + countOfRequestItem * 2) = 0xffffffffffffffff;

		return (void*)ulPosFree;
	}

	cout<<" space not enough"<<endl;
	return NULL;
};

bool CLLogger::WriteLogger(void *pvAddress, int size)  ///假设要改变的字节都是小于等于8字节的
{
	if(pvAddress <= m_pvLoggerBeginPos)
	{
		cout<<"WriteLogger false"<<endl;
		return false;
	}

	unsigned long addr =(unsigned long)pvAddress;	
	addr = addr / 8 * 8;

	int countOfRequestItem = 0;

	if(addr + 8 >= (unsigned long)pvAddress + size)
	{
		if(!IsHadWriteInSameAddr((void*)(addr - (unsigned long)m_pvLoggerBeginPos)))
			countOfRequestItem = 1;
		else
			return true;
	}
	else
	{
		if(!IsHadWriteInSameAddr((void*)(addr - (unsigned long)m_pvLoggerBeginPos)))
			countOfRequestItem = 1;
		else
			countOfRequestItem = 0;

		if(!IsHadWriteInSameAddr((void*)(addr + 8 - (unsigned long)m_pvLoggerBeginPos)))
		{
			countOfRequestItem++;
			if(countOfRequestItem == 1)
				addr = addr + 8;
		}
		else
		{
			if(countOfRequestItem == 0)
				return true;
		}
		
	}

	unsigned long *pulLoggerAddr = NULL;
	if((pulLoggerAddr = (unsigned long*)FindFirstFreeSpace(countOfRequestItem)) == NULL)
		return false;

	//cout<<"write addr "<<addr<<endl;
	//cout<<"addrfromBegin "<<addr - (unsigned long)m_pvLoggerBeginPos<<endl;

	unsigned long * pValue = (unsigned long*)addr;
	//cout<<"value "<< *value<<endl;
	
/*	if(m_loggerCapacity <= m_nowCountOfLoggerItem * RECORD_LEN)
	{
		cout<<"loggerCapacity is fulled"<<endl;
		return false;
	}
*/	
	//cout<<"loggerallocateaddr "<<(unsigned long)pulLoggerAddr;
	*pulLoggerAddr = addr - (unsigned long)m_pvLoggerBeginPos;
	//cout<<"*addr "<<*pulLoggerAddr;
	*(pulLoggerAddr + 1) = *pValue;
	m_setAddressRegister.insert((void *)(addr - (unsigned long)m_pvLoggerBeginPos));
	//cout<<"*value "<<*(pulLoggerAddr+1)<<endl;

	//memcpy((char*)m_pvLoggerBeginPos + m_nowCountOfLoggerItem * RECORD_LEN, &addr, sizeof(void *));
	//memcpy((char*)m_pvLoggerBeginPos + m_nowCountOfLoggerItem * RECORD_LEN + sizeof(void *), pvAddress, sizeof(unsigned long));	
//	++m_nowCountOfLoggerItem;	

	*((char*)pulLoggerAddr + 7) = 1;   //有效位置1
	
	if(countOfRequestItem == 2)
	{
		addr = addr + 8;		
		*(pulLoggerAddr + 2) = addr - (unsigned long)m_pvLoggerBeginPos;
		*(pulLoggerAddr + 3) = *((unsigned long*)addr);
		m_setAddressRegister.insert((void *)(addr - (unsigned long)m_pvLoggerBeginPos));
		*((char*)(pulLoggerAddr + 2) + 7) = 1;  //有效位置1
	}

	//cout<<"*addr "<<*pulLoggerAddr;
	//cout<<"*value "<<*(pulLoggerAddr+1)<<endl;
	//cout<<"   "<<*(pulLoggerAddr + 2)<<endl;
	//cout<<"*value "<<*(pulLoggerAddr+3)<<endl;
	return true;
};

bool CLLogger::IsHadWriteInSameAddr(void *pvAddress)
{
	unordered_set<void *>::iterator it;
	it = m_setAddressRegister.find(pvAddress);
	if(it != m_setAddressRegister.end())
	{
		return true;
	}
	else
		return false;
		
};

bool CLLogger::ClearLogger()
{
//	cout<<"begin "<<(long)m_pvLoggerBeginPos<<endl;;
	m_setAddressRegister.clear();
	memset(m_pvLoggerBeginPos, 0, m_loggerCapacity);
	*(unsigned long*)m_pvLoggerBeginPos = 0xffffffffffffffff;
	return true;
};

bool CLLogger::Recover()
{
	//cout<<"recover"<<endl;
	//cout<<"begin "<<(long)m_pvLoggerBeginPos<<endl;
	//cout<<"offset "<<posOffset<<endl;
	
//	unsigned long *pulLoggerAddr = (unsigned long*)m_pvLoggerBeginPos;
//	cout<<"*value "<<*(pulLoggerAddr+0)<<endl;
//	cout<<"*value "<<*(pulLoggerAddr+1)<<endl;
//	cout<<"*value "<<*(pulLoggerAddr+2)<<endl;
//	cout<<"*value "<<*(pulLoggerAddr+3)<<endl;
//	cout<<"*value "<<*(pulLoggerAddr+4)<<endl;
//	cout<<"*value "<<*(pulLoggerAddr+5)<<endl;

	for(int i = 0; ; i++)
	{
		unsigned long * oldaddraddr = (unsigned long *)((char *)m_pvLoggerBeginPos + i * RECORD_LEN);
		if((int)((unsigned long)oldaddraddr - (unsigned long)m_pvLoggerBeginPos) >= m_loggerCapacity)
		{
			ClearLogger();
			return true;
		}
//		cout<<"oldaddraddr "<<(unsigned long)oldaddraddr<<endl;;
		unsigned long oldAddr = *(unsigned long *)((char *)m_pvLoggerBeginPos + i * RECORD_LEN);
//		cout<<"old addr "<<oldAddr<<endl;
		if((unsigned long)oldAddr == 0xffffffffffffffff)
		{
			ClearLogger();
			return true;
		}
		
		unsigned long ulIsRight = oldAddr & 0xff00000000000000;
//		cout<<"flag "<<ulIsRight<<endl;
		if(ulIsRight == 0)	
			continue;/////

		oldAddr = oldAddr & ADDR_MASK;
		
//		oldaddraddr = (unsigned long*)((char *)m_pvLoggerBeginPos + i * RECORD_LEN + sizeof(void*));
//		cout<<"value addr"<<(unsigned long)oldaddraddr<<endl;
		unsigned long value = *(unsigned long*)((char *)m_pvLoggerBeginPos + i * RECORD_LEN + sizeof(void*));
//		cout<<"value "<<value<<endl;
		unsigned long *newAddr = (unsigned long*)(oldAddr + (unsigned long)m_pvLoggerBeginPos);
//		cout<<"new addr"<<(unsigned long)newAddr<<endl;
	
		*newAddr = value;
		//memcpy((void*)(*(unsigned long *)((char *)m_pvLoggerBeginPos + i * RECORD_LEN)),(void*)((char *)m_pvLoggerBeginPos + i * RECORD_LEN + sizeof(void*)), sizeof(unsigned long));
	}

//	return true;
};
