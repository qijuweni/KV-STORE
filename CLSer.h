#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#define READ_BUFFER_SIZE 1024

typedef char Key[16];
class CLKVStore;

class CLSer
{
public:
	CLSer();
	bool SerStart(void *pBeginpos, unsigned long size);//bufferevent b);
	static void accept_conn_cb(struct evconnlistener * listener, evutil_socket_t fd, struct sockaddr * address, int socklen, void * ctx);
//   参数可以使用不完整的类型
	static void read_cb(struct bufferevent * bev,void * ctx);
	static void write_cb(struct bufferevent * bev,void * ctx);
	static void read_cb_pipe(struct bufferevent * bev,void * ctx);
//	bool analysePacket(SLRequestHead * pRequestHead);
//	bool  makeResponseHead(SLResponseHead * pResponseHead, char status, char Opaque, unsigned short valueLen);
private:
//	struct bufferevent * m_bev;    ///不要指针不行啦
	CLKVStore  * m_store;
	int m_fd[2];	
};

