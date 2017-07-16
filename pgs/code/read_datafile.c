#include <stdio.h>
#include <stdlib.h>


//int total_levels;
//int* zones_per_level;
int total_zones;
FILE* datafile_ptr;
//int *zone_occupancy;
int* occupancy_per_zones;

int main(int argc, char* argv[] )
{
int i;
//total_levels=atoi(argv[1]);
//zones_per_level=calloc(total_levels,sizeof(int*));
total_zones=atoi(argv[1]);
occupancy_per_zones=calloc(total_zones,sizeof(int*));
//total_zones=0;

/*for (i=0;i<total_levels;i++)
	{	
	*(zones_per_level+i)=atoi(argv[2+i]);
	total_zones+=*(zones_per_level+i);
	}*/


//zone_occupancy=calloc(total_zones,sizeof(int*));
//printf("Total Levels %d\n",total_levels);
printf("Total zones %d\n",total_zones);
 datafile_ptr = fopen("/home/pgs/data/new_data.bin","rb");

for(i=0;i<total_zones;i++)
	{
	fread((occupancy_per_zones+i),sizeof(int),1,datafile_ptr);
	}


//fread(zone_occupancy,total_zones*4,1,datafile_ptr);
for(i=0;i<total_zones;i++)
//printf("%d\n",*(zone_occupancy+i));
printf("%d\n",*(occupancy_per_zones+i));

   close(datafile_ptr);
//	free(zones_per_level);
//	free(zone_occupancy);
 free(occupancy_per_zones);  
 return(0);
}

