#ifndef SENSORINO_UTILS_H
#define SENSORINO_UTILS_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

#define likely(x)	__builtin_expect((x), 1)
#define unlikely(x)	__builtin_expect((x), 0)

#define _glue(x, y)	x##y
#define glue(...)	_glue(__VA_ARGS__)

#ifndef NULL
# define NULL 0
#endif

#include <Arduino.h>

static void pgmWrite(Stream &stream, const char *string) { \
    char buf[strlen_P(string) + 1];
    strcpy_P(buf, string);
    stream.write(buf);
}

/* Maths */
#define EPSILON 0.0001f

#endif // whole file
/* vim: set sw=4 ts=4 et: */
