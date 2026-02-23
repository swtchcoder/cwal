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
#define HASH(r, g, b) ((r << 11) | (g << 5) | b)

typedef struct bucket_s {
	uint32_t color;
	uint32_t count;
	struct bucket_s *next;
} bucket_t;

static void usage(void);
static void version(void);
static void setup(void);
static void count(void);
static void bucket_add(uint32_t color);
static void flatten(void); /* it corrupts the hashmap but who care */
static void sort(void);
static int compare(const void* a, const void* b);
static void clean(void);

static char *filename;
static unsigned char *bitmap;
static int channels;
static uint32_t bitmap_size;
static bucket_t **hashmap;
static uint32_t bucket_count;

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
	count();
	flatten();
	sort();
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
	hashmap = calloc(HASHMAP_SIZE, sizeof(bucket_t *));
	if (!hashmap) {
		perror("cwal: ");
		clean();
		exit(1);
	}
}

static void
count(void)
{
	uint32_t i, j;
	for (i = 0; i < bitmap_size; i++) {
		j = i * channels;
		bucket_add(HASH(bitmap[j], bitmap[j + 1], bitmap[j + 2]));
	}
}

static void
bucket_add(uint32_t color)
{
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
	if (!bucket) {
		perror("cwal: ");
		clean();
		exit(1);
	}
	bucket->color = color;
	bucket->count = 1;
	bucket->next = hashmap[i];
	hashmap[i] = bucket;
	bucket_count++;
}

static void
flatten(void)
{
	uint32_t i, j;
	bucket_t *bucket, *next;
	bucket_t **tmp;
	j = 0;
	tmp = calloc(bucket_count, sizeof(bucket_t *));
	if (!tmp) {
		perror("cwal: ");
		clean();
		exit(1);
	}
	for (i = 0; i < HASHMAP_SIZE; i++) {
		bucket = hashmap[i];
		while (bucket) {
			next = bucket->next;
			bucket->next = NULL;
			tmp[j++] = bucket;
			bucket = next;
		}
	}
	free(hashmap);
	hashmap = tmp;
	bucket_count = j;
}

static void
sort(void)
{
	qsort(hashmap, bucket_count, sizeof(bucket_t *), compare);
}

static int
compare(const void* a, const void* b)
{
	const bucket_t *_a = *(const bucket_t **)a;
	const bucket_t *_b = *(const bucket_t **)b;

	if (_a->count < _b->count) {
		return 1;
	}
	if (_a->count > _b->count) {
		return -1;
	}
	return 0;
}

static void
clean(void)
{
	uint32_t i;
	for (i = 0; i < bucket_count; i++) {
		free(hashmap[i]);
	}
	free(hashmap);
	stbi_image_free(bitmap);
}