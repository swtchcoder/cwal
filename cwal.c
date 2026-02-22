#include <stdio.h>
#include <string.h>
#include "config.h"

#ifndef VERSION
	#define VERSION "unknown"
#endif

static void usage(void);
static void version(void);
static void setup(void);

static char *filename;

int
main(int argc, char *argv[])
{
	if (argc == 1) {
		usage();
		return 0;
	}
	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		usage();
		return 0;
	}
	if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
		version();
		return 0;
	}
	filename = argv[1];
	setup();
	return 0;
}

static void
usage(void)
{
	puts("usage: cwal [-h | -v | image]\n"
	     "options:\n"
	     "\t-h, --help\t\tshow this help message and exit\n"
	     "\t-v, --version\t\tprint cwal version");
}

static void
version(void)
{
	puts("cwal "VERSION);
}

static void
setup(void)
{
	puts(filename);
}