#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "ku_ipc_lib.c"

void main(void){
	char *stst;
	ku_read(stst);
	return;
}
