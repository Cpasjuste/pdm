#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "utils.h"

int write_pid()
{
	int fd = open (PIDFILE, O_WRONLY | O_CREAT, 0640);

	if (fd < 0)
	{
		perror ("open");
		return 0;
	}
	else
	{
		char buf[BUF_LEN];
		snprintf (buf, sizeof(buf), "%d\n", getpid());
		int ret = write (fd, buf, strlen(buf));

		if (ret < 0)
		{
			perror ("write");
			return 0;
		}

		if (close (fd))
		{
			perror ("close");
			return 0;
		}

	}
	return 1;
}

char *pdm_dir_and( char *path )
{
	static char new_path[512];
	
	strcpy( new_path, pdm_dir );
	strcat( new_path, path );

	return new_path;
}

char *str_sub (const char *s, unsigned int start, unsigned int end)
{
	char *new_s = NULL;

	if (s != NULL && start < end)
	{
		new_s = malloc (sizeof (*new_s) * (end - start + 2));
		if (new_s != NULL)
		{
			int i;
			for (i = start; i <= end; i++)
			{
				new_s[i-start] = s[i];
			}
			new_s[i-start] = '\0';
		}
		else
		{
			printf ("Fail -> str_sub\n");
		}
	}
   	return new_s;
}



