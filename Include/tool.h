#ifndef __TOOL_H__
#define __TOOL_H__

#include<errno.h>

#define INVALID_FD(fd) (fd < 0)
#define INVALID_POINTER(p) (p == NULL)

inline void seterrno(int eno)
{
    errno = eno;
}

#endif