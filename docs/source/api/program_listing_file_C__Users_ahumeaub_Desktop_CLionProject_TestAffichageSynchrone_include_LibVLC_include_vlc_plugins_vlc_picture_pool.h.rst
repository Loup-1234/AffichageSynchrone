
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_picture_pool.h:

Program Listing for File vlc_picture_pool.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_picture_pool.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_picture_pool.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_picture_pool.h: picture pool definitions
    *****************************************************************************
    * Copyright (C) 2009 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Laurent Aimar <fenrir _AT_ videolan _DOT_ org>
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
   
   #ifndef VLC_PICTURE_POOL_H
   #define VLC_PICTURE_POOL_H 1
   
   
   #include <vlc_picture.h>
   
   typedef struct picture_pool_t picture_pool_t;
   
   typedef struct {
       unsigned  picture_count;
       picture_t *const *picture;
   
       int       (*lock)(picture_t *);
       void      (*unlock)(picture_t *);
   } picture_pool_configuration_t;
   
   VLC_API picture_pool_t * picture_pool_NewExtended( const picture_pool_configuration_t * ) VLC_USED;
   
   VLC_API picture_pool_t * picture_pool_New(unsigned count,
                                             picture_t *const *tab) VLC_USED;
   
   VLC_API picture_pool_t * picture_pool_NewFromFormat(const video_format_t *fmt,
                                                       unsigned count) VLC_USED;
   
   VLC_API void picture_pool_Release( picture_pool_t * );
   
   VLC_API picture_t * picture_pool_Get( picture_pool_t * ) VLC_USED;
   
   VLC_API picture_t *picture_pool_Wait(picture_pool_t *) VLC_USED;
   
   VLC_API void picture_pool_Enum( picture_pool_t *,
                                   void (*cb)(void *, picture_t *), void *data );
   
   void picture_pool_Cancel( picture_pool_t *, bool canceled );
   
   bool picture_pool_OwnsPic( picture_pool_t *, picture_t *);
   
   VLC_API picture_pool_t * picture_pool_Reserve(picture_pool_t *, unsigned count)
   VLC_USED;
   
   VLC_API unsigned picture_pool_GetSize(const picture_pool_t *);
   
   
   #endif /* VLC_PICTURE_POOL_H */
   
