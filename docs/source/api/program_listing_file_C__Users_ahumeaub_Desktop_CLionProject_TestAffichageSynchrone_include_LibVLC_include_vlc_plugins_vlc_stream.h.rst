
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_stream.h:

Program Listing for File vlc_stream.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_stream.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_stream.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_stream.h: Stream (between access and demux) descriptor and methods
    *****************************************************************************
    * Copyright (C) 1999-2004 VLC authors and VideoLAN
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
   
   #ifndef VLC_STREAM_H
   #define VLC_STREAM_H 1
   
   #include <vlc_block.h>
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   
   struct stream_t
   {
       VLC_COMMON_MEMBERS
   
       /* Module properties for stream filter */
       module_t    *p_module;
   
       char        *psz_name;
       char        *psz_url; 
       const char  *psz_location; 
       char        *psz_filepath; 
       bool         b_preparsing; 
   
       /* Stream source for stream filter */
       stream_t *p_source;
   
       ssize_t     (*pf_read)(stream_t *, void *buf, size_t len);
   
       block_t    *(*pf_block)(stream_t *, bool *eof);
   
       int         (*pf_readdir)(stream_t *, input_item_node_t *);
   
       int         (*pf_seek)(stream_t *, uint64_t);
   
       int         (*pf_control)(stream_t *, int i_query, va_list);
   
       void *p_sys;
   
       /* Weak link to parent input */
       input_thread_t *p_input;
   };
   
   enum stream_query_e
   {
       /* capabilities */
       STREAM_CAN_SEEK,            
       STREAM_CAN_FASTSEEK,        
       STREAM_CAN_PAUSE,           
       STREAM_CAN_CONTROL_PACE,    
       /* */
       STREAM_GET_SIZE=6,          
       STREAM_IS_DIRECTORY,        
   
       /* */
       STREAM_GET_PTS_DELAY = 0x101,
       STREAM_GET_TITLE_INFO, 
       STREAM_GET_TITLE,       
       STREAM_GET_SEEKPOINT,   
       STREAM_GET_META,        
       STREAM_GET_CONTENT_TYPE,    
       STREAM_GET_SIGNAL,      
       STREAM_GET_TAGS,        
   
       STREAM_SET_PAUSE_STATE = 0x200, 
       STREAM_SET_TITLE,       
       STREAM_SET_SEEKPOINT,   
   
       /* XXX only data read through vlc_stream_Read/Block will be recorded */
       STREAM_SET_RECORD_STATE,     
   
       STREAM_SET_PRIVATE_ID_STATE = 0x1000, /* arg1= int i_private_data, bool b_selected    res=can fail */
       STREAM_SET_PRIVATE_ID_CA,             /* arg1= int i_program_number, uint16_t i_vpid, uint16_t i_apid1, uint16_t i_apid2, uint16_t i_apid3, uint8_t i_length, uint8_t *p_data */
       STREAM_GET_PRIVATE_ID_STATE,          /* arg1=int i_private_data arg2=bool *          res=can fail */
   };
   
   VLC_API ssize_t vlc_stream_Read(stream_t *, void *buf, size_t len) VLC_USED;
   
   VLC_API ssize_t vlc_stream_ReadPartial(stream_t *, void *buf, size_t len)
   VLC_USED;
   
   VLC_API ssize_t vlc_stream_Peek(stream_t *, const uint8_t **, size_t) VLC_USED;
   
   VLC_API block_t *vlc_stream_ReadBlock(stream_t *) VLC_USED;
   
   VLC_API uint64_t vlc_stream_Tell(const stream_t *) VLC_USED;
   
   VLC_API bool vlc_stream_Eof(const stream_t *) VLC_USED;
   
   VLC_API int vlc_stream_Seek(stream_t *, uint64_t offset) VLC_USED;
   
   VLC_API int vlc_stream_vaControl(stream_t *s, int query, va_list args);
   
   static inline int vlc_stream_Control(stream_t *s, int query, ...)
   {
       va_list ap;
       int ret;
   
       va_start(ap, query);
       ret = vlc_stream_vaControl(s, query, ap);
       va_end(ap);
       return ret;
   }
   
   VLC_API block_t *vlc_stream_Block(stream_t *s, size_t);
   VLC_API char *vlc_stream_ReadLine(stream_t *);
   VLC_API int vlc_stream_ReadDir(stream_t *, input_item_node_t *);
   
   VLC_API void vlc_stream_Delete(stream_t *s);
   
   VLC_API stream_t *vlc_stream_CommonNew(vlc_object_t *, void (*)(stream_t *));
   
   VLC_USED static inline int vlc_stream_GetSize( stream_t *s, uint64_t *size )
   {
       return vlc_stream_Control( s, STREAM_GET_SIZE, size );
   }
   
   static inline int64_t stream_Size( stream_t *s )
   {
       uint64_t i_pos;
   
       if( vlc_stream_GetSize( s, &i_pos ) )
           return 0;
       if( i_pos >> 62 )
           return (int64_t)1 << 62;
       return i_pos;
   }
   
   VLC_USED
   static inline bool stream_HasExtension( stream_t *s, const char *extension )
   {
       const char *name = (s->psz_filepath != NULL) ? s->psz_filepath
                                                    : s->psz_url;
       const char *ext = strrchr( name, '.' );
       return ext != NULL && !strcasecmp( ext, extension );
   }
   
   static inline char *stream_ContentType( stream_t *s )
   {
       char *res;
       if( vlc_stream_Control( s, STREAM_GET_CONTENT_TYPE, &res ) )
           return NULL;
       return res;
   }
   
   VLC_USED
   static inline char *stream_MimeType( stream_t *s )
   {
       char* mime_type = stream_ContentType( s );
   
       if( mime_type ) /* strip parameters */
           mime_type[strcspn( mime_type, " ;" )] = '\0';
   
       return mime_type;
   }
   
   VLC_USED
   static inline bool stream_IsMimeType(stream_t *s, const char *type)
   {
       char *mime = stream_MimeType(s);
       if (mime == NULL)
           return false;
   
       bool ok = !strcasecmp(mime, type);
       free(mime);
       return ok;
   }
   
   VLC_API stream_t *vlc_stream_MemoryNew(vlc_object_t *obj, uint8_t *base,
                                          size_t size, bool preserve) VLC_USED;
   #define vlc_stream_MemoryNew(a, b, c, d) \
           vlc_stream_MemoryNew(VLC_OBJECT(a), b, c, d)
   
   VLC_API stream_t * vlc_stream_NewURL(vlc_object_t *obj, const char *url)
   VLC_USED;
   #define vlc_stream_NewURL(a, b) vlc_stream_NewURL(VLC_OBJECT(a), b)
   
   
   VLC_API stream_t *vlc_stream_fifo_New(vlc_object_t *parent);
   
   VLC_API int vlc_stream_fifo_Queue(stream_t *s, block_t *block);
   
   VLC_API ssize_t vlc_stream_fifo_Write(stream_t *s, const void *buf,
                                         size_t len);
   
   VLC_API void vlc_stream_fifo_Close(stream_t *s);
   
   
   VLC_API stream_t* vlc_stream_FilterNew( stream_t *p_source, const char *psz_stream_filter );
   
   VLC_API int vlc_stream_FilterDefaultReadDir(stream_t *s,
                                               input_item_node_t *p_node);
   
   #define stream_FilterSetDefaultReadDir(stream) \
   do { \
       (stream)->pf_readdir = vlc_stream_FilterDefaultReadDir; \
   } while (0)
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif
