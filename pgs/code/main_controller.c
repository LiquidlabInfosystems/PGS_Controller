#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/msg.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <pthread.h>

//#include <stddef.h>

#define SERVER "192.168.100.1"
#define DEBUGGING_IP "192.168.100.254"
#define BUFLEN 256
#define PORT_FOR_LC_COMMUNICATION 7777

int portno,total_spaces,total_levels;
int sockfd_main_controller,sockfd_udp_server;
struct sockaddr_in host_address;
struct sockaddr_in sender_address;
struct sockaddr_in serv_addr;
struct hostent *server;

char buffer[256];
char udp_buffer[256];

//char *full="Full";
//char *empty="Empty";
//char *spaces="Spaces";

int *free_space_per_level;
int *zones_per_level;
int ***free_space_per_zones_per_level;

//char *mesg_per_level;
//char ***mesg_per_zones_per_level;

const char* server_ip;

pthread_t thread_id;

struct my_msg_st
{
long int my_msg_type;
char some_text[BUFSIZ];
};

void activate_udp_port()
{
	printf("Entered activate_udp_port\n");
	int sender_address_length = sizeof(sender_address);
	int recv_length;
	if((sockfd_main_controller=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("socket error");
		exit(1);
	}
	memset((char*)&host_address,0,sizeof(host_address));
	host_address.sin_family = AF_INET;
	host_address.sin_port = htons(PORT_FOR_LC_COMMUNICATION);
	host_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(sockfd_main_controller,(struct sockaddr*)&host_address,sizeof(host_address)) == -1)
	{
		perror("bind error");
		exit(1);
	}
	
	while(1)
	{
		printf("Test\n");
		fflush(stdout);
		 //try to receive some data, this is a blocking call
        if ((recv_length = recvfrom(sockfd_main_controller, udp_buffer, BUFLEN, 0, (struct sockaddr *) &sender_address, &sender_address_length)) == -1)
        {
            	perror("recvfrom() error");
	   	exit(1);
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(sender_address.sin_addr), ntohs(sender_address.sin_port));
        printf("%s\n" , udp_buffer);
	//update_level_zone_count(udp_buffer);
	send_data_to_udp_server(udp_buffer);
        send_data_to_udp_server_debugging(udp_buffer);
	bzero(udp_buffer,256); 
        //now reply the client with the same data
     //   if (sendto(sockfd_main_controller, udp_buffer, recv_length, 0, (struct sockaddr*) &sender_address, sender_address_lenth) == -1)
       // {
         //   perror("sendto() error");
	//	exit(1);
       // }
    }
 
	close(sockfd_main_controller);
}

send_data_to_udp_server(char arr[256])
{
	sockfd_udp_server = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
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
	
	sendto(sockfd_udp_server,arr,strlen(arr),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	bzero(buffer,256);
	close(sockfd_udp_server);

}
send_data_to_udp_server_debugging(char arr[256])
{
        sockfd_udp_server = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
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

        sendto(sockfd_udp_server,arr,strlen(arr),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        bzero(buffer,256);
        close(sockfd_udp_server);

}


void  update_level_zone_count(char arr[256])
{
//	printf("test\n");
	int i=0;
	int k=0;
	int j=0;
	int msg_no,level_no,level_free_space,zone_no,zone_free_space;
	char * level_status,*zone_status;	
	char * temp;
	char ** value = malloc(3* sizeof(char*));
	char string[256];
	strcpy(string,arr);
//	printf("test %s\n",string);
	//value = strtok(string,"-");
	//printf("value %s\n",value);
	for (temp = strtok(string,"-"); temp!= NULL; temp = strtok(NULL, "-"))
		{
 		 value[i]=temp;
        printf("value[%d]=%s\n",i,value[i]);
		i++;
		}
printf("i=%d\n",i);








	msg_no=atoi(value[0]);
	switch(msg_no)
	{
		case 22:
		zone_status = "Spaces";
		zone_free_space = atoi(value[3]);
		*((*(free_space_per_zones_per_level+(level_no-1)))+(zone_no-1))=zone_free_space;
		break;

		case 23:
		zone_status = "Full";
		break;

		case 24:
		zone_status = "Empty";
		break;

		case 25:
		level_status = "Spaces";
		level_free_space = atoi(value[2]);
		*(free_space_per_level+(level_no-1))=level_free_space;		
		break;

		case 26:
		level_status = "Full";
		break;

		case 27:
		level_status = "Empty";
		break;
	}	


























/*
	if (i<3)
	{
		level_no=atoi(value[0]);
		printf("level no %d\n",level_no);
		if(strcmp(value[1],"Full")==0)
		{
			
			level_status = value[1];
			printf("Level full\n");		
		
		}
		else if(strcmp(value[1],"Empty")==0)
		{
			level_status = value[1];
			printf("Level Empty\n");
		}
		else
		{
			level_status = "Spaces";
			level_free_space = atoi(value[1]);
			*(free_space_per_level+(level_no-1))=level_free_space;		

			//printf("level free space = %d\n",level_free_space);		
		}


	}		
	else
	{
		level_no=atoi(value[0]);
		zone_no=atoi(value[1]);
		if(strcmp(value[2],"Full")==0)
			zone_status = value[2];
		else if (strcmp(value[2],"Empty")==0)
			zone_status = value[2];
		else
		{
			zone_status = "Spaces";
			zone_free_space = atoi(value[2]);
			*((*(free_space_per_zones_per_level+(level_no-1)))+(zone_no-1))=zone_free_space;
		}
	}
		
	*/

//free(free_space_per_zones_per_level);
//free(free_space_per_level);
for(j=0;j<total_levels;j++)
	{
	       printf("Level%d spaces = %d\n",j+1,*(free_space_per_level+j));
		for(k=1;k<=*(zones_per_level+j);k++)
		printf("Level%d zone%d spaces = %d\n",j+1,k,*((*(free_space_per_zones_per_level+(j))+(k-1))));
		
	}
free(value);

}

int main(int argc,char *argv[] )

{

	int i,temp,thread_error;
	server=gethostbyname(argv[1]);
	server_ip=argv[1];
	portno=atoi(argv[2]);
	printf("udp port %d\n",portno);
	total_spaces=atoi(argv[3]);
	printf("total space %d\n",total_spaces);
	total_levels=atoi(argv[4]);
	printf("total level %d\n",total_levels);

free_space_per_zones_per_level=calloc(total_levels,sizeof(int*));
free_space_per_level=calloc(total_levels,sizeof(int*));
zones_per_level=calloc(total_levels,sizeof(int*));
for (i=0;i<total_levels;i++)
	{	
	*(zones_per_level+i)=atoi(argv[5+i]);
	free_space_per_zones_per_level[i]=calloc((*(zones_per_level+i)),sizeof(int));	
	}

//printf("Zones in L1 %d & L2 %d\n",*zones_per_level[0],*zones_per_level[1]);
if(total_levels > 1)
{
//activate_udp_port();
thread_error=pthread_create(&thread_id,NULL,(void*)&activate_udp_port,NULL);
printf("Thread created\n");
}

int running = 1;
int msgid;
struct my_msg_st lc_ipc_data;
long int msg_to_receive = 0;

 

 msgid = msgget( (key_t)1234,
0666 | IPC_CREAT);
if (msgid == -1)
{
fprintf(stderr, "failed to get:\n");
free(free_space_per_zones_per_level);
free(free_space_per_level);
exit(EXIT_FAILURE);
}

send_data_to_udp_server("Main Thread Started");
send_data_to_udp_server_debugging("Main Thread Started");
 while (running)
{
if(msgrcv(msgid, (void *)&lc_ipc_data,
BUFSIZ,msg_to_receive,0)  == -1)

{
fprintf(stderr, "failedto receive: \n");
free(free_space_per_zones_per_level);
free(free_space_per_level);
exit(EXIT_FAILURE);
}

//printf("You Wrote: %s\n",  
//lc_ipc_data.some_text);
//update_level_zone_count(lc_ipc_data.some_text);
send_data_to_udp_server(lc_ipc_data.some_text);
send_data_to_udp_server_debugging(lc_ipc_data.some_text);
if(strncmp(lc_ipc_data.some_text, "end", 3)
== 0)
{
running = 0;

}
}
if (msgctl(msgid, IPC_RMID, 0) == -1)
{
fprintf(stderr, "failed to delete\n");
free(free_space_per_zones_per_level);
free(free_space_per_level);
exit(EXIT_FAILURE);
}
free(free_space_per_zones_per_level);
free(free_space_per_level);
exit(EXIT_SUCCESS);
}

