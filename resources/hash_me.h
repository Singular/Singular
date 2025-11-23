#ifndef HASH_ME_H
#define HASH_ME_H
#include <stdint.h>     /* defines uint32_t etc */
#include <stddef.h>     /* size_t */
#ifdef __cplusplus
extern "C" {
#endif
uint32_t hashlittle( const void *key, size_t length);
#ifdef __cplusplus
}
#endif
#endif
