
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_block.h:

Program Listing for File vlc_block.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_block.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_block.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_block.h: Data blocks management functions
    *****************************************************************************
    * Copyright (C) 2003 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Laurent Aimar <fenrir@via.ecp.fr>
    *
    * This program is free software; you can redistribute it and/or modify it
    * under the terms of the GNU Lesser General Public License as published by
    * the Free Software Foundation; either version 2.1 of the License, or
    * (at your option) any later version.
    *
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    * GNU Lesser General Public License for more details.
    *
    * You should have received a copy of the GNU Lesser General Public License
    * along with this program; if not, write to the Free Software Foundation,
    * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
    *****************************************************************************/
   
   #ifndef VLC_BLOCK_H
   #define VLC_BLOCK_H 1
   
   
   #include <sys/types.h>  /* for ssize_t */
   
   /****************************************************************************
    * block:
    ****************************************************************************
    * - i_flags may not always be set (ie could be 0, even for a key frame
    *      it depends where you receive the buffer (before/after a packetizer
    *      and the demux/packetizer implementations.
    * - i_dts/i_pts could be VLC_TICK_INVALID, it means no pts/dts
    * - i_length: length in microseond of the packet, can be null except in the
    *      sout where it is mandatory.
    *
    * - i_buffer number of valid data pointed by p_buffer
    *      you can freely decrease it but never increase it yourself
    *      (use block_Realloc)
    * - p_buffer: pointer over datas. You should never overwrite it, you can
    *   only incremment it to skip datas, in others cases use block_Realloc
    *   (don't duplicate yourself in a bigger buffer, block_Realloc is
    *   optimised for preheader/postdatas increase)
    ****************************************************************************/
   
   #define BLOCK_FLAG_DISCONTINUITY 0x0001
   #define BLOCK_FLAG_TYPE_I        0x0002
   #define BLOCK_FLAG_TYPE_P        0x0004
   #define BLOCK_FLAG_TYPE_B        0x0008
   #define BLOCK_FLAG_TYPE_PB       0x0010
   #define BLOCK_FLAG_HEADER        0x0020
   #define BLOCK_FLAG_END_OF_SEQUENCE 0x0040
   #define BLOCK_FLAG_CLOCK         0x0080
   #define BLOCK_FLAG_SCRAMBLED     0x0100
   #define BLOCK_FLAG_PREROLL       0x0200
   #define BLOCK_FLAG_CORRUPTED     0x0400
   #define BLOCK_FLAG_TOP_FIELD_FIRST 0x0800
   #define BLOCK_FLAG_BOTTOM_FIELD_FIRST 0x1000
   #define BLOCK_FLAG_SINGLE_FIELD  0x2000
   
   #define BLOCK_FLAG_INTERLACED_MASK \
       (BLOCK_FLAG_TOP_FIELD_FIRST|BLOCK_FLAG_BOTTOM_FIELD_FIRST|BLOCK_FLAG_SINGLE_FIELD)
   
   #define BLOCK_FLAG_TYPE_MASK \
       (BLOCK_FLAG_TYPE_I|BLOCK_FLAG_TYPE_P|BLOCK_FLAG_TYPE_B|BLOCK_FLAG_TYPE_PB)
   
   /* These are for input core private usage only */
   #define BLOCK_FLAG_CORE_PRIVATE_MASK  0x00ff0000
   #define BLOCK_FLAG_CORE_PRIVATE_SHIFT 16
   
   /* These are for module private usage only */
   #define BLOCK_FLAG_PRIVATE_MASK  0xff000000
   #define BLOCK_FLAG_PRIVATE_SHIFT 24
   
   typedef void (*block_free_t) (block_t *);
   
   struct block_t
   {
       block_t    *p_next;
   
       uint8_t    *p_buffer; 
       size_t      i_buffer; 
       uint8_t    *p_start; 
       size_t      i_size; 
   
       uint32_t    i_flags;
       unsigned    i_nb_samples; /* Used for audio */
   
       vlc_tick_t  i_pts;
       vlc_tick_t  i_dts;
       vlc_tick_t  i_length;
   
       /* Rudimentary support for overloading block (de)allocation. */
       block_free_t pf_release;
   };
   
   VLC_API void block_Init( block_t *, void *, size_t );
   
   VLC_API block_t *block_Alloc(size_t size) VLC_USED VLC_MALLOC;
   
   VLC_API block_t *block_TryRealloc(block_t *, ssize_t pre, size_t body) VLC_USED;
   
   VLC_API block_t *block_Realloc(block_t *, ssize_t pre, size_t body) VLC_USED;
   
   static inline void block_Release(block_t *block)
   {
       block->pf_release(block);
   }
   
   static inline void block_CopyProperties( block_t *dst, block_t *src )
   {
       dst->i_flags   = src->i_flags;
       dst->i_nb_samples = src->i_nb_samples;
       dst->i_dts     = src->i_dts;
       dst->i_pts     = src->i_pts;
       dst->i_length  = src->i_length;
   }
   
   VLC_USED
   static inline block_t *block_Duplicate( block_t *p_block )
   {
       block_t *p_dup = block_Alloc( p_block->i_buffer );
       if( p_dup == NULL )
           return NULL;
   
       block_CopyProperties( p_dup, p_block );
       memcpy( p_dup->p_buffer, p_block->p_buffer, p_block->i_buffer );
   
       return p_dup;
   }
   
   VLC_API block_t *block_heap_Alloc(void *, size_t) VLC_USED VLC_MALLOC;
   
   VLC_API block_t *block_mmap_Alloc(void *addr, size_t length) VLC_USED VLC_MALLOC;
   
   VLC_API block_t * block_shm_Alloc(void *addr, size_t length) VLC_USED VLC_MALLOC;
   
   VLC_API block_t *block_File(int fd, bool write) VLC_USED VLC_MALLOC;
   
   VLC_API block_t *block_FilePath(const char *, bool write) VLC_USED VLC_MALLOC;
   
   static inline void block_Cleanup (void *block)
   {
       block_Release ((block_t *)block);
   }
   #define block_cleanup_push( block ) vlc_cleanup_push (block_Cleanup, block)
   
   
   /****************************************************************************
    * Chains of blocks functions helper
    ****************************************************************************
    * - block_ChainAppend : append a block to the last block of a chain. Try to
    *      avoid using with a lot of data as it's really slow, prefer
    *      block_ChainLastAppend, p_block can be NULL
    * - block_ChainLastAppend : use a pointer over a pointer to the next blocks,
    *      and update it.
    * - block_ChainRelease : release a chain of block
    * - block_ChainExtract : extract data from a chain, return real bytes counts
    * - block_ChainGather : gather a chain, free it and return one block.
    ****************************************************************************/
   static inline void block_ChainAppend( block_t **pp_list, block_t *p_block )
   {
       if( *pp_list == NULL )
       {
           *pp_list = p_block;
       }
       else
       {
           block_t *p = *pp_list;
   
           while( p->p_next ) p = p->p_next;
           p->p_next = p_block;
       }
   }
   
   static inline void block_ChainLastAppend( block_t ***ppp_last, block_t *p_block )
   {
       block_t *p_last = p_block;
   
       **ppp_last = p_block;
   
       while( p_last->p_next ) p_last = p_last->p_next;
       *ppp_last = &p_last->p_next;
   }
   
   static inline void block_ChainRelease( block_t *p_block )
   {
       while( p_block )
       {
           block_t *p_next = p_block->p_next;
           block_Release( p_block );
           p_block = p_next;
       }
   }
   
   static size_t block_ChainExtract( block_t *p_list, void *p_data, size_t i_max )
   {
       size_t  i_total = 0;
       uint8_t *p = (uint8_t*)p_data;
   
       while( p_list && i_max )
       {
           size_t i_copy = __MIN( i_max, p_list->i_buffer );
           memcpy( p, p_list->p_buffer, i_copy );
           i_max   -= i_copy;
           i_total += i_copy;
           p       += i_copy;
   
           p_list = p_list->p_next;
       }
       return i_total;
   }
   
   static inline void block_ChainProperties( block_t *p_list, int *pi_count, size_t *pi_size, vlc_tick_t *pi_length )
   {
       size_t i_size = 0;
       vlc_tick_t i_length = 0;
       int i_count = 0;
   
       while( p_list )
       {
           i_size += p_list->i_buffer;
           i_length += p_list->i_length;
           i_count++;
   
           p_list = p_list->p_next;
       }
   
       if( pi_size )
           *pi_size = i_size;
       if( pi_length )
           *pi_length = i_length;
       if( pi_count )
           *pi_count = i_count;
   }
   
   static inline block_t *block_ChainGather( block_t *p_list )
   {
       size_t  i_total = 0;
       vlc_tick_t i_length = 0;
       block_t *g;
   
       if( p_list->p_next == NULL )
           return p_list;  /* Already gathered */
   
       block_ChainProperties( p_list, NULL, &i_total, &i_length );
   
       g = block_Alloc( i_total );
       if( !g )
           return NULL;
       block_ChainExtract( p_list, g->p_buffer, g->i_buffer );
   
       g->i_flags = p_list->i_flags;
       g->i_pts   = p_list->i_pts;
       g->i_dts   = p_list->i_dts;
       g->i_length = i_length;
   
       /* free p_list */
       block_ChainRelease( p_list );
       return g;
   }
   
   
   VLC_API block_fifo_t *block_FifoNew(void) VLC_USED VLC_MALLOC;
   
   VLC_API void block_FifoRelease(block_fifo_t *);
   
   VLC_API void block_FifoEmpty(block_fifo_t *);
   
   VLC_API void block_FifoPut(block_fifo_t *fifo, block_t *block);
   
   VLC_API block_t *block_FifoGet(block_fifo_t *) VLC_USED;
   
   VLC_API block_t *block_FifoShow(block_fifo_t *);
   
   size_t block_FifoSize(block_fifo_t *) VLC_USED VLC_DEPRECATED;
   VLC_API size_t block_FifoCount(block_fifo_t *) VLC_USED VLC_DEPRECATED;
   
   typedef struct block_fifo_t vlc_fifo_t;
   
   VLC_API void vlc_fifo_Lock(vlc_fifo_t *);
   
   VLC_API void vlc_fifo_Unlock(vlc_fifo_t *);
   
   VLC_API void vlc_fifo_Signal(vlc_fifo_t *);
   
   VLC_API void vlc_fifo_Wait(vlc_fifo_t *);
   
   VLC_API void vlc_fifo_WaitCond(vlc_fifo_t *, vlc_cond_t *);
   
   int vlc_fifo_TimedWaitCond(vlc_fifo_t *, vlc_cond_t *, vlc_tick_t);
   
   VLC_API void vlc_fifo_QueueUnlocked(vlc_fifo_t *, block_t *);
   
   VLC_API block_t *vlc_fifo_DequeueUnlocked(vlc_fifo_t *) VLC_USED;
   
   VLC_API block_t *vlc_fifo_DequeueAllUnlocked(vlc_fifo_t *) VLC_USED;
   
   VLC_API size_t vlc_fifo_GetCount(const vlc_fifo_t *) VLC_USED;
   
   VLC_API size_t vlc_fifo_GetBytes(const vlc_fifo_t *) VLC_USED;
   
   VLC_USED static inline bool vlc_fifo_IsEmpty(const vlc_fifo_t *fifo)
   {
       return vlc_fifo_GetCount(fifo) == 0;
   }
   
   static inline void vlc_fifo_Cleanup(void *fifo)
   {
       vlc_fifo_Unlock((vlc_fifo_t *)fifo);
   }
   #define vlc_fifo_CleanupPush(fifo) vlc_cleanup_push(vlc_fifo_Cleanup, fifo)
   
   
   
   #endif /* VLC_BLOCK_H */
