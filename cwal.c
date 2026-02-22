#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "config.h"

#ifndef VERSION
	#define VERSION "unknown"
#endif
#define HASH(r, g, b) ((r << 16) | (g << 8) | b)

typedef struct bucket_s {
	uint32_t color;
	uint32_t count;
	struct bucket_s *next;
} bucket_t;

static void usage(void);
static void version(void);
static void setup(void);
static void run(void);
static void color_add(uint32_t color);
static void clean(void);

static char *filename;
static unsigned char *bitmap;
static int channels;
static uint32_t bitmap_size;
static bucket_t *hashmap[HASHMAP_SIZE];

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
	int width, height;
	const char *message;
	bitmap = stbi_load(filename, &width, &height, &channels, 0);
	if (!bitmap) {
		message = stbi_failure_reason();
		fprintf(stderr, "cwal: %s: stb_image: %s\n", filename, message);
		exit(1);
	}
	if (channels < 3) {
		fprintf(stderr, "cwal: %s: channels < 3\n", filename);
		clean();
		exit(1);
	}
	bitmap_size = width * height;
}

static void
run(void)
{
	uint32_t i, j;
	for (i = 0; i < bitmap_size; i++) {
		j = i * channels;
		color_add(HASH(bitmap[j], bitmap[j + 1], bitmap[j + 2]));
	}
}

uint32_t t;
static void
color_add(uint32_t color)
{
	t++;
	uint32_t i;
	bucket_t *bucket = NULL;
	i = color & (HASHMAP_SIZE - 1);
	bucket = hashmap[i];
	while (bucket) {
		if (bucket->color == color) {
			bucket->count++;
			return;
		}
		bucket = bucket->next;
	}
	bucket = malloc(sizeof(bucket_t));
	bucket->color = color;
	bucket->count = 1;
	bucket->next = hashmap[i];
	hashmap[i] = bucket;
}

static void
clean(void)
{
	stbi_image_free(bitmap);
}