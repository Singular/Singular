/*******************************************************************
 *
 *                                                                  
 *                    MP version 1.1.2:  Multi Protocol
 *                    Kent State University, Kent, OH
 *                 Authors:  S. Gray, N. Kajler, P. Wang 
 *          (C) 1993, 1994, 1995, 1996, 1997 All Rights Reserved
 * 
 *                                 NOTICE
 * 
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation for non-commercial purposes and without fee is
 *  hereby granted provided that the above copyright notice appear in all
 *  copies and that both the copyright notice and this permission notice
 *  appear in supporting documentation.
 *
 *  Neither Kent State University nor the Authors make any representations
 *  about the suitability of this software for any purpose.  The MP Library
 *  is distributed in the hope that it will be useful, but is provided  "as
 *  is" and without any warranty of any kind and without even the implied
 *  warranty of merchantability or fitness for a particular purpose.
 *
 *    IMPORTANT ADDITION: as of September 2006, MP is also licenced under GPL
 *
 ***************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_Buffer.c,v 1.2 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"

/*
 * create_buffer() is used to create buffer handle and allocate memory to
 * buffer. create_buffer() gaurantees that in the handle
 *    1. the_buffer points to the begining of the memory block allocated to the
 *        buffer.
 *    2. buff points to the byte-aligned starting point at the begning of the
 *       buffer.
 *    3. next is NULL
 * and the first long integer starting from buff is zeroed.
 * create_buffer() returns NULL if failure.
 */

#ifdef __STDC__
buffer_handle_pt create_buffer(buffer_pool_pt buff_pool)
#else
buffer_handle_pt create_buffer(buff_pool)
    buffer_pool_pt buff_pool
#endif
{
    buffer_handle_pt buff;

#ifdef MP_DEBUG
    fprintf(stderr, "create_buffer: entering\n");
#endif

    buff = (buffer_handle_pt)IMP_MemAllocFnc(sizeof(buffer_handle_t));
    if (buff == NULL)
        return NULL;

    buff->buff
        = buff->the_buffer
        = (char *)IMP_MemAllocFnc(buff_pool->buff_size + MP_BytesPerMPUnit*2);

    if (buff->the_buffer == NULL) {
        IMP_MemFreeFnc(buff, sizeof(buffer_handle_t));
        return NULL;
    }

    while ((unsigned int)buff->buff % MP_BytesPerMPUnit != 0)
        buff->buff++;

    *(long*)buff->buff = 0;
    buff->next = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "create_buffer: exiting\n");
#endif

    return buff;
}

/*******************************************************************************
 *
 * destroy_buffer() frees the buffer and and its handle, buff.
 *
 ******************************************************************************/
#ifdef __STDC__
void destroy_buffer(buffer_handle_pt buff,
                    long             buffsize)
#else
void destroy_buffer(buff, buffsize)
    buffer_handle_pt buff;
    long             buffsize;
#endif
{
    if (buff == NULL)
        return;

    if (buff->the_buffer == NULL) {
        IMP_MemFreeFnc(buff, sizeof(buffer_handle_t));
        return;
    }

    IMP_MemFreeFnc(buff->the_buffer, buffsize + MP_BytesPerMPUnit*2);
    IMP_MemFreeFnc(buff, sizeof(buffer_handle_t));

    return;
}


/*******************************************************************************
 *
 * get_buffer() returns a free buffer's handle to the caller.
 * get_buffer() gaurantees that the first byte-aligned long integer
 * is zeroed and the next field in the handle is NULL.
 * get_buffer() returns NULL if fail.
 *
 ******************************************************************************/
#ifdef __STDC__
buffer_handle_pt get_buffer(buffer_pool_pt buff_pool)
#else
buffer_handle_pt get_buffer(buff_pool)
    buffer_pool_pt buff_pool;
#endif
{
    buffer_handle_pt buff_hand;
#ifdef MP_DEBUG
    fprintf(stderr, "get_buffer: entering\n");
#endif

    if (buff_pool->buff != NULL && buff_pool->curr_free_buff != 0) {
        buff_pool->curr_free_buff--;
        buff_hand = buff_pool->buff;
        buff_pool->buff = buff_hand->next;
        buff_hand->next = NULL;
        *(long*)buff_hand->buff = 0;
        return buff_hand;
    }

    return create_buffer(buff_pool);
}


/*******************************************************************************
 *
 * free_buffer() reclaims the buffer, buff, returned by streams.
 * if the number of free buffers already reached maximum,
 * the returned buffer will be destroyed.
 *
 ******************************************************************************/
#ifdef __STDC__
void free_buffer(buffer_pool_pt   buff_pool,
                 buffer_handle_pt buff)
#else
void free_buffer(buff_pool, buff)
    buffer_pool_pt   buff_pool;
    buffer_handle_pt buff;
#endif
{
    if (buff == NULL)
        return;

    if (buff->the_buffer == NULL) {
        IMP_MemFreeFnc(buff, sizeof(buffer_handle_t));
        return;
    }

    if (buff_pool->curr_free_buff >= buff_pool->max_free_buff)
        destroy_buffer(buff, buff_pool->buff_size);
    else {
        buff->next      = buff_pool->buff;
        buff_pool->buff = buff;
        buff_pool->curr_free_buff++;
    }
}


/*******************************************************************************
 *
 * m_create_pool() creates and initializes a buffer pool global to
 * a process. Returns either MP_Success or MP_Failure.
 * arguments:
 *     buffer_size: integer, size of each buffer./
 *     max_free_buffer: maximum number of free buffers to be kept in
 *                      pool at any time.
 *     init_free_buffer: initial number of buffers to be made available
 *                       upon the pool's creation.
 *
 ******************************************************************************/
#ifdef __STDC__
MP_Status_t m_create_pool(MP_Env_pt env,
                          int       buffer_size,
                          int       max_free_buffer,
                          int       init_free_buffer)
#else
MP_Status_t m_create_pool(env, buffer_size, max_free_buffer, init_free_buffer)
    MP_Env_pt env;
    int       buffer_size;
    int       max_free_buffer;
    int       init_free_buffer;
#endif
{
    buffer_handle_pt buff_hand, old_buff_hand;
    int              buff_count;

    /* create the pool structure */
    env->buff_pool = (buffer_pool_pt)IMP_MemAllocFnc(sizeof(buffer_pool_t));
    if (env->buff_pool == NULL)
        return MP_Failure;

    /* check parameter, use default values if necessary */
    for ( ; buffer_size % MP_BytesPerMPUnit != 0; buffer_size++);

    if (buffer_size < MP_MINIMUM_BUFF_SIZE)
        env->buff_pool->buff_size = MP_DEFAULT_BUFF_SIZE;
    else
        env->buff_pool->buff_size = buffer_size;

    if (max_free_buffer <= 0)
        env->buff_pool->max_free_buff
            = max_free_buffer
            = MP_DEFAULT_MAX_FREE_BUFF;
    else
        env->buff_pool->max_free_buff = max_free_buffer;

    if (init_free_buffer <= 0)
        init_free_buffer = MP_DEFAULT_INIT_FREE_BUFF;

    if (init_free_buffer > max_free_buffer)
        init_free_buffer = max_free_buffer;

    env->buff_pool->curr_free_buff = 0;

    /* allocate buffers and make a list out of them */
    env->buff_pool->buff = create_buffer(env->buff_pool);
    if (env->buff_pool->buff == NULL)
        return MP_Failure;

    old_buff_hand = env->buff_pool->buff;

    for (buff_count = 1; buff_count < init_free_buffer; buff_count++) {
        buff_hand = create_buffer(env->buff_pool);
        if (buff_hand == NULL) {
            env->buff_pool->curr_free_buff = buff_count;
            return MP_Failure;
        }
        old_buff_hand->next = buff_hand;
        old_buff_hand = buff_hand;
    }

    env->buff_pool->curr_free_buff = buff_count;

    return MP_Success;
}


/*******************************************************************************
 *
 * m_free_pool() frees all buffers and their handles.
 *
 ******************************************************************************/
#ifdef __STDC__
void m_free_pool(buffer_pool_pt buff_pool)
#else
void m_free_pool(buff_pool)
    buffer_pool_pt buff_pool;
#endif
{
    buffer_handle_pt buff;

    if (buff_pool == NULL)
        return;

    for (buff = buff_pool->buff; buff != NULL; buff = buff_pool->buff) {
        buff_pool->buff = buff->next;
        destroy_buffer(buff, buff_pool->buff_size);
    }

    buff_pool->curr_free_buff = 0;
    IMP_MemFreeFnc(buff_pool, sizeof(buffer_pool_t));

    return;
}


/*******************************************************************************
 *
 * get_o_buff() provides the caller with an output buffer and sets up
 * the pointers in the streams associated with that buffer properly.
 * get_o_buff() first looks for output buffers alerady assigned to the stream,
 * if there is none, it requests for one from the pool.
 * if get_o_buff() fails to get a buffer, it returns NULL.
 * argument:
 *     link: the stream that needs to get an output buffer.
 *
 ******************************************************************************/
#ifdef __STDC__
buffer_handle_pt get_o_buff(MP_Link_pt link)
#else
buffer_handle_pt get_o_buff(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt buff;
    buffer_pool_pt   buff_pool;

#ifdef MP_DEBUG
    fprintf(stderr, "get_o_buff: entering\n");
#endif

    if (link == NULL)
        return NULL;

    buff_pool = link->env->buff_pool;
    /* test if initial call from m_create_link */
    if (link->curr_o_buff == NULL && link->o_buff == NULL) {
        buff = get_buffer(buff_pool);
        if (buff == NULL)
            return NULL;
    }
    /* test if already at end of chain */
    else
        if (link->curr_o_buff->next == NULL) {
            buff = get_buffer(buff_pool);
            if (buff == NULL)
                return NULL;
            link->curr_o_buff->next = buff;
        } else
            /* last possibility: still have allocated free buffers in chain */
            buff = link->curr_o_buff->next;

    link->out_finger     = link->out_base = buff->buff;
    link->out_finger    += sizeof(long);
    link->o_frag_header  = (unsigned long *)buff->buff;
    link->out_boundry    = link->out_finger + buff_pool->buff_size;

    *link->o_frag_header = 0;

#ifdef MP_DEBUG
    fprintf(stderr, "get_o_buff: exiting\n");
#endif

    return buff;
}


/*******************************************************************************
 *
 * get_i_buff() provides the caller with an input buffer and sets up
 * the pointers in the streams associated with that buffer properly.
 * get_i_buff() first look for inpuy buffers alerady assigned to the stream,
 * if there is none, it requests for one from the pool.
 * if get_i_buff() fails to get a buffer, it returns NULL.
 * argument:
 *     link: the stream that needs to get an input buffer.
 *
 ******************************************************************************/
#ifdef __STDC__
buffer_handle_pt get_i_buff(MP_Link_pt link)
#else
buffer_handle_pt get_i_buff(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt buff;
    buffer_pool_pt   buff_pool;


    if (link == NULL)
        return NULL;

    buff_pool = link->env->buff_pool;

    /* test if initial call from m_create_link */
    if (link->curr_i_buff == NULL && link->i_buff == NULL) {
        buff = get_buffer(buff_pool);
        if (buff == NULL)
            return NULL;
    } else
        /* test if at the end of allocated chain */
        if (link->curr_i_buff->next == NULL) {
            buff = get_buffer(buff_pool);
            if (buff == NULL)
                return NULL;
            link->curr_i_buff->next = buff;
        } else
            /* last possibility: still have allocated free buffers in chain */
            buff = link->curr_i_buff->next;

    link->in_finger      = link->in_base = buff->buff;
    link->in_finger     += sizeof(long);
    link->i_frag_header  = (unsigned long *)buff->buff;
    link->in_boundry     = link->in_finger + buff_pool->buff_size;

    *link->i_frag_header = 0;

    return buff;
}



/*******************************************************************************
 *
 * m_free_in_bufs() frees all but the first input buffers of a stream.
 * It then resets the in_???  pointers in the stream according to the first
 * buffer.
 * argument:
 *      link: the stream whose input buffers need to be freed.
 *
 ******************************************************************************/
#ifdef __STDC__
void m_free_in_bufs(MP_Link_pt link)
#else
void m_free_in_bufs(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt cur_buff, old_buff;
    buffer_pool_pt   buff_pool;
    int              counter;


    if (link == NULL || link->i_buff == NULL)
        return;

    buff_pool = link->env->buff_pool;
    cur_buff  = link->i_buff->next;

    for (counter = 0; cur_buff != NULL; counter++) {
        old_buff = cur_buff;
        cur_buff = old_buff->next;
        free_buffer(link->env->buff_pool, old_buff);
    }

    link->i_buff->next   = NULL;
    link->curr_i_buff    = link->i_buff;
    link->in_base        = link->i_buff->buff;
    link->i_frag_header  = (unsigned long *)link->in_base;
    link->in_boundry     = link->in_finger = link->in_base + sizeof(long);
    link->in_boundry    += buff_pool->buff_size;

    *link->i_frag_header = 0;
}


/*******************************************************************************
 *
 * MP_ResetLink() frees all but the first num_o_buff input buffers of a stream.
 * It then resets the out_??? pointers in the stream according to the first
 * buffer.
 * argument:
 *      link: the stream whose output buffers need to be freed.
 *
 ******************************************************************************/
#ifdef __STDC__
int MP_ResetLink(MP_Link_pt link)
#else
int MP_ResetLink(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt cur_buff, old_buff, old_old_buff;
    buffer_pool_pt   buff_pool;
    int              counter;


    if (link == NULL || link->o_buff == NULL)
        return 0;

    buff_pool    = link->env->buff_pool;
    old_old_buff = link->o_buff;
    cur_buff     = link->o_buff->next;

    for (counter = 1; counter < link->num_o_buff && cur_buff != NULL;
         counter++) {
        old_old_buff = cur_buff;
        cur_buff     = cur_buff->next;
    }

    while (cur_buff != NULL) {
        old_buff = cur_buff;
        cur_buff = old_buff->next;
        free_buffer(link->env->buff_pool, old_buff);
    }

    old_old_buff->next   = NULL;

    link->curr_o_buff    = link->o_buff;
    link->out_base       = link->o_buff->buff;
    link->o_frag_header  = (unsigned long *)link->out_base;
    link->out_boundry    = link->out_finger = link->out_base + sizeof(long);
    link->out_boundry   += buff_pool->buff_size;

    *link->o_frag_header = 0;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_INIT_EVENTS)
        MP_LogEvent(link, MP_INIT_EVENT,
                    "MP_ResetLink: reset link sending buffers");
#endif
    return MP_ClearError(link);
}



/*******************************************************************************
 *
 * reset_i_buff() resets all pointers associated with the input stream
 * according to the first buffer of the stream. All buffers remain
 * intact and unaffected by reset_i_buff.
 *
 ******************************************************************************/
#ifdef __STDC__
MP_Status_t reset_i_buff(MP_Link_pt link)
#else
MP_Status_t reset_i_buff(link)
    MP_Link_pt link;
#endif
{
    if (link == NULL)
        return MP_Failure;

    link->curr_i_buff   = link->i_buff;
    link->in_finger     = link->in_base = link->curr_i_buff->buff;
    link->i_frag_header = (unsigned long *)(link->in_base);
    link->in_boundry    = (char *)((unsigned long)(link->in_finger) + sizeof(long)
                          + (*(link->i_frag_header) & ~eor_mask));

    return MP_Success;
}


