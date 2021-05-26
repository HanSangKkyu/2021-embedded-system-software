#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "ku_ipc_lib.c"

void main(void){
	char stst;
	ku_read(&stst);
	printf("%c\n", stst);
	printf("%d\n", (int)stst-(int)('0'));
	
	return;
}
