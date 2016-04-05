#include "CLSer.h"
#include "CLProtocol.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <iostream>

#define IPCKEY 0x366378

using namespace std;

int main()
{
	CLSer p;
	int shmid;
	unsigned long size = 1024 * 1024 * 1024 ;
	
	shmid = shmget(IPCKEY, size, IPC_CREAT|0666);
	if(shmid == -1)
	{
		cout<<"shmget false"<<endl;
	}


	void *pBegin;
	
	if((long)(pBegin = shmat(shmid, NULL, 0)) == -1)
	{
		cout<<"shmat false"<<endl;
	}

	unsigned long v = (unsigned long)pBegin;

	cout<<"id "<<shmid<<"  begin addr "<<v<<endl;
	p.SerStart(pBegin, size);
	
	shmdt(pBegin);
	return 0;
}
