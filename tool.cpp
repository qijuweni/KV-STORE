#include "string.h"
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

int set_address(char *hname, int port, struct sockaddr_in *sap)
{
 	if(sap == NULL)
		return -1;

 	memset(sap, 0, sizeof(*sap));
        sap->sin_family = AF_INET;

        if(hname != NULL) 
        {
                if(!inet_pton(AF_INET, hname, &sap->sin_addr))
                {
			cout<<"transfer addr false"<<endl;
           		return -1;
	   	}
        }       
        else
                sap->sin_addr.s_addr = htonl(INADDR_ANY);
                  
        if(port < 0 || port > 65535)
	{
		cout<<"port not right"<<endl;
		return -1;
	}
        else    
                sap->sin_port = htons(port);
	return 0;
};


