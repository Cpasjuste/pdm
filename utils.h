#ifndef __UTILS_H__
#define __UTILS_H__

#define PIDFILE "/var/run/pdm.pid"
#define BUF_LEN 32

char pdm_dir[512];

int write_pid();
char *pdm_dir_and( char *path );

#endif
