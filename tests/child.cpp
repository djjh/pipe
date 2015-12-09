#include <stdio.h>
#include "pipe.h"

int main(int argc, char ** argv)
{

	pipe_t p;

	pipe_open(&p, NULL, pm_write);
	int msg = 69;
	if (pipe_write(&p, &msg, sizeof(msg), pm_blocking)< 0)
		return 0;
	//pipe_close(&p);

	printf("\n[child] opening read pipe\n");
	pipe_open(&p, NULL, pm_read);

	printf("[child] piping from parent\n");
	msg = ~69;
	if (pipe_read(&p, &msg, sizeof(msg), pm_blocking) < 0)
		return 0;

	printf("[child] parent piped to child: %s\n", (msg==69)?"yes":"no");

	msg = ~96;
	if (pipe_read(&p, &msg, sizeof(msg), pm_nonblocking) < 0)
		return 0;

	printf("[child] parent piped to child: %s\n", (msg==96)?"yes":"no");

	//pipe_close(&p);
	return 0;
}

