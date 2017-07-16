#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

static struct sockaddr_in serv_addr_udp;
int sockfd_udp,portno;
struct hostent *server;
struct sockaddr_in serv_addr;
const char* server_ip;
char msg_buffer[256];


#define DEBUGGING_IP "192.168.100.254"

send_data_to_udp_server(char arr[256])
{
	sockfd_udp= socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (socket <0)
	error("Error opening socket");

	if(server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
//	bzero((char*) & serv_addr,sizeof(serv_addr));
	memset ((char*)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
//	bcopy((char *)server->h_addr,
  //      (char *)&serv_addr.sin_addr.s_addr,
   //     server->h_length);
	serv_addr.sin_port = htons(portno);
	
	if(inet_aton(server_ip,&serv_addr.sin_addr) == 0)
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	
	sendto(sockfd_udp,arr,strlen(arr),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	bzero(arr,256);
	close(sockfd_udp);

}


send_data_to_udp_server_debugging(char arr[256])
{
        sockfd_udp= socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (socket <0)
        error("Error opening socket");

        if(server == NULL)
        {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
//      bzero((char*) & serv_addr,sizeof(serv_addr));
        memset ((char*)&serv_addr,0,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
//      bcopy((char *)server->h_addr,
  //      (char *)&serv_addr.sin_addr.s_addr,
   //     server->h_length);
        serv_addr.sin_port = htons(portno);

        if(inet_aton(DEBUGGING_IP,&serv_addr.sin_addr) == 0)
        { 
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
        }
        
        sendto(sockfd_udp,arr,strlen(arr),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        bzero(arr,256);
        close(sockfd_udp);

}




int main ( int argc, char *argv[] )
{
	char* data;
	int i, data_argument_count;
	data= (char *)malloc(sizeof(char)*256);
	server_ip=argv[1];
	server=gethostbyname(argv[1]);
	portno = atoi(argv[2]);
	
	data_argument_count=argc-3;
	for(i=3;i<argc;i++)
		{
		data=strcat(data,argv[i]);
		data=strcat(data," ");
		}
	snprintf(msg_buffer,256,"%s",data);
	send_data_to_udp_server(msg_buffer);
	snprintf(msg_buffer,256,"%s",data);
	send_data_to_udp_server_debugging(msg_buffer);
	free(data);
	return 0;
}
	



