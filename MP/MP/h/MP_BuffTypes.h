/****************************************************************
 *
 *   HEADER FILE:  MP_BuffTypes.h
 *
 *       Contains all declarations related to buffer and buffer pool.
 *
 ****************************************************************/
#ifndef _MP_BuffTypes_h
#define _MP_BuffTypes_h


#define MP_MINIMUM_BUFF_SIZE       512
#define MP_DEFAULT_BUFF_SIZE      1024
#define MP_DEFAULT_MAX_FREE_BUFF    32
#define MP_DEFAULT_INIT_FREE_BUFF   16

struct buffer_handle {
        char * the_buffer; /* ptr to block of memory */
        char * buff;       /* byte aligned ptr to buffer */
        struct buffer_handle *next;
};

typedef struct buffer_handle buffer_handle_t;
typedef buffer_handle_t  *buffer_handle_pt;

struct buffer_pool {
        int      buff_size;           /* buffer size */
        int      max_free_buff;       /* max. no. of free buff. */
        int      curr_free_buff;      /* current no. of free buff. */
        buffer_handle_pt   buff;      /* ptr to list of buff handle */
};

typedef struct buffer_pool buffer_pool_t;
typedef buffer_pool_t *buffer_pool_pt;

#endif
