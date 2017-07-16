
#include<stdio.h>
#include<stdlib.h>

//int total_levels;
int total_zones;
//int *zones_per_level;
//int ***occupancy_per_zones_per_level;
FILE* datafile_ptr;
FILE* zone_space_datafile_ptr;
int *occupancy_per_zones;
int* total_space_per_zones;

initialize_data(void)
{
	int i,j;
	int initial_value=0;
	datafile_ptr=fopen("/home/pgs/data/new_data.bin","wb");
	zone_space_datafile_ptr=fopen("/home/pgs/data/zone_space_data.bin","wb");
	/*for(i=0;i<total_levels;i++)
	{
		for(j=0;j<(*(zones_per_level+i));j++)

			{	
	 *((*(occupancy_per_zones_per_level+(i)))+(j))=&initial_value;
	fwrite(*((*(occupancy_per_zones_per_level+(i)))+(j)),1,sizeof(int),datafile_ptr);
			}
	}*/



	for(i=0;i<total_zones;i++)
	{
		*(occupancy_per_zones+i)=initial_value;
		*(total_space_per_zones+i)=initial_value;
		fwrite((occupancy_per_zones+i),1,sizeof(int),datafile_ptr);
		fwrite((total_space_per_zones+i),1,sizeof(int),zone_space_datafile_ptr);
	}
		


		fclose(datafile_ptr);
		fclose(zone_space_datafile_ptr);
}

int main(int argc, char* argv[] )
{


int i;
//total_levels=atoi(argv[1]);
  total_zones=atoi(argv[1]);
//occupancy_per_zones_per_level=calloc(total_levels,sizeof(int*));
//zones_per_level=calloc(total_levels,sizeof(int*));
occupancy_per_zones=calloc(total_zones,sizeof(int*));
total_space_per_zones=calloc(total_zones,sizeof(int*));
//total_zones=0;
/*for (i=0;i<total_levels;i++)
	{	
	*(zones_per_level+i)=atoi(argv[2+i]);
	total_zones+=*(zones_per_level+i);
	occupancy_per_zones_per_level[i]=calloc((*(zones_per_level+i)),sizeof(int));	
	}
*/
initialize_data(); 
//free(occupancy_per_zones_per_level);
//free(zones_per_level);
free(occupancy_per_zones);
free(total_space_per_zones);
return 0;
}

