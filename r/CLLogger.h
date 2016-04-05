#ifndef LOGGER
#define LOGGER

#include <unordered_set>

#define RECORD_LEN 16
#define ADDR_MASK 0xfeffffffffffffff
#define FLAG_MASK 0x0100000000000000
	
class CLLogger
{
public:
	CLLogger(void *pvLoggerBeginPos, int loggerCapacity, bool isRecovering);
	virtual ~CLLogger();

	bool WriteLogger(void *pvAddress, int size);
	bool IsHadWriteInSameAddr(void *pvAddress);
	bool ClearLogger();
	bool Recover();

protected:
	void * FindFirstFreeSpace(int countOfRequestItem);

private:
	void *m_pvLoggerBeginPos;
//	int m_nowCountOfLoggerItem;
	int m_loggerCapacity;

	std::unordered_set<void *> m_setAddressRegister;
};

#endif
