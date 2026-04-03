
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_discoverer.h:

Program Listing for File libvlc_media_discoverer.h
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_discoverer.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc_media_discoverer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc_media_discoverer.h:  libvlc external API
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
   
   #ifndef VLC_LIBVLC_MEDIA_DISCOVERER_H
   #define VLC_LIBVLC_MEDIA_DISCOVERER_H 1
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   typedef enum libvlc_media_discoverer_category_t {
       libvlc_media_discoverer_devices,
       libvlc_media_discoverer_lan,
       libvlc_media_discoverer_podcasts,
       libvlc_media_discoverer_localdirs,
   } libvlc_media_discoverer_category_t;
   
   typedef struct libvlc_media_discoverer_description_t {
       char *psz_name;
       char *psz_longname;
       libvlc_media_discoverer_category_t i_cat;
   } libvlc_media_discoverer_description_t;
   
   
   typedef struct libvlc_media_discoverer_t libvlc_media_discoverer_t;
   
   LIBVLC_API libvlc_media_discoverer_t *
   libvlc_media_discoverer_new( libvlc_instance_t * p_inst,
                                const char * psz_name );
   
   LIBVLC_API int
   libvlc_media_discoverer_start( libvlc_media_discoverer_t * p_mdis );
   
   LIBVLC_API void
   libvlc_media_discoverer_stop( libvlc_media_discoverer_t * p_mdis );
   
   LIBVLC_API void
   libvlc_media_discoverer_release( libvlc_media_discoverer_t * p_mdis );
   
   LIBVLC_API libvlc_media_list_t *
   libvlc_media_discoverer_media_list( libvlc_media_discoverer_t * p_mdis );
   
   LIBVLC_API int
   libvlc_media_discoverer_is_running( libvlc_media_discoverer_t * p_mdis );
   
   LIBVLC_API size_t
   libvlc_media_discoverer_list_get( libvlc_instance_t *p_inst,
                                     libvlc_media_discoverer_category_t i_cat,
                                     libvlc_media_discoverer_description_t ***ppp_services );
   
   LIBVLC_API void
   libvlc_media_discoverer_list_release( libvlc_media_discoverer_description_t **pp_services,
                                         size_t i_count );
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif /* <vlc/libvlc.h> */
