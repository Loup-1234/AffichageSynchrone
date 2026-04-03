
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc.h:

Program Listing for File libvlc.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc.h:  libvlc external API
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
   
   
   #ifndef VLC_LIBVLC_H
   #define VLC_LIBVLC_H 1
   
   #if defined (_WIN32) && defined (DLL_EXPORT)
   # define LIBVLC_API __declspec(dllexport)
   #elif defined (__GNUC__) && (__GNUC__ >= 4)
   # define LIBVLC_API __attribute__((visibility("default")))
   #else
   # define LIBVLC_API
   #endif
   
   #ifdef __LIBVLC__
   /* Avoid unhelpful warnings from libvlc with our deprecated APIs */
   #   define LIBVLC_DEPRECATED
   #elif defined(__GNUC__) && \
         (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
   # define LIBVLC_DEPRECATED __attribute__((deprecated))
   #else
   # define LIBVLC_DEPRECATED
   #endif
   
   #include <stdio.h>
   #include <stdarg.h>
   #include <stdint.h>
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   typedef struct libvlc_instance_t libvlc_instance_t;
   
   typedef int64_t libvlc_time_t;
   
   
   LIBVLC_API const char *libvlc_errmsg (void);
   
   LIBVLC_API void libvlc_clearerr (void);
   
   LIBVLC_API const char *libvlc_vprinterr (const char *fmt, va_list ap);
   
   LIBVLC_API const char *libvlc_printerr (const char *fmt, ...);
   
   
   LIBVLC_API libvlc_instance_t *
   libvlc_new( int argc , const char *const *argv );
   
   LIBVLC_API void libvlc_release( libvlc_instance_t *p_instance );
   
   LIBVLC_API void libvlc_retain( libvlc_instance_t *p_instance );
   
   LIBVLC_API
   int libvlc_add_intf( libvlc_instance_t *p_instance, const char *name );
   
   LIBVLC_API
   void libvlc_set_exit_handler( libvlc_instance_t *p_instance,
                                 void (*cb) (void *), void *opaque );
   
   LIBVLC_API
   void libvlc_set_user_agent( libvlc_instance_t *p_instance,
                               const char *name, const char *http );
   
   LIBVLC_API
   void libvlc_set_app_id( libvlc_instance_t *p_instance, const char *id,
                           const char *version, const char *icon );
   
   LIBVLC_API const char * libvlc_get_version(void);
   
   LIBVLC_API const char * libvlc_get_compiler(void);
   
   LIBVLC_API const char * libvlc_get_changeset(void);
   
   LIBVLC_API void libvlc_free( void *ptr );
   
   
   typedef struct libvlc_event_manager_t libvlc_event_manager_t;
   
   struct libvlc_event_t;
   
   typedef int libvlc_event_type_t;
   
   typedef void ( *libvlc_callback_t )( const struct libvlc_event_t *p_event, void *p_data );
   
   LIBVLC_API int libvlc_event_attach( libvlc_event_manager_t *p_event_manager,
                                           libvlc_event_type_t i_event_type,
                                           libvlc_callback_t f_callback,
                                           void *user_data );
   
   LIBVLC_API void libvlc_event_detach( libvlc_event_manager_t *p_event_manager,
                                            libvlc_event_type_t i_event_type,
                                            libvlc_callback_t f_callback,
                                            void *p_user_data );
   
   LIBVLC_API const char * libvlc_event_type_name( libvlc_event_type_t event_type );
   
   
   
   enum libvlc_log_level
   {
       LIBVLC_DEBUG=0,   
       LIBVLC_NOTICE=2,  
       LIBVLC_WARNING=3, 
       LIBVLC_ERROR=4    
   };
   
   typedef struct vlc_log_t libvlc_log_t;
   
   LIBVLC_API void libvlc_log_get_context(const libvlc_log_t *ctx,
                          const char **module, const char **file, unsigned *line);
   
   LIBVLC_API void libvlc_log_get_object(const libvlc_log_t *ctx,
                           const char **name, const char **header, uintptr_t *id);
   
   typedef void (*libvlc_log_cb)(void *data, int level, const libvlc_log_t *ctx,
                                 const char *fmt, va_list args);
   
   LIBVLC_API void libvlc_log_unset( libvlc_instance_t *p_instance );
   
   LIBVLC_API void libvlc_log_set( libvlc_instance_t *p_instance,
                                   libvlc_log_cb cb, void *data );
   
   
   LIBVLC_API void libvlc_log_set_file( libvlc_instance_t *p_instance, FILE *stream );
   
   
   typedef struct libvlc_module_description_t
   {
       char *psz_name;
       char *psz_shortname;
       char *psz_longname;
       char *psz_help;
       struct libvlc_module_description_t *p_next;
   } libvlc_module_description_t;
   
   LIBVLC_API
   void libvlc_module_description_list_release( libvlc_module_description_t *p_list );
   
   LIBVLC_API
   libvlc_module_description_t *libvlc_audio_filter_list_get( libvlc_instance_t *p_instance );
   
   LIBVLC_API
   libvlc_module_description_t *libvlc_video_filter_list_get( libvlc_instance_t *p_instance );
   
   
   
   LIBVLC_API
   int64_t libvlc_clock(void);
   
   static inline int64_t libvlc_delay(int64_t pts)
   {
       return pts - libvlc_clock();
   }
   
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif 
