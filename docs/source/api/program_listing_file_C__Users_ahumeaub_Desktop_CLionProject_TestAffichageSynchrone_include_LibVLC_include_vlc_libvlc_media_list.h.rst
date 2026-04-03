
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_list.h:

Program Listing for File libvlc_media_list.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_list.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc_media_list.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc_media_list.h:  libvlc_media_list API
    *****************************************************************************
    * Copyright (C) 1998-2008 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Pierre d'Herbemont
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
   
   #ifndef LIBVLC_MEDIA_LIST_H
   #define LIBVLC_MEDIA_LIST_H 1
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   typedef struct libvlc_media_list_t libvlc_media_list_t;
   
   LIBVLC_API libvlc_media_list_t *
       libvlc_media_list_new( libvlc_instance_t *p_instance );
   
   LIBVLC_API void
       libvlc_media_list_release( libvlc_media_list_t *p_ml );
   
   LIBVLC_API void
       libvlc_media_list_retain( libvlc_media_list_t *p_ml );
   
   LIBVLC_API void
   libvlc_media_list_set_media( libvlc_media_list_t *p_ml, libvlc_media_t *p_md );
   
   LIBVLC_API libvlc_media_t *
       libvlc_media_list_media( libvlc_media_list_t *p_ml );
   
   LIBVLC_API int
   libvlc_media_list_add_media( libvlc_media_list_t *p_ml, libvlc_media_t *p_md );
   
   LIBVLC_API int
   libvlc_media_list_insert_media( libvlc_media_list_t *p_ml,
                                   libvlc_media_t *p_md, int i_pos );
   
   LIBVLC_API int
   libvlc_media_list_remove_index( libvlc_media_list_t *p_ml, int i_pos );
   
   LIBVLC_API int
       libvlc_media_list_count( libvlc_media_list_t *p_ml );
   
   LIBVLC_API libvlc_media_t *
       libvlc_media_list_item_at_index( libvlc_media_list_t *p_ml, int i_pos );
   LIBVLC_API int
       libvlc_media_list_index_of_item( libvlc_media_list_t *p_ml,
                                        libvlc_media_t *p_md );
   
   LIBVLC_API int
       libvlc_media_list_is_readonly( libvlc_media_list_t * p_ml );
   
   LIBVLC_API void
       libvlc_media_list_lock( libvlc_media_list_t *p_ml );
   
   LIBVLC_API void
       libvlc_media_list_unlock( libvlc_media_list_t *p_ml );
   
   LIBVLC_API libvlc_event_manager_t *
       libvlc_media_list_event_manager( libvlc_media_list_t *p_ml );
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif /* _LIBVLC_MEDIA_LIST_H */
