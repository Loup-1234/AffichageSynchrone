
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_library.h:

Program Listing for File libvlc_media_library.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_library.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc_media_library.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc_media_library.h:  libvlc external API
    *****************************************************************************
    * Copyright (C) 1998-2009 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Clément Stenac <zorglub@videolan.org>
    *          Jean-Paul Saman <jpsaman@videolan.org>
    *          Pierre d'Herbemont <pdherbemont@videolan.org>
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
   
   #ifndef VLC_LIBVLC_MEDIA_LIBRARY_H
   #define VLC_LIBVLC_MEDIA_LIBRARY_H 1
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   typedef struct libvlc_media_library_t libvlc_media_library_t;
   
   LIBVLC_API libvlc_media_library_t *
       libvlc_media_library_new( libvlc_instance_t * p_instance );
   
   LIBVLC_API void
       libvlc_media_library_release( libvlc_media_library_t * p_mlib );
   
   LIBVLC_API void
       libvlc_media_library_retain( libvlc_media_library_t * p_mlib );
   
   LIBVLC_API int
       libvlc_media_library_load( libvlc_media_library_t * p_mlib );
   
   LIBVLC_API libvlc_media_list_t *
       libvlc_media_library_media_list( libvlc_media_library_t * p_mlib );
   
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif /* VLC_LIBVLC_MEDIA_LIBRARY_H */
