#include <iostream>
#include "CLSer.h"
#include "CLProtocol.h"
#include <string.h>
#include <unistd.h>
#include  "CLKVStore.h"
#include "CLHashMap.h"

#define CONNECTED 2
#define PORT 9876
#define SET 0x0A
#define GET 0x02
#define SUCCESS 4
#define KEY_MISS 3
#define SETERROR 5

using namespace std;

int set_address(char *hname, int port, struct sockaddr_in *sap);

struct SLPara
{
	CLSer *m_pSer;
	CLProtocol *m_pPro;
};

CLSer::CLSer()
{
};

/*bool CLProtocolSer::makeResponseHead(SLResponseHead * pResponseHead, char status, char Opaque, unsigned short valueLen)
{
	if(pResponseHead == NULL)
		return false;

	pResponseHead->status = status;
	pResponseHead->Opaque = Opaque;
	pResponseHead->valueLen = valueLen;

	return true;
};

bool CLProtocolSer::analysePacket(SLRequestHead * pRequestHead)
{
	SLResponseHead responseHead;
	responseHead.Opaque = pRequestHead->Opaque;  	 		
	responseHead.valueLen = 0;

	switch(pRequestHead->opcade)
	{
	case SET:
		{
			cout<<"set "<<pRequestHead->key<<endl;
			if(pRequestHead->valueLen == 0)
			{
				responseHead.status = SETERROR;
			}
			char * value = new char[pRequestHead->valueLen];				  				  
			if(bufferevent_read(m_bev, value, pRequestHead->valueLen) != pRequestHead->valueLen)
			{
				cout<<"read value false"<<endl;
				responseHead.status = SETERROR;
			}
			if(m_store->set(pRequestHead->key, sizeof(pRequestHead->key), value, pRequestHead->valueLen))
			{
				responseHead.status = SUCCESS;
			}
			else
			{
				responseHead.status = SETERROR;
			}
			delete value;
			bufferevent_write(m_bev, &responseHead, sizeof(responseHead));
			if(responseHead.status == SETERROR)
				return false;
			break;
		}
	case GET:
		{
			cout<<"GET "<<pRequestHead->key<<endl;
			const char * value;
			int valueLen;
			if(m_store->get(pRequestHead->key, &value, &valueLen))
			{	
				responseHead.status = SUCCESS;
				responseHead.valueLen = valueLen;
				if(value != NULL && responseHead.valueLen > 0)
				{
					bufferevent_write(m_bev, &responseHead, sizeof(responseHead));
					bufferevent_write(m_bev, value, valueLen);
					delete value;
				}
				else
				{
					responseHead.status  = KEY_MISS;
					bufferevent_write(m_bev, &responseHead, sizeof(responseHead));
					return false;
				}
			}
		       	else
			{
				responseHead.status  = KEY_MISS;
				bufferevent_write(m_bev, &responseHead, sizeof(responseHead));
				return false;
			}	
			break;
		}
	default:
		{
			cout<<"request not right"<<endl;
			return false;
		}
	}
	return true;
};
*/

void CLSer::write_cb(struct bufferevent * bev,void * ctx)
{
//	cout<<"write "<<endl;

//	bufferevent_write(bev, pResPack, len);
};

void CLSer::read_cb(struct bufferevent * bev,void * ctx)
{
	char buffer[READ_BUFFER_SIZE] = { 0 };
	size_t lenData = bufferevent_read(bev, buffer, READ_BUFFER_SIZE);
//	if(lenData != sizeof(SLRequestHead))
//		cout<<"read requestHead error"<<endl;
	
	SLPara *p = (SLPara*)ctx;
	CLProtocol *pPro = p->m_pPro;
//	cout<<"read_cb"<<endl;

	SLProtocolPackage *pPackage = pPro->GetPackage(&buffer, lenData);
	
	if(pPackage != NULL)
	{
/*		unsigned short len = 0;
		void *pResPack = pPro->MakePackage( SUCCESS, pPackage->m_RequestHead.Opaque, &len, NULL);
		if(pResPack == NULL || len == 0)
			cout<<"ResPack wrong"<<endl;
		bufferevent_write(bev, pResPack, len);
		cout<<"write ok"<<endl;*/
		SLTask *pTask = new SLTask;
		pTask->pPackage = pPackage;
		pTask->bev = bev;
		p->m_pSer->m_store->InsertTask(pTask);
	}
	else
	{
		cout<<"not full pPackage"<<endl;
	}
};

void CLSer::accept_conn_cb(struct evconnlistener * listener, evutil_socket_t fd, struct sockaddr * address, int socklen, void * ctx)
{
	(void )socklen;

	cout<<"accept "<<ntohs(((sockaddr_in*)address)->sin_port)<<endl;
	struct event_base * base = evconnlistener_get_base(listener);
//	CLSer * p = (CLSer*)ctx;
	bufferevent *pBufferev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	
	SLPara *pPara = new SLPara;
	pPara->m_pSer = (CLSer*)ctx;
	pPara->m_pPro = new CLProtocol;

	bufferevent_setcb(pBufferev, read_cb, write_cb, NULL, pPara);
	bufferevent_enable(pBufferev, EV_READ | EV_WRITE);
//	SLResponseHead responseHead;
//	p->makeResponseHead(&responseHead, CONNECTED, '2', 0);

	//      cout<<"status"<<responseHead.status + '0'<<"Opaque"<<responseHead.Opaque<<endl;
//	bufferevent_write(p->m_bev, &responseHead, sizeof(responseHead));
};

void CLSer::read_cb_pipe(struct bufferevent * bev,void * ctx)
{
//	cout<<"read pipe"<<endl;	
	SLTask *pTask ;
	size_t lenData = bufferevent_read(bev, &pTask, sizeof(SLTask*));
	if(lenData != sizeof(SLTask*))
		cout<<"read_PIPE requestHead error"<<endl;
	
	void* p = CLProtocol::MakePackage(pTask->pPackage);
	int size = ((SLResponseHead*)p)->valueLen + sizeof(SLResponseHead);
	bufferevent_write(pTask->bev, p, size);
//	cout<<"pipe ok"<<endl;
};

bool CLSer::SerStart(void *pBeginpos, unsigned long size)
{
	struct event_base * base;
	struct evconnlistener * listener;
	struct sockaddr_in sin;

	base = event_base_new();
	if(!base)
	{   
		cout<<"mainbase false"<<endl;
		return false;
	}   

	pipe(m_fd);
	bufferevent *pBufferev = bufferevent_socket_new(base, m_fd[0], BEV_OPT_CLOSE_ON_FREE);	
	bufferevent_setcb(pBufferev, read_cb_pipe, NULL, NULL, NULL);

	bufferevent_enable(pBufferev, EV_READ | EV_WRITE);
	m_store = new CLKVStore(pBeginpos,size,m_fd[1]);

	if(set_address(NULL, PORT, &sin) == -1) 
	{   
		cout<<"setaddress false"<<endl;
		return false;
		//                return -1; 
	}   

	listener = evconnlistener_new_bind(base, accept_conn_cb, this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

	if(!listener)
	{   
		cout<<"create listener false"<<endl;
		return false; 
	}   
		
	event_base_dispatch(base);
	return true;
};














