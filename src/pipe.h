#ifndef __PIPE_H
#define __PIPE_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _pipe pipe_t;

typedef enum _pipe_rw {
	pm_write,
	pm_read
} pipe_rw;

typedef enum _pipe_blk {
	pm_blocking,
	pm_nonblocking
} pipe_blk;

#include "pipe_repr.h"


/*
 * Opens a child process with the given command from the command line,
 * returning a pipe_t, with read/write permissions as specified by rw.
 *
 * Pipes are currently unidirectional.
 * 
 * if command == NULL:
 *   if mode == w -> returns pipe to stdout
 *   if mode == r -> returns pipe opens pipe to stdin
 * if command != NULL:
 *   if mode == w -> runs command, returns output pipe to file
 *   if mode == r -> runs command, returns input pipe from file
 *
 * Returns < 0 on failure.
 */
int pipe_open(pipe_t * pipe, char const * command, pipe_rw rw);

//
int pipe_write(pipe_t * pipe, void const * data, unsigned size, pipe_blk b);

//
int pipe_read(pipe_t * pipe, void * data, unsigned size, pipe_blk b);

//
int pipe_close(pipe_t * pipe);

#ifdef __cplusplus
}
#endif
#endif  // __PIPE_H
