/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../util.h"

const char *
gid(void)
{
	return bprintf("%d", getgid());
}

const char *
username(void)
{
	struct passwd *pw = getpwuid(geteuid());

	if (pw == NULL) {
		fprintf(stderr, "getpwuid '%d': %s\n", geteuid(), strerror(errno));
		return NULL;
	}

	return bprintf("%s", pw->pw_name);
}

const char *
uid(void)
{
	return bprintf("%d", geteuid());
}
