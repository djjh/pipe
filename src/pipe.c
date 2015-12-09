#include "pipe.h"

#if defined(__linux__) || defined(__APPLE__)

int pipe_open(pipe_t * p, char const * f, pipe_rw rw)
{
	// validate input
	if (!p)
		return -1;

	p->rw = rw;
	p->blk = pm_blocking; // blocking by default

	if (f) {
		// open file
		if (!(p->f = popen(f, (rw == pm_read) ? "r" : "w")))
				return -1;

		// store representation
		if ((p->fd = fileno(p->f)) == -1)
				return -1;
		return 0;
	}

	p->f = 0;
	p->fd = (rw == pm_read) ? 0 : 1;

	return 0;
}

int pipe_write(pipe_t * p, void const * d, unsigned s, pipe_blk b)
{
	int r;

	// validate input
	if (!p || !d || !s)
		return -1;

	// trying to read from the write end of the pipe
	if (p->rw == pm_read)
		return -1;

	if (p->blk != b) {
		int flags = fcntl(p->fd, F_GETFL, 0);
		flags = (b == pm_blocking) ? flags^O_NONBLOCK : flags|O_NONBLOCK;
		fcntl(p->fd, F_SETFL, flags);
		p->blk = b;
	}

	// write
	if((r = write(p->fd, d, s)) < 0)
		return -1;

	return r;
}

int pipe_read(pipe_t * p, void * d, unsigned s, pipe_blk b)
{
	int r;

	// validate input
	if (!p || !d || !s)
		return -1;

	// trying to write to the read end of the pipe
	if (p->rw == pm_write)
		return -1;

	if (p->blk != b) {
		int flags = fcntl(p->fd, F_GETFL, 0);
		flags = (b == pm_blocking) ? flags^O_NONBLOCK : flags|O_NONBLOCK;
		fcntl(p->fd, F_SETFL, flags);
		p->blk = b;
	}

	// read
	if((r = read(p->fd, d, s)) < 0)
		return -1;

	return r;
}

int pipe_close(pipe_t * p)
{
	if (!p)
		return -1;

	if (!p->f && (p->fd != 0 || p->fd != 1))
		return -1;

	if (p->f)
		pclose(p->f);

	return 0;
}

#elif defined(_WIN32) || defined(_WIN64)
#include <stdio.h>
#include <strsafe.h>

int pipe_open_read(pipe_t * p, char const *f)
{
	// 
	SECURITY_ATTRIBUTES sa;
	STARTUPINFO si;
	HANDLE ow;

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// PARENT OUTPIPE
	if (!CreatePipe(&p->h, &ow, &sa, 0))
		return -1;

	if (!SetHandleInformation(p->h, HANDLE_FLAG_INHERIT, 0))
		return -1;

	// CREATE CHILD
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.hStdOutput = ow;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.dwFlags |= STARTF_USESTDHANDLES;
	ZeroMemory(&p->p, sizeof(p->p));

	if (!CreateProcess(
			NULL,
			TEXT(f),
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si,
			&p->p))
		return -1;

	// CLOSE YOUR ACCESS TO CHILDS PIPEs
	if (!CloseHandle(ow))
		return -1;

	return 0;
}

int pipe_open_write(pipe_t * p, char const *f)
{
	// 
	SECURITY_ATTRIBUTES sa;
	STARTUPINFO si;
	HANDLE ir;


	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// PARENT OUTPIPE
	if (!CreatePipe(&ir, &p->h, &sa, 0))
		return -1;

	if (!SetHandleInformation(p->h, HANDLE_FLAG_INHERIT, 0))
		return -1;

	// CREATE CHILD
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.hStdInput = ir;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.dwFlags = STARTF_USESTDHANDLES;
	ZeroMemory(&p->p, sizeof(p->p));

	if (!CreateProcess(
			NULL,
			TEXT(f),
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si,
			&p->p))
		return -1;

	// CLOSE YOUR ACCESS TO CHILDS PIPEs
	if (!CloseHandle(ir))
		return -1;

	return 0;
}

int pipe_open(pipe_t * p, char const * f, pipe_rw rw)
{
	// validate input
	if (!p)
		return -1;

	// todo: check if open and what not

	// initialize pipe defaults
	p->h = INVALID_HANDLE_VALUE;
	ZeroMemory(&p->p, sizeof(p->p));
	p->rw = rw;
	p->blk = pm_blocking;

	if (f) {
		if (rw == pm_read && (pipe_open_read(p, f) < 0))
			return -1;
		if (rw == pm_write && (pipe_open_write(p, f) < 0))
			return -1;
		return 0;
	}

	if (rw == pm_read)
	{
		p->h = GetStdHandle(STD_INPUT_HANDLE);
		if (p->h == INVALID_HANDLE_VALUE)
			return -1;
	}
	else if (rw == pm_write)
	{
		p->h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (p->h == INVALID_HANDLE_VALUE)
			return -1;
	}

	return 0;
}

int pipe_write(pipe_t * p, void const * d, unsigned s, pipe_blk b)
{
	DWORD r = 0;

	// validate input
	if (!p || !d || !s)
		return -1;

	// trying to read from the write end of the pipe
	if (p->rw == pm_read)
		return -1;

	p->blk = b;

	if (p->blk == pm_blocking)
	{
		// write
		if (!WriteFile(p->h, d, s, &r, NULL))
			return -1;
	}
	else
	{
		// write
		if (!WriteFile(p->h, d, s, &r, NULL))
			return -1;
	}

	return r;
}

int pipe_read(pipe_t * p, void * d, unsigned s, pipe_blk b)
{
	DWORD r = 0; // read
	DWORD a = 0; // avail
	DWORD l = 0; // left

	// validate input
	if (!p || !d || !s)
		return -1;

	// trying to write to the read end of the pipe
	if (p->rw == pm_write)
		return -1;

	p->blk = b;

	if (p->blk == pm_blocking)
	{
		// read
		if (!ReadFile(p->h, d, s, &r, NULL))
			return -1;
	}
	else
	{

		if (!PeekNamedPipe(p->h, NULL, 0, NULL, &a, NULL))
			return -1;

		// read
		if (a >= s && !ReadFile(p->h, d, s, &r, NULL))
			return -1;
	}

	return r;
}

int pipe_close(pipe_t * p)
{
	if (!p)
		return -1;
	if (p->p.hProcess)
	{
	   WaitForSingleObject(p->p.hProcess, INFINITE);
	   CloseHandle(p->p.hProcess);
	   CloseHandle(p->p.hThread);

		ZeroMemory(&p->p, sizeof(p->p));
	}
	p->h = 0;
	return 0;
}

#endif
