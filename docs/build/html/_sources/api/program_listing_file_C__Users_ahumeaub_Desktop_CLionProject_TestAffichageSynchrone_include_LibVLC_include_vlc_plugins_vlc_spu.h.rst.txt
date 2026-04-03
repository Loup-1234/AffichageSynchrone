
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_spu.h:

Program Listing for File vlc_spu.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_spu.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_spu.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_spu.h: spu_t definition and functions.
    *****************************************************************************
    * Copyright (C) 1999-2010 VLC authors and VideoLAN
    * Copyright (C) 2010 Laurent Aimar
    * $Id$
    *
    * Authors: Gildas Bazin <gbazin@videolan.org>
    *          Laurent Aimar <fenrir _AT_ videolan _DOT_ org>
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
   
   #ifndef VLC_SPU_H
   #define VLC_SPU_H 1
   
   #include <vlc_subpicture.h>
   
   #ifdef __cplusplus
   extern "C" {
   #endif
   
   
   typedef struct spu_private_t spu_private_t;
   
   struct spu_t
   {
       VLC_COMMON_MEMBERS
   
       spu_private_t *p;
   };
   
       VLC_API spu_t * spu_Create( vlc_object_t *, vout_thread_t * );
   #define spu_Create(a,b) spu_Create(VLC_OBJECT(a),b)
   VLC_API void spu_Destroy( spu_t * );
   
   VLC_API void spu_PutSubpicture( spu_t *, subpicture_t * );
   
   VLC_API subpicture_t * spu_Render( spu_t *, const vlc_fourcc_t *p_chroma_list, const video_format_t *p_fmt_dst, const video_format_t *p_fmt_src, vlc_tick_t render_subtitle_date, vlc_tick_t render_osd_date, bool ignore_osd );
   
   VLC_API int spu_RegisterChannel( spu_t * );
   
   VLC_API void spu_ClearChannel( spu_t *, int );
   
   VLC_API void spu_ChangeSources( spu_t *, const char * );
   
   VLC_API void spu_ChangeFilters( spu_t *, const char * );
   
   
   #ifdef __cplusplus
   }
   #endif
   
   #endif /* VLC_SPU_H */
   
