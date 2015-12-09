#include <cstdio>
#include <cassert>
#include "pipe.h"

int main(int argc, char ** argv)
{
	if (argc != 2)
		return 0;

	pipe_t p;

	// test child to parent pipe
	printf("\n[parent] test child to parent\n");
	pipe_open(&p, argv[1], pm_read);
	int msg = ~69;
	if (pipe_read(&p, &msg, sizeof(msg), pm_blocking) < 0)
		printf("[parent] error reading\n");
	printf("[parent] child piped to parent: %s\n", (msg==69)?"yes":"no");
	pipe_close(&p);

	// test parent to child pipe
	printf("\n[parent] opening write pipe\n");
	if (pipe_open(&p, "wpc.exe", pm_write) < 0)
	{
		printf("[parent] error opening\n");
		return 0;
	}

	printf("[parent] piping to child\n");

	msg = 69;
	if (pipe_write(&p, &msg, sizeof(msg), pm_blocking) < 0)
		printf("[parent] error writing\n");

	msg = 96;
	if (pipe_write(&p, &msg, sizeof(msg), pm_blocking) < 0)
		printf("[parent] error writing\n");

	pipe_close(&p);


	return 0;
}


