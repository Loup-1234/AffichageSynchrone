
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_renderer_discoverer.h:

Program Listing for File libvlc_renderer_discoverer.h
=====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_renderer_discoverer.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc_renderer_discoverer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc_renderer_discoverer.h:  libvlc external API
    *****************************************************************************
    * Copyright © 2016 VLC authors and VideoLAN
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
   
   #ifndef VLC_LIBVLC_RENDERER_DISCOVERER_H
   #define VLC_LIBVLC_RENDERER_DISCOVERER_H 1
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   typedef struct libvlc_renderer_discoverer_t libvlc_renderer_discoverer_t;
   
   typedef struct libvlc_rd_description_t
   {
       char *psz_name;
       char *psz_longname;
   } libvlc_rd_description_t;
   
   #define LIBVLC_RENDERER_CAN_AUDIO 0x0001
   #define LIBVLC_RENDERER_CAN_VIDEO 0x0002
   
   typedef struct libvlc_renderer_item_t libvlc_renderer_item_t;
   
   
   LIBVLC_API libvlc_renderer_item_t *
   libvlc_renderer_item_hold(libvlc_renderer_item_t *p_item);
   
   LIBVLC_API void
   libvlc_renderer_item_release(libvlc_renderer_item_t *p_item);
   
   LIBVLC_API const char *
   libvlc_renderer_item_name(const libvlc_renderer_item_t *p_item);
   
   LIBVLC_API const char *
   libvlc_renderer_item_type(const libvlc_renderer_item_t *p_item);
   
   LIBVLC_API const char *
   libvlc_renderer_item_icon_uri(const libvlc_renderer_item_t *p_item);
   
   LIBVLC_API int
   libvlc_renderer_item_flags(const libvlc_renderer_item_t *p_item);
   
   LIBVLC_API libvlc_renderer_discoverer_t *
   libvlc_renderer_discoverer_new( libvlc_instance_t *p_inst,
                                   const char *psz_name );
   
   LIBVLC_API void
   libvlc_renderer_discoverer_release( libvlc_renderer_discoverer_t *p_rd );
   
   LIBVLC_API int
   libvlc_renderer_discoverer_start( libvlc_renderer_discoverer_t *p_rd );
   
   LIBVLC_API void
   libvlc_renderer_discoverer_stop( libvlc_renderer_discoverer_t *p_rd );
   
   LIBVLC_API libvlc_event_manager_t *
   libvlc_renderer_discoverer_event_manager( libvlc_renderer_discoverer_t *p_rd );
   
   LIBVLC_API size_t
   libvlc_renderer_discoverer_list_get( libvlc_instance_t *p_inst,
                                        libvlc_rd_description_t ***ppp_services );
   
   LIBVLC_API void
   libvlc_renderer_discoverer_list_release( libvlc_rd_description_t **pp_services,
                                            size_t i_count );
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif
