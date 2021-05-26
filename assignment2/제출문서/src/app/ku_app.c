#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "ku_sense.c"
#include "ku_act.c"

void main(void){
	int read_data;
	
	while(1){
		ku_read(&read_data);
		printf("now temperature is %d\n", read_data);

		ku_write(read_data);
		sleep(1);
	}


	return;
}
