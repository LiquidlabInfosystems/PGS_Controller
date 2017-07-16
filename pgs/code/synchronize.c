#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int sockfd_udp,portno;
static struct sockaddr_in serv_addr_udp;
struct hostent *server;
struct sockaddr_in serv_addr;
const char* server_ip;
FILE* datafile_ptr;
FILE* zone_space_datafile_ptr;
char* data;
#define DEBUGGING_IP "192.168.100.254"


//void send_data_to_udp_server(char *);
//void send_data_to_udp_server_debugging(char *);


void read_from_datafile(int zone_index,int cntrl_no)
{

//	printf("Test\n");
	int i,zone_occupancy,total_zone_space,zone_free_space;

	int level_free_space=0;
	int total_level_space=0;
	int level_occupancy=0;
	datafile_ptr = fopen("/home/pgs/data/new_data.bin","rb");
	zone_space_datafile_ptr=fopen("/home/pgs/data/zone_space_data.bin","rb");
	for(i=0;i<=zone_index;i++)
	{	
//		printf("i=%d\n",i);
		fseek(datafile_ptr,(i*4),SEEK_SET);
//		printf("Test3\n");
		fseek(zone_space_datafile_ptr,(i*4),SEEK_SET);
//		printf("Test2\n");
		fread(&zone_occupancy,sizeof(int),1,datafile_ptr);
		fread(&total_zone_space,sizeof(int),1,zone_space_datafile_ptr);
//		printf("Test1\n");
		if(zone_occupancy>total_zone_space)
		snprintf(data,256,"23-%d-%d",cntrl_no,i+1);
	        else if (zone_occupancy<=0)
		snprintf(data,256,"24-%d-%d",cntrl_no,i+1);
		else
		{
			zone_free_space=total_zone_space-zone_occupancy;
			snprintf(data,256,"22-%d-%d-%d",cntrl_no,i+1,zone_free_space);
		}
		send_data_to_udp_server(data);
		send_data_to_udp_server_debugging(data);
		printf("%s\n",data);
		total_level_space+=total_zone_space;
		level_occupancy+=zone_occupancy;


	}
	if(level_occupancy>total_level_space)
	snprintf(data,256,"26-%d",cntrl_no);
	else if(level_occupancy<=0)
	snprintf(data,256,"27-%d",cntrl_no);
	else
	{
	level_free_space=total_level_space-level_occupancy;
	snprintf(data,256,"25-%d-%d",cntrl_no,level_free_space);
	}
	send_data_to_udp_server(data);
	send_data_to_udp_server_debugging(data);
	fclose(datafile_ptr);
	fclose(zone_space_datafile_ptr);
	
}




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
	//bzero(arr,256);
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
//	bzero((char*) & serv_addr,sizeof(serv_addr));
	memset ((char*)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
//	bcopy((char *)server->h_addr,
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

	int controller_number,zone_number,zone_occupancy;
	

	int i, data_argument_count;
	data= (char *)malloc(sizeof(char)*256);
	server_ip=argv[1];
	server=gethostbyname(argv[1]);
	portno = atoi(argv[2]);

	controller_number=atoi(argv[3]);
	zone_number=atoi(argv[4]);
//	printf("zone_number=%d\n",zone_number);
	read_from_datafile(zone_number-1,controller_number);
	
	
	return 0;
}

