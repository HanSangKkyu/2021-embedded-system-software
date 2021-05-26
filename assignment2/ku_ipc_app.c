#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "ku_ipc_lib.c"
#include "ku_ipc_act_lib.c"

void main(void){
	char stst;
	int read_data;

	while(1){
		ku_read(&stst);
		read_data = (int)stst-(int)('0');
		printf("now temperature is %d\n", read_data);

		ku_write(read_data);
		sleep(1);
	}

	// ku_write(27);
	
	
	return;
}
