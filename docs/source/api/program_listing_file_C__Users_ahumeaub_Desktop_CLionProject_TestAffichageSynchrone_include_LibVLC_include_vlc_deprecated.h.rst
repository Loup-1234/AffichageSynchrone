
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_deprecated.h:

Program Listing for File deprecated.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_deprecated.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\deprecated.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * deprecated.h:  libvlc deprecated API
    *****************************************************************************
    * Copyright (C) 1998-2008 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Clément Stenac <zorglub@videolan.org>
    *          Jean-Paul Saman <jpsaman@videolan.org>
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
   
   #ifndef LIBVLC_DEPRECATED_H
   #define LIBVLC_DEPRECATED_H 1
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   LIBVLC_DEPRECATED
   LIBVLC_API float libvlc_media_player_get_fps( libvlc_media_player_t *p_mi );
   
   
   LIBVLC_DEPRECATED
   LIBVLC_API void libvlc_media_player_set_agl ( libvlc_media_player_t *p_mi, uint32_t drawable );
   
   LIBVLC_DEPRECATED
   LIBVLC_API uint32_t libvlc_media_player_get_agl ( libvlc_media_player_t *p_mi );
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_track_description_release( libvlc_track_description_t *p_track_description );
   
   
   
   LIBVLC_DEPRECATED LIBVLC_API
   int libvlc_video_get_height( libvlc_media_player_t *p_mi );
   
   LIBVLC_DEPRECATED LIBVLC_API
   int libvlc_video_get_width( libvlc_media_player_t *p_mi );
   
   LIBVLC_DEPRECATED LIBVLC_API libvlc_track_description_t *
           libvlc_video_get_title_description( libvlc_media_player_t *p_mi );
   
   LIBVLC_DEPRECATED LIBVLC_API libvlc_track_description_t *
           libvlc_video_get_chapter_description( libvlc_media_player_t *p_mi, int i_title );
   
   LIBVLC_DEPRECATED LIBVLC_API int
   libvlc_video_set_subtitle_file( libvlc_media_player_t *p_mi, const char *psz_subtitle );
   
   LIBVLC_DEPRECATED LIBVLC_API void
   libvlc_toggle_teletext( libvlc_media_player_t *p_mi );
   
   
   
   LIBVLC_DEPRECATED LIBVLC_API
   int libvlc_audio_output_device_count( libvlc_instance_t *p_instance, const char *psz_audio_output );
   
   LIBVLC_DEPRECATED LIBVLC_API
   char *libvlc_audio_output_device_longname( libvlc_instance_t *p_instance, const char *psz_output,
                                              int i_device );
   
   LIBVLC_DEPRECATED LIBVLC_API
   char *libvlc_audio_output_device_id( libvlc_instance_t *p_instance, const char *psz_audio_output, int i_device );
   
   LIBVLC_DEPRECATED
   LIBVLC_API int libvlc_audio_output_get_device_type( libvlc_media_player_t *p_mi );
   
   LIBVLC_DEPRECATED
   LIBVLC_API void libvlc_audio_output_set_device_type( libvlc_media_player_t *p_mp,
                                                        int device_type );
   
   
   
   LIBVLC_DEPRECATED LIBVLC_API void
   libvlc_media_parse( libvlc_media_t *p_md );
   
   LIBVLC_DEPRECATED LIBVLC_API void
   libvlc_media_parse_async( libvlc_media_t *p_md );
   
   LIBVLC_DEPRECATED LIBVLC_API int
      libvlc_media_is_parsed( libvlc_media_t *p_md );
   
   LIBVLC_DEPRECATED LIBVLC_API
   int libvlc_media_get_tracks_info( libvlc_media_t *p_md,
                                     libvlc_media_track_info_t **tracks );
   
   
   
   LIBVLC_DEPRECATED int
       libvlc_media_list_add_file_content( libvlc_media_list_t * p_ml,
                                           const char * psz_uri );
   
   
   
   LIBVLC_DEPRECATED LIBVLC_API libvlc_media_discoverer_t *
   libvlc_media_discoverer_new_from_name( libvlc_instance_t * p_inst,
                                          const char * psz_name );
   
   LIBVLC_DEPRECATED LIBVLC_API char *
   libvlc_media_discoverer_localized_name( libvlc_media_discoverer_t * p_mdis );
   
   LIBVLC_DEPRECATED LIBVLC_API libvlc_event_manager_t *
   libvlc_media_discoverer_event_manager( libvlc_media_discoverer_t * p_mdis );
   
   
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_wait( libvlc_instance_t *p_instance );
   
   
   
   
   typedef struct libvlc_log_iterator_t libvlc_log_iterator_t;
   
   typedef struct libvlc_log_message_t
   {
       int         i_severity;   /* 0=INFO, 1=ERR, 2=WARN, 3=DBG */
       const char *psz_type;     /* module type */
       const char *psz_name;     /* module name */
       const char *psz_header;   /* optional header */
       const char *psz_message;  /* message */
   } libvlc_log_message_t;
   
   LIBVLC_DEPRECATED LIBVLC_API
   unsigned libvlc_get_log_verbosity( const libvlc_instance_t *p_instance );
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_set_log_verbosity( libvlc_instance_t *p_instance, unsigned level );
   
   LIBVLC_DEPRECATED LIBVLC_API
   libvlc_log_t *libvlc_log_open( libvlc_instance_t *p_instance );
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_log_close( libvlc_log_t *p_log );
   
   LIBVLC_DEPRECATED LIBVLC_API
   unsigned libvlc_log_count( const libvlc_log_t *p_log );
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_log_clear( libvlc_log_t *p_log );
   
   LIBVLC_DEPRECATED LIBVLC_API
   libvlc_log_iterator_t *libvlc_log_get_iterator( const libvlc_log_t *p_log );
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_log_iterator_free( libvlc_log_iterator_t *p_iter );
   
   LIBVLC_DEPRECATED LIBVLC_API
   int libvlc_log_iterator_has_next( const libvlc_log_iterator_t *p_iter );
   
   LIBVLC_DEPRECATED LIBVLC_API
   libvlc_log_message_t *libvlc_log_iterator_next( libvlc_log_iterator_t *p_iter,
                                                   libvlc_log_message_t *p_buf );
   
   
   
   LIBVLC_DEPRECATED LIBVLC_API
   void libvlc_playlist_play( libvlc_instance_t *p_instance, int i_id,
                              int i_options, char **ppsz_options );
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif /* _LIBVLC_DEPRECATED_H */
