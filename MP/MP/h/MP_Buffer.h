/****************************************************************
 *
 *   HEADER FILE:  MP_Buffer.h
 *
 *       Contains all declarations related to buffer and buffer pool.
 *
 ****************************************************************/
#ifndef _MP_Buffer_h
#define _MP_Buffer_h

#ifndef _MP_BuffTypes_h
#include "MP_BuffTypes.h"
#endif

EXTERN MP_Status_t m_create_pool _ANSI_ARGS_((MP_Env_pt env, int buffer_size,
                                       int max_free_buff, int init_free_buff));
EXTERN void m_free_pool     _ANSI_ARGS_((buffer_pool_pt buff_pool));
EXTERN void m_free_in_bufs  _ANSI_ARGS_((MP_Link_pt link));
EXTERN int MP_ResetLink     _ANSI_ARGS_((MP_Link_pt link));
EXTERN void destroy_buffer  _ANSI_ARGS_((buffer_handle_pt buff, long buffsize));
EXTERN void free_buffer     _ANSI_ARGS_((buffer_pool_pt buff_pool,
                                         buffer_handle_pt buff));
EXTERN buffer_handle_pt create_buffer _ANSI_ARGS_((buffer_pool_pt buff_pool));
EXTERN buffer_handle_pt get_buffer    _ANSI_ARGS_((buffer_pool_pt buff_pool));
EXTERN buffer_handle_pt get_o_buff    _ANSI_ARGS_((MP_Link_pt link));
EXTERN buffer_handle_pt get_i_buff    _ANSI_ARGS_((MP_Link_pt link));
EXTERN MP_Status_t reset_i_buff       _ANSI_ARGS_((MP_Link_pt link));

#endif /* _MP_Buffer_h */
