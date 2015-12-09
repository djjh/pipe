#ifndef __PIPE_REPR_H
#define __PIPE_REPR_H

#include "pipe.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

struct _pipe {
	FILE *f;
	int fd;
	pipe_rw rw;
	pipe_blk blk;
};

#elif defined(_WIN32) || defined(_WIN64)

#include <Windows.h> // anonymous pipes are in Winbase.h

struct _pipe {
	PROCESS_INFORMATION p;
	HANDLE h;
	HANDLE ah;
   pipe_rw rw;
	pipe_blk blk;
};
	
#endif

#ifdef __cplusplus
}
#endif

#endif  // __PIPE_REPR_H
