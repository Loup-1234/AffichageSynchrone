
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_configuration.h:

Program Listing for File vlc_configuration.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_configuration.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_configuration.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_configuration.h : configuration management module
    * This file describes the programming interface for the configuration module.
    * It includes functions allowing to declare, get or set configuration options.
    *****************************************************************************
    * Copyright (C) 1999-2006 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Gildas Bazin <gbazin@videolan.org>
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
   
   #ifndef VLC_CONFIGURATION_H
   #define VLC_CONFIGURATION_H 1
   
   
   #include <sys/types.h>  /* for ssize_t */
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   struct config_category_t
   {
       int         i_id;
       const char *psz_name;
       const char *psz_help;
   };
   
   typedef union
   {
       char       *psz;
       int64_t     i;
       float       f;
   } module_value_t;
   
   typedef int (*vlc_string_list_cb)(vlc_object_t *, const char *,
                                     char ***, char ***);
   typedef int (*vlc_integer_list_cb)(vlc_object_t *, const char *,
                                      int64_t **, char ***);
   
   struct module_config_t
   {
       uint8_t     i_type;                        /* Configuration type */
       char        i_short;               /* Optional short option name */
       unsigned    b_advanced:1;                     /* Advanced option */
       unsigned    b_internal:1;          /* Hidden from prefs and help */
       unsigned    b_unsaveable:1;       /* Not stored in configuration */
       unsigned    b_safe:1;       /* Safe in web plugins and playlists */
       unsigned    b_removed:1;                           /* Deprecated */
   
       const char *psz_type;                           /* Configuration subtype */
       const char *psz_name;                                     /* Option name */
       const char *psz_text;       /* Short comment on the configuration option */
       const char *psz_longtext;    /* Long comment on the configuration option */
   
       module_value_t value;                                    /* Option value */
       module_value_t orig;
       module_value_t min;
       module_value_t max;
   
       /* Values list */
       uint16_t list_count;                                /* Options list size */
       union
       {
           const char **psz;          /* List of possible values for the option */
           const int  *i;
           vlc_string_list_cb psz_cb;
           vlc_integer_list_cb i_cb;
       } list;
       const char **list_text;                /* Friendly names for list values */
       const char *list_cb_name;
       void *owner;
   };
   
   /*****************************************************************************
    * Prototypes - these methods are used to get, set or manipulate configuration
    * data.
    *****************************************************************************/
   VLC_API int config_GetType(const char *) VLC_USED;
   VLC_API int64_t config_GetInt(vlc_object_t *, const char *) VLC_USED;
   VLC_API void config_PutInt(vlc_object_t *, const char *, int64_t);
   VLC_API float config_GetFloat(vlc_object_t *, const char *) VLC_USED;
   VLC_API void config_PutFloat(vlc_object_t *, const char *, float);
   VLC_API char * config_GetPsz(vlc_object_t *, const char *) VLC_USED VLC_MALLOC;
   VLC_API void config_PutPsz(vlc_object_t *, const char *, const char *);
   VLC_API ssize_t config_GetIntChoices(vlc_object_t *, const char *,
                                        int64_t **, char ***) VLC_USED;
   VLC_API ssize_t config_GetPszChoices(vlc_object_t *, const char *,
                                        char ***, char ***) VLC_USED;
   
   VLC_API int config_SaveConfigFile( vlc_object_t * );
   #define config_SaveConfigFile(a) config_SaveConfigFile(VLC_OBJECT(a))
   
   VLC_API void config_ResetAll( vlc_object_t * );
   #define config_ResetAll(a) config_ResetAll(VLC_OBJECT(a))
   
   VLC_API module_config_t * config_FindConfig(const char *) VLC_USED;
   VLC_API char * config_GetDataDir(void) VLC_USED VLC_MALLOC;
   VLC_API char *config_GetLibDir(void) VLC_USED;
   
   typedef enum vlc_userdir
   {
       VLC_HOME_DIR, /* User's home */
       VLC_CONFIG_DIR, /* VLC-specific configuration directory */
       VLC_DATA_DIR, /* VLC-specific data directory */
       VLC_CACHE_DIR, /* VLC-specific user cached data directory */
       /* Generic directories (same as XDG) */
       VLC_DESKTOP_DIR=0x80,
       VLC_DOWNLOAD_DIR,
       VLC_TEMPLATES_DIR,
       VLC_PUBLICSHARE_DIR,
       VLC_DOCUMENTS_DIR,
       VLC_MUSIC_DIR,
       VLC_PICTURES_DIR,
       VLC_VIDEOS_DIR,
   } vlc_userdir_t;
   
   VLC_API char * config_GetUserDir( vlc_userdir_t ) VLC_USED VLC_MALLOC;
   
   VLC_API void config_AddIntf( vlc_object_t *, const char * );
   VLC_API void config_RemoveIntf( vlc_object_t *, const char * );
   VLC_API bool config_ExistIntf( vlc_object_t *, const char * ) VLC_USED;
   
   #define config_GetInt(a,b) config_GetInt(VLC_OBJECT(a),b)
   #define config_PutInt(a,b,c) config_PutInt(VLC_OBJECT(a),b,c)
   #define config_GetFloat(a,b) config_GetFloat(VLC_OBJECT(a),b)
   #define config_PutFloat(a,b,c) config_PutFloat(VLC_OBJECT(a),b,c)
   #define config_GetPsz(a,b) config_GetPsz(VLC_OBJECT(a),b)
   #define config_PutPsz(a,b,c) config_PutPsz(VLC_OBJECT(a),b,c)
   
   #define config_AddIntf(a,b) config_AddIntf(VLC_OBJECT(a),b)
   #define config_RemoveIntf(a,b) config_RemoveIntf(VLC_OBJECT(a),b)
   #define config_ExistIntf(a,b) config_ExistIntf(VLC_OBJECT(a),b)
   
   /****************************************************************************
    * config_chain_t:
    ****************************************************************************/
   struct config_chain_t
   {
       config_chain_t *p_next;     
   
       char        *psz_name;      
       char        *psz_value;     
   };
   
   VLC_API void config_ChainParse( vlc_object_t *, const char *psz_prefix, const char *const *ppsz_options, config_chain_t * );
   #define config_ChainParse( a, b, c, d ) config_ChainParse( VLC_OBJECT(a), b, c, d )
   
   VLC_API const char *config_ChainParseOptions( config_chain_t **pp_cfg, const char *ppsz_opts );
   
   VLC_API char *config_ChainCreate( char **ppsz_name, config_chain_t **pp_cfg, const char *psz_string ) VLC_USED VLC_MALLOC;
   
   VLC_API void config_ChainDestroy( config_chain_t * );
   
   VLC_API config_chain_t * config_ChainDuplicate( const config_chain_t * ) VLC_USED VLC_MALLOC;
   
   VLC_API char * config_StringUnescape( char *psz_string );
   
   VLC_API char * config_StringEscape( const char *psz_string ) VLC_USED VLC_MALLOC;
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif /* _VLC_CONFIGURATION_H */
