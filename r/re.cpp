#include "CLSer.h"
#include "CLProtocol.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <iostream>

#define IPCKEY 0x366378

using namespace std;

int main(int argc, char **argv)
{
	CLSer p;
	int shmid;
	shmid = atoi(argv[1]);
char *pt;
void * buf = (void*)(strtol(argv[2], &pt, 0));
cout<<buf<<endl;
	
	if((long)(buf = shmat(shmid, buf, 0)) == -1)
	{
		cout<<"shmat false "<<endl;
	}

	void *pBegin = buf;
	cout<<"id "<<shmid<<"  begin addr "<<(unsigned long)buf<<endl;
	p.SerStart(pBegin, 1024 *1024*1024);
	
	shmdt(pBegin);
	return 0;
}
