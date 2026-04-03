
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_list_player.h:

Program Listing for File libvlc_media_list_player.h
===================================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_media_list_player.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc_media_list_player.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc_media_list_player.h:  libvlc_media_list API
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
   
   #ifndef LIBVLC_MEDIA_LIST_PLAYER_H
   #define LIBVLC_MEDIA_LIST_PLAYER_H 1
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   typedef struct libvlc_media_list_player_t libvlc_media_list_player_t;
   
   typedef enum libvlc_playback_mode_t
   {
       libvlc_playback_mode_default,
       libvlc_playback_mode_loop,
       libvlc_playback_mode_repeat
   } libvlc_playback_mode_t;
   
   LIBVLC_API libvlc_media_list_player_t *
       libvlc_media_list_player_new( libvlc_instance_t * p_instance );
   
   LIBVLC_API void
       libvlc_media_list_player_release( libvlc_media_list_player_t * p_mlp );
   
   LIBVLC_API void
       libvlc_media_list_player_retain( libvlc_media_list_player_t *p_mlp );
   
   LIBVLC_API libvlc_event_manager_t *
       libvlc_media_list_player_event_manager(libvlc_media_list_player_t * p_mlp);
   
   LIBVLC_API void
       libvlc_media_list_player_set_media_player(
                                        libvlc_media_list_player_t * p_mlp,
                                        libvlc_media_player_t * p_mi );
   
   LIBVLC_API libvlc_media_player_t *
       libvlc_media_list_player_get_media_player(libvlc_media_list_player_t * p_mlp);
   
   LIBVLC_API void
       libvlc_media_list_player_set_media_list(
                                        libvlc_media_list_player_t * p_mlp,
                                        libvlc_media_list_t * p_mlist );
   
   LIBVLC_API
   void libvlc_media_list_player_play(libvlc_media_list_player_t * p_mlp);
   
   LIBVLC_API
   void libvlc_media_list_player_pause(libvlc_media_list_player_t * p_mlp);
   
   LIBVLC_API
   void libvlc_media_list_player_set_pause(libvlc_media_list_player_t * p_mlp,
                                           int do_pause);
   
   LIBVLC_API int
       libvlc_media_list_player_is_playing( libvlc_media_list_player_t * p_mlp );
   
   LIBVLC_API libvlc_state_t
       libvlc_media_list_player_get_state( libvlc_media_list_player_t * p_mlp );
   
   LIBVLC_API
   int libvlc_media_list_player_play_item_at_index(libvlc_media_list_player_t * p_mlp,
                                                   int i_index);
   
   LIBVLC_API
   int libvlc_media_list_player_play_item(libvlc_media_list_player_t * p_mlp,
                                          libvlc_media_t * p_md);
   
   LIBVLC_API void
       libvlc_media_list_player_stop( libvlc_media_list_player_t * p_mlp);
   
   LIBVLC_API
   int libvlc_media_list_player_next(libvlc_media_list_player_t * p_mlp);
   
   LIBVLC_API
   int libvlc_media_list_player_previous(libvlc_media_list_player_t * p_mlp);
   
   
   
   LIBVLC_API
   void libvlc_media_list_player_set_playback_mode(libvlc_media_list_player_t * p_mlp,
                                                   libvlc_playback_mode_t e_mode );
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif /* LIBVLC_MEDIA_LIST_PLAYER_H */
