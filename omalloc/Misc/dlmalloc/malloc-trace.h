/*
 * malloc-trace.h
 */

#define MTRACE_VERSION 0xdead0002

enum action_code {
	CODE_MALLOC = 0,
	CODE_FREE,
	CODE_REALLOC,
	CODE_CALLOC,
	CODE_CFREE,
	CODE_MEMALIGN,
	CODE_TIMESTAMP,
	CODE_LAST
};

typedef struct _action {
	int code;
	size_t size;
	void *ptr;
	void *ptr2;
} ACTION;
