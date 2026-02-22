VERSION=unknown

CC=cc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -Os -DVERSION=\"${VERSION}\"
LDFLAGS=