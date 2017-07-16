#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <errno.h>


#define SIG_DT 44 
#define MSG_SIZE 256
#define MAIN_CONTROLLER_UDP_PORT 7777
#define DEBUGGING_IP "192.168.100.254"

int controller_no;
int is_main_controller;

int sockfd,portno,n,sockfd_udp,portno_udp;
struct sockaddr_in serv_addr;
static struct sockaddr_in serv_addr_udp;
struct hostent *server,*server_udp;
//struct hostent *main_controller_ip;
char buffer[256],buffer_udp[256];
//char * dataptr;
char data[256];
static char alive_msg[256];

const char* main_controller_ip;
const char* server_ip;

struct itimerval old;
struct itimerval new;

int controller_no;
int total_spaces_in_level;
static int total_free_in_level;
static int total_occupied_in_level;
int total_zones;
//int *total_spaces_in_zone;
int* zones_per_level;
int* zone_occupancy;
int zone_index;

int total_levels;
static int total_zones_in_parking;
int total_detectors;
int entry_detectors[10][10];
int exit_detectors[10][10];


static struct zone_data{
	int total_space;
	int total_free;
	int total_occupied;
	int total_entry;
	int total_exit;
}zone_data_arr[10];

struct zone_data * zone_data_ptr=zone_data_arr;


static int a[20];
int *detector_ptr[20];

struct ipc_msg_st ipc_data;
int msgid;
char ipc_msg_buffer[256];
struct ipc_msg_st
{
	long int ipc_msg_type;
	char ipc_msg[MSG_SIZE];
};


FILE* datafile_ptr;




void send_data_to_main_controller_udp (char arr[256])
{
 //	printf("data sent to main controller through udp\n");
	sockfd_udp = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (socket <0)
        error("Error opening socket");
//	printf("UDP socket created %d\n",sockfd_udp);
        if(server == NULL)
        {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
//      bzero((char*) & serv_addr,sizeof(serv_addr_udp));
        memset ((char*)&serv_addr_udp,0,sizeof(serv_addr_udp));
        serv_addr_udp.sin_family = AF_INET;
//      bcopy((char *)server_udp->h_addr,
  //      (char *)&serv_addr_udp.sin_addr.s_addr,
   //     server_udp->h_length);
        serv_addr_udp.sin_port = htons(MAIN_CONTROLLER_UDP_PORT);

        if(inet_aton(main_controller_ip,&serv_addr_udp.sin_addr) == 0)
        {
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
        }
 sendto(sockfd_udp,arr,strlen(arr),0,(struct sockaddr*)&serv_addr_udp,sizeof(serv_addr_udp));
        bzero(buffer_udp,256);
        close(sockfd_udp);


}

void send_data_to_server_udp(char arr[256])
{
 sockfd_udp = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (socket <0)
        error("Error opening socket");
//	printf("UDP socket created %d\n",sockfd_udp);
        if(server == NULL)
        {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
//      bzero((char*) & serv_addr,sizeof(serv_addr_udp));
        memset ((char*)&serv_addr_udp,0,sizeof(serv_addr_udp));
        serv_addr_udp.sin_family = AF_INET;
//      bcopy((char *)server_udp->h_addr,
  //      (char *)&serv_addr_udp.sin_addr.s_addr,
   //     server_udp->h_length);
        serv_addr_udp.sin_port = htons(portno_udp);

        if(inet_aton(server_ip,&serv_addr_udp.sin_addr) == 0)
        {
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
        }
 sendto(sockfd_udp,arr,strlen(arr),0,(struct sockaddr*)&serv_addr_udp,sizeof(serv_addr_udp));
        bzero(buffer_udp,256);
        close(sockfd_udp);



}


void send_data_to_server_udp_debugging(char arr[256])
{
 sockfd_udp = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (socket <0)
        error("Error opening socket");
//      printf("UDP socket created %d\n",sockfd_udp);
        if(server == NULL)
        {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
//      bzero((char*) & serv_addr,sizeof(serv_addr_udp));
        memset ((char*)&serv_addr_udp,0,sizeof(serv_addr_udp));
        serv_addr_udp.sin_family = AF_INET;
//      bcopy((char *)server_udp->h_addr,
  //      (char *)&serv_addr_udp.sin_addr.s_addr,
   //     server_udp->h_length);
        serv_addr_udp.sin_port = htons(portno_udp);

        if(inet_aton(DEBUGGING_IP,&serv_addr_udp.sin_addr) == 0)
        {
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
        }
 sendto(sockfd_udp,arr,strlen(arr),0,(struct sockaddr*)&serv_addr_udp,sizeof(serv_addr_udp));
        bzero(buffer_udp,256);
        close(sockfd_udp);



}




int send_data_to_main_controller_ipc(char ipc_buffer[256])
	{
	ipc_data.ipc_msg_type=1;
	strcpy(ipc_data.ipc_msg,ipc_buffer);
	if(msgsnd(msgid,(void*)&ipc_data,MSG_SIZE,0) == -1)
		{
			fprintf(stderr,"msgsnd failed\n");
			exit(EXIT_FAILURE);
		}




	}

/*int get_zoneindex(int zone)
{
	int i,j,k;
	zone_index=0;
	for(i=1;i<=total_levels;i++)
		{
		if(i==controller_no)
		{
			for(j=1;j<=*(zones_per_level+(i-1));j++)
			{
				if(j==zone)
				{
					if(i==1)
					zone_index+=(zone-1);
					else
					{
						for(k=0;k<i-1;k++)
						{
						zone_index+=*(zones_per_level+k);
						}				
						zone_index+=(zone-1);
						break;
					}
				}
													
			}
		}
		}
	return zone_index;
}*/





/* int read_zoneoccupancy_from_datafile(int zone_index)
{
	int i; 	
	datafile_ptr = fopen("/home/pgs/data/new_data.bin","rb");
	printf("zone occupancy details ");
	for(i=0;i<total_zones;i++)
	{
	fread((zone_occupancy+i),sizeof(int),1,datafile_ptr);
	printf("%d",*(zone_occupancy+i));
	}
	printf(" Occupancy for zone index %d is %d\n",zone_index,*(zone_occupancy+(zone_index-1)));
	
	return *(zone_occupancy+(zone_index-1));
	fclose(datafile_ptr);

}*/


int read_zoneoccupancy_from_datafile(int zone_index)
{
	int i,occupancy;
	datafile_ptr = fopen("/home/pgs/data/new_data.bin","rb");
	fseek(datafile_ptr,(zone_index*4),SEEK_SET);
	fread(&occupancy,sizeof(int),1,datafile_ptr);
	fclose(datafile_ptr);
	return (occupancy);

}

/* void write_zoneoccupancy_to_datafile(int *updated_occupancy)
{	
	int i,j;
	datafile_ptr = fopen("/home/pgs/data/new_data.bin","r+");
	
	for(i=0;i<total_zones;i++)
		fwrite((updated_occupancy+i),1,sizeof(int),datafile_ptr);
	fclose(datafile_ptr);	
} */

void write_zoneoccupancy_to_datafile(int occupancy,int zone_index)
{
	int i;
	datafile_ptr = fopen("/home/pgs/data/new_data.bin","r+");
	//printf("zone index=%d\n",zone_index);
	fseek(datafile_ptr,(zone_index*4),SEEK_SET);
	fwrite(&occupancy,1,sizeof(int),datafile_ptr);
	fclose(datafile_ptr);
}



void write_zone_spaces_to_zone_space_datafile(int total_zone_space,int zone_index)
{
        int i;
        datafile_ptr = fopen("/home/pgs/data/zone_space_data.bin","r+");
        //printf("zone index=%d\n",zone_index);
        fseek(datafile_ptr,(zone_index*4),SEEK_SET);
        fwrite(&total_zone_space,1,sizeof(int),datafile_ptr);
        fclose(datafile_ptr);
}


int update_zone(int zone_number,char* operation)
{
//	zone_index=get_zoneindex(zone_number+1);
//	printf("zone index of zone %d is %d\n",zone_number+1,zone_index);
//	printf("\nZone_data_ptr occupied value =%d\n",(zone_data_ptr+zone_number)->total_occupied);
	(zone_data_ptr+zone_number)->total_occupied=read_zoneoccupancy_from_datafile(zone_number);
//	printf("\nzone structure occupied value %d\n",(zone_data_ptr+zone_number)->total_occupied);
	if ((*operation) =='+')
	{
	
	(zone_data_ptr+zone_number)->total_occupied++;
//	*(zone_occupancy+(zone_index-1))=(zone_data_ptr+zone_number)->total_occupied;	
	write_zoneoccupancy_to_datafile((zone_data_ptr+zone_number)->total_occupied,zone_number);
	//total_occupied_in_level++;
	}

	else
	{
//	*(zone_occupancy+(zone_index-1))=(zone_data_ptr+zone_number)->total_occupied;
	(zone_data_ptr+zone_number)->total_occupied--;
//	write_zoneoccupancy_to_datafile(zone_occupancy);
	write_zoneoccupancy_to_datafile((zone_data_ptr+zone_number)->total_occupied,zone_number);
	//total_occupied_in_level--;
	}

	(zone_data_ptr+zone_number)->total_free = ((zone_data_ptr+zone_number)->total_space)-((zone_data_ptr+zone_number)->total_occupied);
	
	//printf("Free space in zone %d is %d\n",zone_number+1,(zone_data_ptr+zone_number)->total_free);	

	if((zone_data_ptr+zone_number)->total_free <= 0)
	return -1;

	else if ((zone_data_ptr+zone_number)->total_free >= (zone_data_ptr+zone_number)->total_space)
	return 0;

	else
	return ((zone_data_ptr+zone_number)->total_free); 
	
}

int calculate_total_occupied_in_level(void)
{
	int i,zone_occupancy,level_occupancy=0;
	datafile_ptr = fopen("/home/pgs/data/new_data.bin","rb");
	for(i=0;i<total_zones;i++)
		{
		fread(&zone_occupancy,sizeof(int),1,datafile_ptr);
		level_occupancy+=zone_occupancy;
		}
	fclose(datafile_ptr);
	return level_occupancy;
}

int update_level(void)
{
	total_occupied_in_level=calculate_total_occupied_in_level();
	total_free_in_level= total_spaces_in_level-total_occupied_in_level;
	
	if (total_free_in_level <=0)	
	return -1;
	
	else if (total_free_in_level >= total_spaces_in_level)
	return 0;
	
	else 
	return (total_free_in_level);

}

void send_data(char arr[256])

{
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        error("ERROR opening socket");

   	if (server == NULL)
	{
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    	}
        bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	
	    	
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    	n = write(sockfd,arr,strlen(arr));
  
  	if (n < 0)
        error("ERROR writing to socket");

    	bzero(buffer,256);
    	n = read(sockfd,buffer,255);

    	if (n < 0)
        error("ERROR reading from socket");

//    	printf("%s\n",buffer);
    	close(sockfd);


}

void send_zone(int zone_number,int free_space)
{

	if(free_space == -1)
	{
		snprintf(data,256,"23-%d-%d",controller_no,(zone_number+1));
		//printf("Zone%d is Full\n",(zone_number+1));
	//	send_data(data);
		if(is_main_controller)
		send_data_to_main_controller_ipc(data);
		else
		send_data_to_main_controller_udp(data);
	}
       	else if(free_space == 0)
	{
		snprintf(data,256,"24-%d-%d",controller_no,(zone_number+1));
		//printf("Zone%d is Empty\n",(zone_number+1));
	//	send_data(data);
	//	send_data_to_server_udp(data);
	//	send_data_to_server_udp_debugging(data);
		
		if(is_main_controller)
		send_data_to_main_controller_ipc(data);
		else
		send_data_to_main_controller_udp(data);
	}
	else
	{
		snprintf(data,256,"22-%d-%d-%d",controller_no,(zone_number+1),free_space);
	//	printf("%d free space in zone %d\n", free_space,(zone_number+1));
	//	send_data(data);
		if(is_main_controller)
		send_data_to_main_controller_ipc(data);
		else
		send_data_to_main_controller_udp(data);
	}
}

void send_level(int free_space)
{
	if(free_space==-1)
	{

		snprintf(data,256,"26-%d",controller_no);
		//printf("Level %d Full\n",controller_no);
	//	send_data(data);
		if(is_main_controller)
		send_data_to_main_controller_ipc(data);
		else
		send_data_to_main_controller_udp(data);
	}
	else if(free_space == 0)
	{

		snprintf(data,256,"27-%d",controller_no);
		//printf("Level %d is Empty\n",controller_no);
	//	send_data(data);
		if(is_main_controller)
		send_data_to_main_controller_ipc(data);
		else
		send_data_to_main_controller_udp(data);
	}
	else
	{
		snprintf(data,256,"25-%d-%d",controller_no,free_space);
		//printf("%d free space in Level %d\n", free_space,controller_no);
	//	send_data(data);
		if(is_main_controller)
		send_data_to_main_controller_ipc(data);
		else
		send_data_to_main_controller_udp(data);
	}

}

void catch_alarm(int sig)
{
	send_data_to_server_udp(alive_msg);
	send_data_to_server_udp_debugging(alive_msg);
}

void receiveData(int n, siginfo_t *info, void *unused) {
        char event_buf[256];
	int detector,i,j,zone_free,level_free;
	int signal_data = info->si_int;
	*detector_ptr[(signal_data-1)]+=1;
	
	//printf("received signal %d\n",signal_data);
	if(signal_data%2!=0)
	{	
		detector=(signal_data+1)/2;
		
             snprintf(event_buf,256,"28-%d-%d",controller_no,detector);

		send_data_to_server_udp(event_buf);
                send_data_to_server_udp_debugging(event_buf);					
//		printf("detector%d forward \n",detector);		
		for(i=0;i<total_zones;i++)
		{
			for(j=0;j<(zone_data_ptr+i)->total_entry;j++)
			{	
				if(detector == entry_detectors[i][j])
					{
//					printf("detector%d is entry%d for zone %d\n",detector,(j+1),(i+1)); 
					zone_free = update_zone(i,"+");
					//printf ("zone_free value %d\n",zone_free);
					send_zone(i,zone_free);
					}

			}
		}
		
		for(i=0;i<total_zones;i++)
		{
			for(j=0;j<(zone_data_ptr+i)->total_exit;j++)
			{
				if(detector == exit_detectors[i][j])
					{
					zone_free = update_zone(i,"-");
					send_zone(i,zone_free);
				
					}
				
			}
		
		}

	}
	else
	{
		detector=signal_data/2;


             snprintf(event_buf,256,"29-%d-%d",controller_no,detector);

		send_data_to_server_udp(event_buf);
		send_data_to_server_udp_debugging(event_buf);
		
		//printf("signal_data=%d\n",signal_data);
		//printf("detector%d reverse \n",detector);
	      for(i=0;i<total_zones;i++)
                {
                        for(j=0;j<(zone_data_ptr+i)->total_entry;j++)
                        {
                                if(detector == entry_detectors[i][j])
                                        {
                                        zone_free = update_zone(i,"-");
                                       send_zone(i,zone_free);
                                        }
			
                          //    	level_free =  update_level();
			//	send_level(level_free);
                        }

                }
		
		for(i=0;i<total_zones;i++)
                {
                        for(j=0;j<(zone_data_ptr+i)->total_exit;j++)
                        {
                                if(detector == exit_detectors[i][j])
                                        {
                                        zone_free = update_zone(i,"+");
					send_zone(i,zone_free);
                                        }
                        
			}
                }

	}					
level_free = update_level();
send_level(level_free);
}




int main ( int argc, char *argv[] )
{
	
	int j,l,k=0;
	
	is_main_controller = atoi(argv[1]);
	printf("is main controller %d\n",is_main_controller);
	if(!is_main_controller)
	main_controller_ip = argv[2];
	
	server_ip=argv[3];
	server=gethostbyname(argv[3]);
	server_udp=gethostbyname(argv[3]);	
        printf("server_udp %s\n",server_udp->h_addr);
	portno = atoi(argv[4]);
	portno_udp = atoi(argv[4]);
	int arg_count=argc;
	int configfd;
	char buf[10];
	int i;
	
	controller_no=atoi(argv[5]);
        printf("controller_no %d \n",controller_no);




	total_levels=atoi(argv[6]);
	zones_per_level=calloc(total_levels,sizeof(int*));
	for(l=1;l<=total_levels;l++)
		{
		*(zones_per_level+(l-1))=atoi(argv[6+l]);
		total_zones_in_parking+=(*(zones_per_level+(l-1)));
		}
	zone_occupancy=calloc(total_zones_in_parking,sizeof(int*));




	total_spaces_in_level=atoi(argv[7+total_levels]);
        printf("total spaces %d \n",total_spaces_in_level);
	total_zones=atoi(argv[8+total_levels]);
	printf("total zones = %d\n",total_zones);
	total_detectors=atoi(argv[9+total_levels]);
        msgid = msgget((key_t)1234,0666|IPC_CREAT);
        if(msgid == -1)
        {
        fprintf(stderr,"failed to create msg queue\n");
	}
	printf("total detectors =%d\n",total_detectors);
	msgid = msgget((key_t)1234,0666|IPC_CREAT); 	
	if(msgid == -1)
	{
		fprintf(stderr,"failed to create msg queue\n");
		exit(EXIT_FAILURE);
	}
	printf("1");
	for(i=0;i<total_zones;i++)
	{
		zone_data_ptr->total_space = atoi(argv[10+total_levels+k]);k++;

		write_zone_spaces_to_zone_space_datafile(zone_data_ptr->total_space,i);

		
		zone_data_ptr->total_entry = atoi(argv[10+total_levels+k]);k++;
		zone_data_ptr->total_exit  = atoi(argv[10+total_levels+k]);k++;

		if(zone_data_ptr->total_entry==0)
		{
			for(j=0;j<=zone_data_ptr->total_entry;j++)
			{
			entry_detectors[i][j]=atoi(argv[10+total_levels+k]);k++;
			}
		}
		if(zone_data_ptr->total_entry>0)
		{	
			for(j=0;j<zone_data_ptr->total_entry;j++)
			{
			entry_detectors[i][j]=atoi(argv[10+total_levels+k]);k++;
			}
	       	}
		
		if(zone_data_ptr->total_exit==0)
		{
			for(j=0;j<=zone_data_ptr->total_exit;j++)
			{
			exit_detectors[i][j]=atoi(argv[10+total_levels+k]);k++;
			}
		}

		if(zone_data_ptr->total_exit>0)
		{

			for(j=0;j<zone_data_ptr->total_exit;j++)
			{
			exit_detectors[i][j]=atoi(argv[10+total_levels+k]);k++;
			}
		}
		zone_data_ptr++;
	printf("i=%d\n",i);
}
	printf("Test1");	
	zone_data_ptr-=total_zones;

	printf("Test\n");		
	for(i=0;i<(total_detectors*2);i++)
	detector_ptr[i]=&a[i];

//printf("controller No = %d\n Total Spaces In Level = %d\n Total zones = %d\n Total detectors =%d \n", controller_no,total_spaces_in_level,total_zones,total_detectors);

	
	/* setup the signal handler for SIG_DT
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */

	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	//sig.sa_flags = SA_ONSTACK;
	sigaction(SIG_DT, &sig, NULL);

	

	/* kernel needs to know our pid to be able to send us a signal ->
 	 * we use debugfs for this -> do not forget to mount the debugfs!
 	 */
	configfd = open("/sys/kernel/debug/signalconfpid", O_WRONLY);
	if(configfd < 0) {
		perror("open");
		return -1;
	}
	sprintf(buf, "%i", getpid());
	if (write(configfd, buf, strlen(buf) + 1) < 0) {
		perror("fwrite"); 
		return -1;
	}
snprintf(alive_msg,256,"21-%d",controller_no);
signal (SIGALRM, catch_alarm);
  new.it_interval.tv_sec = 5; 
  new.it_interval.tv_usec = 0; 
  new.it_value.tv_sec = 5;
  new.it_value.tv_usec = 0;
   
  old.it_interval.tv_sec = 0;
  old.it_interval.tv_usec = 0;
  old.it_value.tv_sec = 0;
  old.it_value.tv_usec = 0;
   
  if (setitimer (ITIMER_REAL, &new, &old) < 0)
      printf("timer init failed\n");
  //else
    //  printf("timer init succeeded\n");
while(1)
{

sleep(1000);
}

	return 0;

}

