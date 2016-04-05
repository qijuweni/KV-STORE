#define SUCCESS 4
#define FAIL 3
#define SETERROR 5

#define SET 0x0A
#define GET 0x02
#define DEL 0x01

typedef char Key[16];

struct SLResponseHead
{
	char status;
	char Opaque;
	unsigned short valueLen;
};

struct SLRequestHead
{
	char opcade;
	char Opaque;
	Key  key;
	unsigned short valueLen;
};

struct SLProtocolPackage
{
	SLRequestHead m_RequestHead;
	char *pValue;
};

class CLProtocol
{
public:
	CLProtocol();
	virtual ~CLProtocol();
	
	SLProtocolPackage* GetPackage(void *pData, int size);
	static void* MakePackage(SLProtocolPackage* pPackage);
	
private:

	int m_restHeadSize;
	int m_restValueSize;
	SLProtocolPackage *m_pPackage;
};
