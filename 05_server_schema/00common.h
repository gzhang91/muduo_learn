#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

void PRINT(const char *file, int line, const char *fmt, ...);

int CreateSocketOrDie();

void BindOrDie(int fd, const char *ip_text, unsigned short port);

void ListenOrDie(int fd);

int AcceptOrDie(int fd, char ip_text[], int len);

int Read(int fd, char **buff);

int Write(int fd, char *buff, int len);

#endif // _COMMON_H_