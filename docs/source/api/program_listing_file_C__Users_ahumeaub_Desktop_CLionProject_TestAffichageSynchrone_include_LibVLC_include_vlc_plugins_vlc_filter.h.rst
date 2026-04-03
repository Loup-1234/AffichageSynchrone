
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_filter.h:

Program Listing for File vlc_filter.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_filter.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_filter.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_filter.h: filter related structures and functions
    *****************************************************************************
    * Copyright (C) 1999-2014 VLC authors and VideoLAN
    *
    * Authors: Gildas Bazin <gbazin@videolan.org>
    *          Antoine Cellerier <dionoea at videolan dot org>
    *          Rémi Denis-Courmont
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
   
   #ifndef VLC_FILTER_H
   #define VLC_FILTER_H 1
   
   #include <vlc_es.h>
   
   
   typedef struct filter_owner_sys_t filter_owner_sys_t;
   
   typedef struct filter_owner_t
   {
       void *sys;
   
       union
       {
           struct
           {
               picture_t * (*buffer_new)( filter_t * );
           } video;
           struct
           {
               subpicture_t * (*buffer_new)( filter_t * );
           } sub;
       };
   } filter_owner_t;
   
   struct vlc_mouse_t;
   
   struct filter_t
   {
       VLC_COMMON_MEMBERS
   
       /* Module properties */
       module_t *          p_module;
       filter_sys_t *      p_sys;
   
       /* Input format */
       es_format_t         fmt_in;
   
       /* Output format of filter */
       es_format_t         fmt_out;
       bool                b_allow_fmt_out_change;
   
       /* Name of the "video filter" shortcut that is requested, can be NULL */
       const char *        psz_name;
       /* Filter configuration */
       config_chain_t *    p_cfg;
   
       union
       {
           picture_t * (*pf_video_filter)( filter_t *, picture_t * );
   
           block_t * (*pf_audio_filter)( filter_t *, block_t * );
   
           void (*pf_video_blend)( filter_t *,  picture_t *, const picture_t *,
                                    int, int, int );
   
           subpicture_t *(*pf_sub_source)( filter_t *, vlc_tick_t );
   
           subpicture_t *(*pf_sub_filter)( filter_t *, subpicture_t * );
   
           int (*pf_render)( filter_t *, subpicture_region_t *,
                             subpicture_region_t *, const vlc_fourcc_t * );
       };
   
       union
       {
           /* TODO: video filter drain */
           block_t *(*pf_audio_drain) ( filter_t * );
       };
   
       void (*pf_flush)( filter_t * );
   
       void (*pf_change_viewpoint)( filter_t *, const vlc_viewpoint_t * );
   
       union
       {
           int (*pf_video_mouse)( filter_t *, struct vlc_mouse_t *,
                                  const struct vlc_mouse_t *p_old,
                                  const struct vlc_mouse_t *p_new );
           int (*pf_sub_mouse)( filter_t *, const struct vlc_mouse_t *p_old,
                                const struct vlc_mouse_t *p_new,
                                const video_format_t * );
       };
   
       /* Input attachments
        * XXX use filter_GetInputAttachments */
       int (*pf_get_attachments)( filter_t *, input_attachment_t ***, int * );
   
       /* Private structure for the owner of the decoder */
       filter_owner_t      owner;
   };
   
   static inline picture_t *filter_NewPicture( filter_t *p_filter )
   {
       picture_t *pic = p_filter->owner.video.buffer_new( p_filter );
       if( pic == NULL )
           msg_Warn( p_filter, "can't get output picture" );
       return pic;
   }
   
   static inline void filter_Flush( filter_t *p_filter )
   {
       if( p_filter->pf_flush != NULL )
           p_filter->pf_flush( p_filter );
   }
   
   static inline void filter_ChangeViewpoint( filter_t *p_filter,
                                              const vlc_viewpoint_t *vp)
   {
       if( p_filter->pf_change_viewpoint != NULL )
           p_filter->pf_change_viewpoint( p_filter, vp );
   }
   
   static inline block_t *filter_DrainAudio( filter_t *p_filter )
   {
       if( p_filter->pf_audio_drain )
           return p_filter->pf_audio_drain( p_filter );
       else
           return NULL;
   }
   
   static inline subpicture_t *filter_NewSubpicture( filter_t *p_filter )
   {
       subpicture_t *subpic = p_filter->owner.sub.buffer_new( p_filter );
       if( subpic == NULL )
           msg_Warn( p_filter, "can't get output subpicture" );
       return subpic;
   }
   
   static inline int filter_GetInputAttachments( filter_t *p_filter,
                                                 input_attachment_t ***ppp_attachment,
                                                 int *pi_attachment )
   {
       if( !p_filter->pf_get_attachments )
           return VLC_EGENERIC;
       return p_filter->pf_get_attachments( p_filter,
                                            ppp_attachment, pi_attachment );
   }
   
   VLC_API void filter_AddProxyCallbacks( vlc_object_t *obj, filter_t *filter,
                                          vlc_callback_t restart_cb );
   # define filter_AddProxyCallbacks(a, b, c) \
       filter_AddProxyCallbacks(VLC_OBJECT(a), b, c)
   
   VLC_API void filter_DelProxyCallbacks( vlc_object_t *obj, filter_t *filter,
                                          vlc_callback_t restart_cb);
   # define filter_DelProxyCallbacks(a, b, c) \
       filter_DelProxyCallbacks(VLC_OBJECT(a), b, c)
   
   VLC_API filter_t * filter_NewBlend( vlc_object_t *, const video_format_t *p_dst_chroma ) VLC_USED;
   
   VLC_API int filter_ConfigureBlend( filter_t *, int i_dst_width, int i_dst_height, const video_format_t *p_src );
   
   VLC_API int filter_Blend( filter_t *, picture_t *p_dst, int i_dst_x, int i_dst_y, const picture_t *p_src, int i_alpha );
   
   VLC_API void filter_DeleteBlend( filter_t * );
   
   #define VIDEO_FILTER_WRAPPER( name )                                    \
       static picture_t *name ## _Filter ( filter_t *p_filter,             \
                                           picture_t *p_pic )              \
       {                                                                   \
           picture_t *p_outpic = filter_NewPicture( p_filter );            \
           if( p_outpic )                                                  \
           {                                                               \
               name( p_filter, p_pic, p_outpic );                          \
               picture_CopyProperties( p_outpic, p_pic );                  \
           }                                                               \
           picture_Release( p_pic );                                       \
           return p_outpic;                                                \
       }
   
   
   typedef struct filter_chain_t filter_chain_t;
   
   filter_chain_t * filter_chain_New( vlc_object_t *, const char *, enum es_format_category_e )
   VLC_USED;
   #define filter_chain_New( a, b, c ) filter_chain_New( VLC_OBJECT( a ), b, c )
   
   VLC_API filter_chain_t * filter_chain_NewVideo( vlc_object_t *obj, bool change,
                                                   const filter_owner_t *owner )
   VLC_USED;
   #define filter_chain_NewVideo( a, b, c ) \
           filter_chain_NewVideo( VLC_OBJECT( a ), b, c )
   
   VLC_API void filter_chain_Delete( filter_chain_t * );
   
   VLC_API void filter_chain_Reset( filter_chain_t *, const es_format_t *, const es_format_t * );
   
   VLC_API filter_t *filter_chain_AppendFilter(filter_chain_t *chain,
       const char *name, config_chain_t *cfg, const es_format_t *fmt_in,
       const es_format_t *fmt_out);
   
   VLC_API int filter_chain_AppendConverter(filter_chain_t *chain,
       const es_format_t *fmt_in, const es_format_t *fmt_out);
   
   VLC_API int filter_chain_AppendFromString(filter_chain_t *chain,
                                             const char *str);
   
   VLC_API void filter_chain_DeleteFilter(filter_chain_t *chain,
                                          filter_t *filter);
   
   VLC_API bool filter_chain_IsEmpty(const filter_chain_t *chain);
   
   VLC_API const es_format_t *filter_chain_GetFmtOut(filter_chain_t *chain);
   
   VLC_API picture_t *filter_chain_VideoFilter(filter_chain_t *chain,
                                               picture_t *pic);
   
   VLC_API void filter_chain_VideoFlush( filter_chain_t * );
   
   void filter_chain_SubSource(filter_chain_t *chain, spu_t *,
                               vlc_tick_t display_date);
   
   VLC_API subpicture_t *filter_chain_SubFilter(filter_chain_t *chain,
                                                subpicture_t *subpic);
   
   VLC_API int filter_chain_MouseFilter( filter_chain_t *, struct vlc_mouse_t *,
                                         const struct vlc_mouse_t * );
   
   VLC_API int filter_chain_MouseEvent( filter_chain_t *,
                                        const struct vlc_mouse_t *,
                                        const video_format_t * );
   
   int filter_chain_ForEach( filter_chain_t *chain,
                             int (*cb)( filter_t *, void * ), void *opaque );
   
   #endif /* _VLC_FILTER_H */
