
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_input.h:

Program Listing for File vlc_input.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_input.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_input.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_input.h: Core input structures
    *****************************************************************************
    * Copyright (C) 1999-2015 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Christophe Massiot <massiot@via.ecp.fr>
    *          Laurent Aimar <fenrir@via.ecp.fr>
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
   
   #ifndef VLC_INPUT_H
   #define VLC_INPUT_H 1
   
   
   #include <vlc_es.h>
   #include <vlc_meta.h>
   #include <vlc_epg.h>
   #include <vlc_events.h>
   #include <vlc_input_item.h>
   #include <vlc_vout.h>
   #include <vlc_vout_osd.h>
   
   #include <string.h>
   
   /*****************************************************************************
    * Seek point: (generalisation of chapters)
    *****************************************************************************/
   struct seekpoint_t
   {
       int64_t i_time_offset;
       char    *psz_name;
   };
   
   static inline seekpoint_t *vlc_seekpoint_New( void )
   {
       seekpoint_t *point = (seekpoint_t*)malloc( sizeof( seekpoint_t ) );
       if( !point )
           return NULL;
       point->i_time_offset = -1;
       point->psz_name = NULL;
       return point;
   }
   
   static inline void vlc_seekpoint_Delete( seekpoint_t *point )
   {
       if( !point ) return;
       free( point->psz_name );
       free( point );
   }
   
   static inline seekpoint_t *vlc_seekpoint_Duplicate( const seekpoint_t *src )
   {
       seekpoint_t *point = vlc_seekpoint_New();
       if( likely(point) )
       {
           if( src->psz_name ) point->psz_name = strdup( src->psz_name );
           point->i_time_offset = src->i_time_offset;
       }
       return point;
   }
   
   /*****************************************************************************
    * Title:
    *****************************************************************************/
   
   /* input_title_t.i_flags field */
   #define INPUT_TITLE_MENU         0x01   /* Menu title */
   #define INPUT_TITLE_INTERACTIVE  0x02   /* Interactive title. Playback position has no meaning. */
   
   typedef struct input_title_t
   {
       char        *psz_name;
   
       int64_t     i_length;   /* Length(microsecond) if known, else 0 */
   
       unsigned    i_flags;    /* Is it a menu or a normal entry */
   
       /* Title seekpoint */
       int         i_seekpoint;
       seekpoint_t **seekpoint;
   } input_title_t;
   
   static inline input_title_t *vlc_input_title_New(void)
   {
       input_title_t *t = (input_title_t*)malloc( sizeof( input_title_t ) );
       if( !t )
           return NULL;
   
       t->psz_name = NULL;
       t->i_flags = 0;
       t->i_length = 0;
       t->i_seekpoint = 0;
       t->seekpoint = NULL;
   
       return t;
   }
   
   static inline void vlc_input_title_Delete( input_title_t *t )
   {
       int i;
       if( t == NULL )
           return;
   
       free( t->psz_name );
       for( i = 0; i < t->i_seekpoint; i++ )
           vlc_seekpoint_Delete( t->seekpoint[i] );
       free( t->seekpoint );
       free( t );
   }
   
   static inline input_title_t *vlc_input_title_Duplicate( const input_title_t *t )
   {
       input_title_t *dup = vlc_input_title_New( );
       if( dup == NULL) return NULL;
   
       if( t->psz_name ) dup->psz_name = strdup( t->psz_name );
       dup->i_flags     = t->i_flags;
       dup->i_length    = t->i_length;
       if( t->i_seekpoint > 0 )
       {
           dup->seekpoint = (seekpoint_t**)vlc_alloc( t->i_seekpoint, sizeof(seekpoint_t*) );
           if( likely(dup->seekpoint) )
           {
               for( int i = 0; i < t->i_seekpoint; i++ )
                   dup->seekpoint[i] = vlc_seekpoint_Duplicate( t->seekpoint[i] );
               dup->i_seekpoint = t->i_seekpoint;
           }
       }
   
       return dup;
   }
   
   /*****************************************************************************
    * Attachments
    *****************************************************************************/
   struct input_attachment_t
   {
       char *psz_name;
       char *psz_mime;
       char *psz_description;
   
       size_t i_data;
       void *p_data;
   };
   
   static inline void vlc_input_attachment_Delete( input_attachment_t *a )
   {
       if( !a )
           return;
   
       free( a->p_data );
       free( a->psz_description );
       free( a->psz_mime );
       free( a->psz_name );
       free( a );
   }
   
   static inline input_attachment_t *vlc_input_attachment_New( const char *psz_name,
                                                               const char *psz_mime,
                                                               const char *psz_description,
                                                               const void *p_data,
                                                               size_t i_data )
   {
       input_attachment_t *a = (input_attachment_t *)malloc( sizeof (*a) );
       if( unlikely(a == NULL) )
           return NULL;
   
       a->psz_name = strdup( psz_name ? psz_name : "" );
       a->psz_mime = strdup( psz_mime ? psz_mime : "" );
       a->psz_description = strdup( psz_description ? psz_description : "" );
       a->i_data = i_data;
       a->p_data = malloc( i_data );
       if( i_data > 0 && likely(a->p_data != NULL) )
           memcpy( a->p_data, p_data, i_data );
   
       if( unlikely(a->psz_name == NULL || a->psz_mime == NULL
                 || a->psz_description == NULL || (i_data > 0 && a->p_data == NULL)) )
       {
           vlc_input_attachment_Delete( a );
           a = NULL;
       }
       return a;
   }
   
   static inline input_attachment_t *vlc_input_attachment_Duplicate( const input_attachment_t *a )
   {
       return vlc_input_attachment_New( a->psz_name, a->psz_mime, a->psz_description,
                                        a->p_data, a->i_data );
   }
   
   /*****************************************************************************
    * input defines/constants.
    *****************************************************************************/
   
   typedef struct input_resource_t input_resource_t;
   
   struct input_thread_t
   {
       VLC_COMMON_MEMBERS
   };
   
   #define INPUT_RECORD_PREFIX "vlc-record-%Y-%m-%d-%Hh%Mm%Ss-$ N-$ p"
   
   /*****************************************************************************
    * Input events and variables
    *****************************************************************************/
   
   
   typedef enum input_state_e
   {
       INIT_S = 0,
       OPENING_S,
       PLAYING_S,
       PAUSE_S,
       END_S,
       ERROR_S,
   } input_state_e;
   
   
   #define INPUT_RATE_DEFAULT  1000
   #define INPUT_RATE_MIN        32            /* Up to 32/1 */
   #define INPUT_RATE_MAX     32000            /* Up to 1/32 */
   
   typedef enum input_event_type_e
   {
       /* "state" has changed */
       INPUT_EVENT_STATE,
       /* b_dead is true */
       INPUT_EVENT_DEAD,
   
       /* "rate" has changed */
       INPUT_EVENT_RATE,
   
       /* At least one of "position" or "time" */
       INPUT_EVENT_POSITION,
   
       /* "length" has changed */
       INPUT_EVENT_LENGTH,
   
       /* A title has been added or removed or selected.
        * It implies that the chapter has changed (no chapter event is sent) */
       INPUT_EVENT_TITLE,
       /* A chapter has been added or removed or selected. */
       INPUT_EVENT_CHAPTER,
   
       /* A program ("program") has been added or removed or selected,
        * or "program-scrambled" has changed.*/
       INPUT_EVENT_PROGRAM,
       /* A ES has been added or removed or selected */
       INPUT_EVENT_ES,
       /* "teletext-es" has changed */
       INPUT_EVENT_TELETEXT,
   
       /* "record" has changed */
       INPUT_EVENT_RECORD,
   
       /* input_item_t media has changed */
       INPUT_EVENT_ITEM_META,
       /* input_item_t info has changed */
       INPUT_EVENT_ITEM_INFO,
       /* input_item_t epg has changed */
       INPUT_EVENT_ITEM_EPG,
   
       /* Input statistics have been updated */
       INPUT_EVENT_STATISTICS,
       /* At least one of "signal-quality" or "signal-strength" has changed */
       INPUT_EVENT_SIGNAL,
   
       /* "audio-delay" has changed */
       INPUT_EVENT_AUDIO_DELAY,
       /* "spu-delay" has changed */
       INPUT_EVENT_SUBTITLE_DELAY,
   
       /* "bookmark" has changed */
       INPUT_EVENT_BOOKMARK,
   
       /* cache" has changed */
       INPUT_EVENT_CACHE,
   
       /* A audio_output_t object has been created/deleted by *the input* */
       INPUT_EVENT_AOUT,
       /* A vout_thread_t object has been created/deleted by *the input* */
       INPUT_EVENT_VOUT,
   
   } input_event_type_e;
   
   enum input_query_e
   {
       /* input variable "position" */
       INPUT_GET_POSITION,         /* arg1= double *       res=    */
       INPUT_SET_POSITION,         /* arg1= double         res=can fail    */
   
       /* input variable "length" */
       INPUT_GET_LENGTH,           /* arg1= int64_t *      res=can fail    */
   
       /* input variable "time" */
       INPUT_GET_TIME,             /* arg1= int64_t *      res=    */
       INPUT_SET_TIME,             /* arg1= int64_t        res=can fail    */
   
       /* input variable "rate" (nominal is INPUT_RATE_DEFAULT) */
       INPUT_GET_RATE,             /* arg1= int *          res=    */
       INPUT_SET_RATE,             /* arg1= int            res=can fail    */
   
       /* input variable "state" */
       INPUT_GET_STATE,            /* arg1= int *          res=    */
       INPUT_SET_STATE,            /* arg1= int            res=can fail    */
   
       /* input variable "audio-delay" and "sub-delay" */
       INPUT_GET_AUDIO_DELAY,      /* arg1 = int* res=can fail */
       INPUT_SET_AUDIO_DELAY,      /* arg1 = int  res=can fail */
       INPUT_GET_SPU_DELAY,        /* arg1 = int* res=can fail */
       INPUT_SET_SPU_DELAY,        /* arg1 = int  res=can fail */
   
       /* Menu (VCD/DVD/BD) Navigation */
       INPUT_NAV_ACTIVATE,
       INPUT_NAV_UP,
       INPUT_NAV_DOWN,
       INPUT_NAV_LEFT,
       INPUT_NAV_RIGHT,
       INPUT_NAV_POPUP,
       INPUT_NAV_MENU,
   
       /* Meta datas */
       INPUT_ADD_INFO,   /* arg1= char* arg2= char* arg3=...     res=can fail */
       INPUT_REPLACE_INFOS,/* arg1= info_category_t *            res=cannot fail */
       INPUT_MERGE_INFOS,/* arg1= info_category_t *              res=cannot fail */
       INPUT_DEL_INFO,   /* arg1= char* arg2= char*              res=can fail */
   
       /* bookmarks */
       INPUT_GET_BOOKMARK,    /* arg1= seekpoint_t *               res=can fail */
       INPUT_GET_BOOKMARKS,   /* arg1= seekpoint_t *** arg2= int * res=can fail */
       INPUT_CLEAR_BOOKMARKS, /* res=can fail */
       INPUT_ADD_BOOKMARK,    /* arg1= seekpoint_t *  res=can fail   */
       INPUT_CHANGE_BOOKMARK, /* arg1= seekpoint_t * arg2= int * res=can fail   */
       INPUT_DEL_BOOKMARK,    /* arg1= seekpoint_t *  res=can fail   */
       INPUT_SET_BOOKMARK,    /* arg1= int  res=can fail    */
   
       /* titles */
       INPUT_GET_TITLE_INFO,     /* arg1=input_title_t** arg2= int * res=can fail */
       INPUT_GET_FULL_TITLE_INFO,     /* arg1=input_title_t*** arg2= int * res=can fail */
   
       /* seekpoints */
       INPUT_GET_SEEKPOINTS,  /* arg1=seekpoint_t*** arg2= int * res=can fail */
   
       /* Attachments */
       INPUT_GET_ATTACHMENTS, /* arg1=input_attachment_t***, arg2=int*  res=can fail */
       INPUT_GET_ATTACHMENT,  /* arg1=input_attachment_t**, arg2=char*  res=can fail */
   
       /* On the fly input slave */
       INPUT_ADD_SLAVE,       /* arg1= enum slave_type, arg2= const char *,
                               * arg3= bool forced, arg4= bool notify,
                               * arg5= bool check_extension */
   
       /* On the fly record while playing */
       INPUT_SET_RECORD_STATE, /* arg1=bool    res=can fail */
       INPUT_GET_RECORD_STATE, /* arg1=bool*   res=can fail */
   
       /* ES */
       INPUT_RESTART_ES,       /* arg1=int (-AUDIO/VIDEO/SPU_ES for the whole category) */
   
       /* Viewpoint */
       INPUT_UPDATE_VIEWPOINT, /* arg1=(const vlc_viewpoint_t*), arg2=bool b_absolute */
       INPUT_SET_INITIAL_VIEWPOINT, /* arg1=(const vlc_viewpoint_t*) */
   
       /* Input ressources
        * XXX You must call vlc_object_release as soon as possible */
       INPUT_GET_AOUT,         /* arg1=audio_output_t **              res=can fail */
       INPUT_GET_VOUTS,        /* arg1=vout_thread_t ***, size_t *        res=can fail */
       INPUT_GET_ES_OBJECTS,   /* arg1=int id, vlc_object_t **dec, vout_thread_t **, audio_output_t ** */
   
       /* Renderers */
       INPUT_SET_RENDERER,     /* arg1=vlc_renderer_item_t* */
   
       /* External clock managments */
       INPUT_GET_PCR_SYSTEM,   /* arg1=vlc_tick_t *, arg2=vlc_tick_t *       res=can fail */
       INPUT_MODIFY_PCR_SYSTEM,/* arg1=int absolute, arg2=vlc_tick_t   res=can fail */
   };
   
   
   /*****************************************************************************
    * Prototypes
    *****************************************************************************/
   
   VLC_API input_thread_t * input_Create( vlc_object_t *p_parent, input_item_t *,
                                          const char *psz_log, input_resource_t *,
                                          vlc_renderer_item_t* p_renderer ) VLC_USED;
   #define input_Create(a,b,c,d,e) input_Create(VLC_OBJECT(a),b,c,d,e)
   
   VLC_API int input_Start( input_thread_t * );
   
   VLC_API void input_Stop( input_thread_t * );
   
   VLC_API int input_Read( vlc_object_t *, input_item_t * );
   #define input_Read(a,b) input_Read(VLC_OBJECT(a),b)
   
   VLC_API int input_vaControl( input_thread_t *, int i_query, va_list  );
   
   VLC_API int input_Control( input_thread_t *, int i_query, ...  );
   
   VLC_API void input_Close( input_thread_t * );
   
   static inline
   input_thread_t *input_CreateAndStart( vlc_object_t *parent,
                                         input_item_t *item, const char *log )
   {
       input_thread_t *input = input_Create( parent, item, log, NULL, NULL );
       if( input != NULL && input_Start( input ) )
       {
           vlc_object_release( input );
           input = NULL;
       }
       return input;
   }
   #define input_CreateAndStart(a,b,c) input_CreateAndStart(VLC_OBJECT(a),b,c)
   
   VLC_API input_item_t* input_GetItem( input_thread_t * ) VLC_USED;
   
   static inline input_state_e input_GetState( input_thread_t * p_input )
   {
       input_state_e state = INIT_S;
       input_Control( p_input, INPUT_GET_STATE, &state );
       return state;
   }
   
   static inline vout_thread_t *input_GetVout( input_thread_t *p_input )
   {
        vout_thread_t **pp_vout, *p_vout;
        size_t i_vout;
   
        if( input_Control( p_input, INPUT_GET_VOUTS, &pp_vout, &i_vout ) )
            return NULL;
   
        for( size_t i = 1; i < i_vout; i++ )
            vlc_object_release( (vlc_object_t *)(pp_vout[i]) );
   
        p_vout = (i_vout >= 1) ? pp_vout[0] : NULL;
        free( pp_vout );
        return p_vout;
   }
   
   static inline int input_AddSlave( input_thread_t *p_input, enum slave_type type,
                                     const char *psz_uri, bool b_forced,
                                     bool b_notify, bool b_check_ext )
   {
       return input_Control( p_input, INPUT_ADD_SLAVE, type, psz_uri, b_forced,
                             b_notify, b_check_ext );
   }
   
   static inline int input_UpdateViewpoint( input_thread_t *p_input,
                                            const vlc_viewpoint_t *p_viewpoint,
                                            bool b_absolute )
   {
       return input_Control( p_input, INPUT_UPDATE_VIEWPOINT, p_viewpoint,
                             b_absolute );
   }
   
   static inline audio_output_t *input_GetAout( input_thread_t *p_input )
   {
        audio_output_t *p_aout;
        return input_Control( p_input, INPUT_GET_AOUT, &p_aout ) ? NULL : p_aout;
   }
   
   static inline int input_GetEsObjects( input_thread_t *p_input, int i_id,
                                         vlc_object_t **pp_decoder,
                                         vout_thread_t **pp_vout, audio_output_t **pp_aout )
   {
       return input_Control( p_input, INPUT_GET_ES_OBJECTS, i_id,
                             pp_decoder, pp_vout, pp_aout );
   }
   
   static inline int input_GetPcrSystem( input_thread_t *p_input, vlc_tick_t *pi_system, vlc_tick_t *pi_delay )
   {
       return input_Control( p_input, INPUT_GET_PCR_SYSTEM, pi_system, pi_delay );
   }
   static inline int input_ModifyPcrSystem( input_thread_t *p_input, bool b_absolute, vlc_tick_t i_system )
   {
       return input_Control( p_input, INPUT_MODIFY_PCR_SYSTEM, b_absolute, i_system );
   }
   
   /* */
   VLC_API decoder_t * input_DecoderCreate( vlc_object_t *, const es_format_t *, input_resource_t * ) VLC_USED;
   VLC_API void input_DecoderDelete( decoder_t * );
   VLC_API void input_DecoderDecode( decoder_t *, block_t *, bool b_do_pace );
   VLC_API void input_DecoderDrain( decoder_t * );
   VLC_API void input_DecoderFlush( decoder_t * );
   
   VLC_API char * input_CreateFilename( input_thread_t *, const char *psz_path, const char *psz_prefix, const char *psz_extension ) VLC_USED;
   
   VLC_API input_resource_t * input_resource_New( vlc_object_t * ) VLC_USED;
   
   VLC_API void input_resource_Release( input_resource_t * );
   
   VLC_API void input_resource_TerminateVout( input_resource_t * );
   
   VLC_API void input_resource_Terminate( input_resource_t * );
   
   VLC_API audio_output_t *input_resource_HoldAout( input_resource_t * );
   
   VLC_API audio_output_t *input_resource_GetAout( input_resource_t * );
   
   VLC_API void input_resource_PutAout( input_resource_t *, audio_output_t * );
   
   VLC_API void input_resource_ResetAout( input_resource_t * );
   
   #endif
