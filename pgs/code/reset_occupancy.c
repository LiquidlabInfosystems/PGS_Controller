#include<stdio.h>
int total_zones;

FILE *datafile_ptr;


int reset_occupancy(void)
{
	int i,reset_value=0;
	datafile_ptr=fopen("/home/pgs/data/new_data.bin","r+");
	for(i=1;i<=total_zones;i++)
		{	
		fwrite(&reset_value,1,sizeof(int),datafile_ptr);
		}
	fclose(datafile_ptr);
}

int main(int argc, char* argv[] )
{
	total_zones=atoi(argv[1]);
	reset_occupancy();	
	return 0;
}
