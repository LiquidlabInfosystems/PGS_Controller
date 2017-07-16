#include<stdio.h>

FILE* datafile_ptr;
int zone_number,occupancy;

void update_occupancy(void)
{

datafile_ptr=fopen("/home/pgs/data/new_data.bin","r+");
fseek(datafile_ptr,((zone_number-1)*4),SEEK_SET);
fwrite(&occupancy,1,sizeof(int),datafile_ptr);
fclose(datafile_ptr);
}

int main(int argc,char*argv[])
{

zone_number=atoi(argv[1]);
occupancy=atoi(argv[2]);
update_occupancy();
return 0;
}

