
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_vout.h:

Program Listing for File vlc_vout.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_vout.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_vout.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_vout.h: common video definitions
    *****************************************************************************
    * Copyright (C) 1999 - 2008 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Vincent Seguin <seguin@via.ecp.fr>
    *          Samuel Hocevar <sam@via.ecp.fr>
    *          Olivier Aubert <oaubert 47 videolan d07 org>
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
   
   #ifndef VLC_VOUT_H_
   #define VLC_VOUT_H_ 1
   
   #include <vlc_es.h>
   #include <vlc_picture.h>
   #include <vlc_subpicture.h>
   
   
   typedef struct {
       vout_thread_t        *vout;
       vlc_object_t         *input;
       bool                 change_fmt;
       const video_format_t *fmt;
       unsigned             dpb_size;
   } vout_configuration_t;
   
   typedef struct vout_thread_sys_t vout_thread_sys_t;
   
   struct vout_thread_t {
       VLC_COMMON_MEMBERS
   
       /* Private vout_thread data */
       vout_thread_sys_t *p;
   };
   
   /* Alignment flags */
   #define VOUT_ALIGN_LEFT         0x0001
   #define VOUT_ALIGN_RIGHT        0x0002
   #define VOUT_ALIGN_HMASK        0x0003
   #define VOUT_ALIGN_TOP          0x0004
   #define VOUT_ALIGN_BOTTOM       0x0008
   #define VOUT_ALIGN_VMASK        0x000C
   
   /*****************************************************************************
    * Prototypes
    *****************************************************************************/
   
   VLC_API vout_thread_t * vout_Request( vlc_object_t *object, const vout_configuration_t *cfg );
   #define vout_Request(a,b) vout_Request(VLC_OBJECT(a),b)
   
   VLC_API void vout_Close( vout_thread_t *p_vout );
   
   static inline void vout_CloseAndRelease( vout_thread_t *p_vout )
   {
       vout_Close( p_vout );
       vlc_object_release( p_vout );
   }
   
   VLC_API int vout_GetSnapshot( vout_thread_t *p_vout,
                                 block_t **pp_image, picture_t **pp_picture,
                                 video_format_t *p_fmt,
                                 const char *psz_format, vlc_tick_t i_timeout );
   
   VLC_API void vout_ChangeAspectRatio( vout_thread_t *p_vout,
                                        unsigned int i_num, unsigned int i_den );
   
   /* */
   VLC_API picture_t * vout_GetPicture( vout_thread_t * );
   VLC_API void vout_PutPicture( vout_thread_t *, picture_t * );
   
   /* Subpictures channels ID */
   #define VOUT_SPU_CHANNEL_INVALID      (-1) /* Always fails in comparison */
   #define VOUT_SPU_CHANNEL_OSD            1 /* OSD channel is automatically cleared */
   #define VOUT_SPU_CHANNEL_AVAIL_FIRST    8 /* Registerable channels from this offset */
   
   /* */
   VLC_API void vout_PutSubpicture( vout_thread_t *, subpicture_t * );
   VLC_API int vout_RegisterSubpictureChannel( vout_thread_t * );
   VLC_API void vout_FlushSubpictureChannel( vout_thread_t *, int );
   
   
   #endif /* _VLC_VIDEO_H */
