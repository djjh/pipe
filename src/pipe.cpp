#include "pipe.h"

#if defined(__linux__) || defined(__APPLE__)

#include <stdio.h>
#include <unistd.h>

struct _pipe {
	int fd;
};

int ppopen(ppipe *p, char const *filename, ppipe_mod m)
{
	FILE * f;

	// validate input
	if (!p)
		return -1;

	// open file
  	if (!(f = popen(filename, (m == ppipe_read) ? "r" : "w")))
			return -1;

	// store representation
	if ((p->fd = fileno(f)) == -1)
	      return -1;

   return 0;
}

int ppwrite(ppipe const *p, void *d, unsigned s);
{
	int r;

	// validate input
	if (!p || !d || !s)
		return -1;

	// write 
	if((r = write(p->fd, d, s)) < 0)
		return -1;

	return r;
}

int ppread(ppipe const *p, void *d, unsigned s);
{
}

void ppclose(ppipe const *p);
{
	pclose(p->fd);
}

#elif defined(_WIN32) || defined(_WIN64)
#endif
