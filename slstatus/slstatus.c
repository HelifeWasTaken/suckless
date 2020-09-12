/* See LICENSE file for copyright and license details. */

#include <alsa/asoundlib.h>
#include <err.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <limits.h>
#include <linux/wireless.h>
#include <locale.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>

#undef strlcat
#undef strlcpy

#include "extern/arg.h"
#include "extern/strlcat.h"
#include "extern/strlcpy.h"
#include "extern/concat.h"

struct arg {
	char *(*func)();
	const char *fmt;
	const char *args;
};

static char *smprintf(const char *fmt, ...);
static char *battery_perc(const char *bat);
static char *battery_state(const char *bat);
static char *cpu_perc(void);
static char *datetime(const char *fmt);
static char *disk_free(const char *mnt);
static char *disk_perc(const char *mnt);
static char *disk_total(const char *mnt);
static char *disk_used(const char *mnt);
static char *entropy(void);
static char *gid(void);
static char *hostname(void);
static char *ip(const char *iface);
static char *load_avg(void);
static char *ram_free(void);
static char *ram_perc(void);
static char *ram_used(void);
static char *ram_total(void);
static char *run_command(const char *cmd);
static char *swap_free(void);
static char *swap_perc(void);
static char *swap_used(void);
static char *swap_total(void);
static char *temp(const char *file);
static char *uid(void);
static char *uptime(void);
static char *username(void);
static char *vol_perc(const char *card);
static char *wifi_perc(const char *iface);
static char *wifi_essid(const char *iface);
static char *kernel_release(void);
static void set_status(const char *str);
static void sighandler(const int signo);
static void usage(void);

char *argv0;
char concat[];
static unsigned short int delay;
static unsigned short int done;
static unsigned short int dflag, oflag;
static Display *dpy;

#include "config.h"

static char *
smprintf(const char *fmt, ...)
{
	va_list ap;
	char *ret;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	ret = malloc(++len);
	if (ret == NULL) {
		err(1, "malloc");
	}

	va_start(ap, fmt);
	vsnprintf(ret, len, fmt, ap);
	va_end(ap);

	return ret;
}

static char *
battery_perc(const char *bat)
{
	int perc;
	FILE *fp;

	ccat(3, "/sys/class/power_supply/", bat, "/capacity");
	fp = fopen(concat, "r");
	if (fp == NULL) {
		warn("Failed to open file %s", concat);
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "%i", &perc);
	fclose(fp);

	return smprintf("%d%%", perc);
}

static char *
battery_state(const char *bat)
{
	char state[12];
	FILE *fp;

	ccat(3, "/sys/class/power_supply/", bat, "/status");
	fp = fopen(concat, "r");
	if (fp == NULL) {
		warn("Failed to open file %s", concat);
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "%12s", state);
	fclose(fp);

	if (strcmp(state, "Charging") == 0) {
		return smprintf("+");
	} else if (strcmp(state, "Discharging") == 0) {
		return smprintf("-");
	} else if (strcmp(state, "Full") == 0) {
		return smprintf("=");
	} else {
		return smprintf("?");
	}
}

static char *
cpu_perc(void)
{
	int perc;
	long double a[4], b[4];
	FILE *fp;

	fp = fopen("/proc/stat", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/stat");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
	fclose(fp);

	delay = (UPDATE_INTERVAL - (UPDATE_INTERVAL - 1));
	sleep(delay);

	fp = fopen("/proc/stat", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/stat");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
	fclose(fp);

	perc = 100 * ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
	return smprintf("%d%%", perc);
}

static char *
datetime(const char *fmt)
{
	time_t t;
	char str[80];

	t = time(NULL);
	if (strftime(str, sizeof(str), fmt, localtime(&t)) == 0) {
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%s", str);
}

static char *
disk_free(const char *mnt)
{
	struct statvfs fs;

	if (statvfs(mnt, &fs) < 0) {
		warn("Failed to get filesystem info");
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%f", (float)fs.f_bsize * (float)fs.f_bfree / 1024 / 1024 / 1024);
}

static char *
disk_perc(const char *mnt)
{
	int perc;
	struct statvfs fs;

	if (statvfs(mnt, &fs) < 0) {
		warn("Failed to get filesystem info");
		return smprintf(UNKNOWN_STR);
	}

	perc = 100 * (1.0f - ((float)fs.f_bfree / (float)fs.f_blocks));

	return smprintf("%d%%", perc);
}

static char *
disk_total(const char *mnt)
{
	struct statvfs fs;

	if (statvfs(mnt, &fs) < 0) {
		warn("Failed to get filesystem info");
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%f", (float)fs.f_bsize * (float)fs.f_blocks / 1024 / 1024 / 1024);
}

static char *
disk_used(const char *mnt)
{
	struct statvfs fs;

	if (statvfs(mnt, &fs) < 0) {
		warn("Failed to get filesystem info");
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%f", (float)fs.f_bsize * ((float)fs.f_blocks - (float)fs.f_bfree) / 1024 / 1024 / 1024);
}

static char *
entropy(void)
{
	int num;
	FILE *fp;

	fp= fopen("/proc/sys/kernel/random/entropy_avail", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/sys/kernel/random/entropy_avail");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "%d", &num);
	fclose(fp);

	return smprintf("%d", num);
}

static char *
gid(void)
{
	return smprintf("%d", getgid());
}

static char *
hostname(void)
{
	char buf[HOST_NAME_MAX];

	if (gethostname(buf, sizeof(buf)) == -1) {
		warn("hostname");
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%s", buf);
}

static char *
ip(const char *iface)
{
	struct ifaddrs *ifaddr, *ifa;
	int s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		warn("Failed to get IP address for interface %s", iface);
		return smprintf(UNKNOWN_STR);
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if ((strcmp(ifa->ifa_name, iface) == 0) && (ifa->ifa_addr->sa_family == AF_INET)) {
			if (s != 0) {
				warnx("Failed to get IP address for interface %s", iface);
				return smprintf(UNKNOWN_STR);
			}
			return smprintf("%s", host);
		}
	}

	freeifaddrs(ifaddr);

	return smprintf(UNKNOWN_STR);
}

static char *
load_avg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		warnx("Failed to get the load avg");
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

static char *
ram_free(void)
{
	long free;
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "MemFree: %ld kB\n", &free);
	fclose(fp);

	return smprintf("%f", (float)free / 1024 / 1024);
}

static char *
ram_perc(void)
{
	long total, free, buffers, cached;
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "MemTotal: %ld kB\n", &total);
	fscanf(fp, "MemFree: %ld kB\n", &free);
	fscanf(fp, "MemAvailable: %ld kB\nBuffers: %ld kB\n", &buffers, &buffers);
	fscanf(fp, "Cached: %ld kB\n", &cached);
	fclose(fp);

	return smprintf("%d%%", 100 * ((total - free) - (buffers + cached)) / total);
}

static char *
ram_total(void)
{
	long total;
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "MemTotal: %ld kB\n", &total);
	fclose(fp);

	return smprintf("%f", (float)total / 1024 / 1024);
}

static char *
ram_used(void)
{
	long free, total, buffers, cached;
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "MemTotal: %ld kB\n", &total);
	fscanf(fp, "MemFree: %ld kB\n", &free);
	fscanf(fp, "MemAvailable: %ld kB\nBuffers: %ld kB\n", &buffers, &buffers);
	fscanf(fp, "Cached: %ld kB\n", &cached);
	fclose(fp);

	return smprintf("%f", (float)(total - free - buffers - cached) / 1024 / 1024);
}

static char *
run_command(const char *cmd)
{
	FILE *fp;
	char buf[1024] = "n/a";

	fp = popen(cmd, "r");
	if (fp == NULL) {
		warn("Failed to get command output for %s", cmd);
		return smprintf(UNKNOWN_STR);
	}
	fgets(buf, sizeof(buf)-1, fp);
	pclose(fp);

	buf[strlen(buf)] = '\0';
	strtok(buf, "\n");

	return smprintf("%s", buf);
}

static char *
swap_free(void)
{
	long total, free;
	FILE *fp;
	char buf[2048];
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	bytes_read = fread(buf, sizeof(char), sizeof(buf), fp);
	buf[bytes_read] = '\0';
	fclose(fp);
	if (bytes_read == 0 || bytes_read == sizeof(buf)) {
		warn("Failed to read /proc/meminfo\n");
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapTotal");
	sscanf(match, "SwapTotal: %ld kB\n", &total);
	if (total == 0) {
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapFree");
	sscanf(match, "SwapFree: %ld kB\n", &free);

	return smprintf("%f", (float)free / 1024 / 1024);
}

static char *
swap_perc(void)
{
	long total, free, cached;
	FILE *fp;
	char buf[2048];
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	bytes_read = fread(buf, sizeof(char), sizeof(buf), fp);
	buf[bytes_read] = '\0';
	fclose(fp);
	if (bytes_read == 0 || bytes_read == sizeof(buf)) {
		warn("Failed to read /proc/meminfo\n");
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapTotal");
	sscanf(match, "SwapTotal: %ld kB\n", &total);
	if (total == 0) {
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapCached");
	sscanf(match, "SwapCached: %ld kB\n", &cached);

	match = strstr(buf, "SwapFree");
	sscanf(match, "SwapFree: %ld kB\n", &free);

	return smprintf("%d%%", 100 * (total - free - cached) / total);
}

static char *
swap_total(void)
{
	long total;
	FILE *fp;
	char buf[2048];
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	bytes_read = fread(buf, sizeof(char), sizeof(buf), fp);
	buf[bytes_read] = '\0';
	fclose(fp);
	if (bytes_read == 0 || bytes_read == sizeof(buf)) {
		warn("Failed to read /proc/meminfo\n");
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapTotal");
	sscanf(match, "SwapTotal: %ld kB\n", &total);
	if (total == 0) {
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%f", (float)total / 1024 / 1024);
}

static char *
swap_used(void)
{
	long total, free, cached;
	FILE *fp;
	char buf[2048];
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/meminfo");
		return smprintf(UNKNOWN_STR);
	}
	bytes_read = fread(buf, sizeof(char), sizeof(buf), fp);
	buf[bytes_read] = '\0';
	fclose(fp);
	if (bytes_read == 0 || bytes_read == sizeof(buf)) {
		warn("Failed to read /proc/meminfo\n");
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapTotal");
	sscanf(match, "SwapTotal: %ld kB\n", &total);
	if (total == 0) {
		return smprintf(UNKNOWN_STR);
	}

	match = strstr(buf, "SwapCached");
	sscanf(match, "SwapCached: %ld kB\n", &cached);

	match = strstr(buf, "SwapFree");
	sscanf(match, "SwapFree: %ld kB\n", &free);

	return smprintf("%f", (float)(total - free - cached) / 1024 / 1024);
}

static char *
temp(const char *file)
{
	int temp;
	FILE *fp;

	fp = fopen(file, "r");
	if (fp == NULL) {
		warn("Failed to open file %s", file);
		return smprintf(UNKNOWN_STR);
	}
	fscanf(fp, "%d", &temp);
	fclose(fp);

	return smprintf("%d°C", temp / 1000);
}

static char *
uptime(void)
{
	struct sysinfo info;
	int h = 0;
	int m = 0;

	sysinfo(&info);
	h = info.uptime / 3600;
	m = (info.uptime - h * 3600 ) / 60;

	return smprintf("%dh %dm", h, m);
}

static char *
username(void)
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);

	if (pw == NULL) {
		warn("Failed to get username");
		return smprintf(UNKNOWN_STR);
	}

	return smprintf("%s", pw->pw_name);
}

static char *
uid(void)
{
	return smprintf("%d", geteuid());
}


static char *
vol_perc(const char *card)
{
	int mute;
	long int vol, max, min;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;
	snd_mixer_selem_id_t *s_elem;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);
	snd_mixer_selem_id_malloc(&s_elem);
	snd_mixer_selem_id_set_name(s_elem, "Master");
	elem = snd_mixer_find_selem(handle, s_elem);

	if (elem == NULL) {
		snd_mixer_selem_id_free(s_elem);
		snd_mixer_close(handle);
		warn("Failed to get volume percentage for %s", card);
		return smprintf(UNKNOWN_STR);
	}

	snd_mixer_handle_events(handle);
	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_get_playback_volume(elem, 0, &vol);
	snd_mixer_selem_get_playback_switch(elem, 0, &mute);

	snd_mixer_selem_id_free(s_elem);
	snd_mixer_close(handle);

	if (!mute)
		return smprintf("mute");
	else if (max == 0)
		return smprintf("0%%");
	else
		return smprintf("%lu%%", ((uint_fast16_t)(vol * 100) / max));
}

static char *
wifi_perc(const char *iface)
{
	int perc;
	char buf[255];
	char *datastart;
	char status[5];
	FILE *fp;

	ccat(3, "/sys/class/net/", iface, "/operstate");
	fp = fopen(concat, "r");
	if (fp == NULL) {
		warn("Failed to open file %s", concat);
		return smprintf(UNKNOWN_STR);
	}
	fgets(status, 5, fp);
	fclose(fp);
	if(strcmp(status, "up\n") != 0) {
		return smprintf(UNKNOWN_STR);
	}

	fp = fopen("/proc/net/wireless", "r");
	if (fp == NULL) {
		warn("Failed to open file /proc/net/wireless");
		return smprintf(UNKNOWN_STR);
	}
	ccat(2, iface, ":");
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);
	fclose(fp);

	datastart = strstr(buf, concat);
	if (datastart != NULL) {
		datastart = strstr(buf, ":");
		sscanf(datastart + 1, " %*d   %d  %*d  %*d		  %*d	   %*d		%*d		 %*d	  %*d		 %*d", &perc);
	}

	return smprintf("%d%%", perc);
}

static char *
wifi_essid(const char *iface)
{
	char id[IW_ESSID_MAX_SIZE+1];
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct iwreq wreq;

	memset(&wreq, 0, sizeof(struct iwreq));
	wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;
	sprintf(wreq.ifr_name, iface);
	if (sockfd == -1) {
		warn("Failed to get ESSID for interface %s", iface);
		return smprintf(UNKNOWN_STR);
	}
	wreq.u.essid.pointer = id;
	if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
		warn("Failed to get ESSID for interface %s", iface);
		return smprintf(UNKNOWN_STR);
	}

	close(sockfd);

	if (strcmp((char *)wreq.u.essid.pointer, "") == 0)
		return smprintf(UNKNOWN_STR);
	else
		return smprintf("%s", (char *)wreq.u.essid.pointer);
}

static char *
kernel_release(void)
{
	struct utsname udata;
	if (uname(&udata) < 0)
		return smprintf(UNKNOWN_STR);

	return smprintf("%s", udata.release);
}

static void
set_status(const char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

static void
sighandler(const int signo)
{
	if (signo == SIGTERM || signo == SIGINT) {
		done = 1;
	}
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-dhov]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	unsigned short int i;
	char status_string[4096];
	char *res, *element;
	struct arg argument;
	struct sigaction act;

	ARGBEGIN {
		case 'd':
			dflag = 1;
			break;
		case 'o':
			oflag = 1;
			break;
		case 'v':
			printf("slstatus %s (C) 2016 slstatus engineers\n", VERSION);
			return 0;
		default:
			usage();
	} ARGEND

	if (dflag && oflag) {
		usage();
	}
	if (dflag && daemon(1, 1) < 0) {
		err(1, "daemon");
	}

	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;
	sigaction(SIGINT,  &act, 0);
	sigaction(SIGTERM, &act, 0);

	if (!oflag) {
		dpy = XOpenDisplay(NULL);
	}

	setlocale(LC_ALL, "");

	while (!done) {
		status_string[0] = '\0';

		for (i = 0; i < sizeof(args) / sizeof(args[0]); ++i) {
			argument = args[i];
			if (argument.args == NULL) {
				res = argument.func();
			} else {
				res = argument.func(argument.args);
			}
			element = smprintf(argument.fmt, res);
			if (element == NULL) {
				element = smprintf(UNKNOWN_STR);
				warnx("Failed to format output");
			}
			strlcat(status_string, element, sizeof(status_string));
			free(res);
			free(element);
		}

		if (!oflag) {
			set_status(status_string);
		} else {
			printf("%s\n", status_string);
		}

		/*
		 * subtract delay time spend in function
		 * calls from the actual global delay time
		 */
		sleep(UPDATE_INTERVAL - delay);
		delay = 0;
	}

	if (!oflag) {
		set_status(NULL);
		XCloseDisplay(dpy);
	}

	return 0;
}
