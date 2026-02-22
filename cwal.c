#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "config.h"

#ifndef VERSION
	#define VERSION "unknown"
#endif

static void usage(void);
static void version(void);
static void setup(void);
static void run(void);
static void clean(void);

static char *filename;
static unsigned char *bitmap;

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
	run();
	clean();
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
	int width, height, channels;
	const char *message;
	bitmap = stbi_load(filename, &width, &height, &channels, 0);
	if (!bitmap) {
		message = stbi_failure_reason();
		fprintf(stderr, "cwal: %s: stb_image: %s\n", filename, message);
		exit(1);
	}
}

static void
run(void)
{
	puts(filename);
}

static void
clean(void)
{
	stbi_image_free(bitmap);
}