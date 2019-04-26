
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

using namespace std;

#define MAXBUF (1024*1024*5)


int main()
{
    cout<<"0000000000000"<<endl;
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAXBUF+1] = {0};
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout<<"create socket error."<<endl;
        exit(1);
    }
                                        }
    return 0;
}
