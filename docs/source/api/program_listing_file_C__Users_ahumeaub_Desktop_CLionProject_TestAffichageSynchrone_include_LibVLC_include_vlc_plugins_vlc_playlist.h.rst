
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_playlist.h:

Program Listing for File vlc_playlist.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_playlist.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_playlist.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_playlist.h : Playlist functions
    *****************************************************************************
    * Copyright (C) 1999-2004 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Samuel Hocevar <sam@zoy.org>
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
   
   #ifndef VLC_PLAYLIST_H_
   #define VLC_PLAYLIST_H_
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   #include <vlc_events.h>
   #include <vlc_aout.h>
   
   TYPEDEF_ARRAY(playlist_item_t*, playlist_item_array_t)
   
   struct intf_thread_t;
   
   
   typedef struct playlist_export_t
   {
       VLC_COMMON_MEMBERS
       char *base_url;
       FILE *p_file;
       playlist_item_t *p_root;
   } playlist_export_t;
   
   struct playlist_item_t
   {
       input_item_t           *p_input;    
   
       playlist_item_t      **pp_children; 
       playlist_item_t       *p_parent;    
       int                    i_children;  
       unsigned               i_nb_played; 
   
       int                    i_id;        
       uint8_t                i_flags;     
   };
   
   typedef enum {
       PLAYLIST_DBL_FLAG          = 0x04,  
       PLAYLIST_RO_FLAG           = 0x08,  
       PLAYLIST_SUBITEM_STOP_FLAG = 0x40,  
       PLAYLIST_NO_INHERIT_FLAG   = 0x80,  
   } playlist_item_flags_e;
   
   typedef enum
   { PLAYLIST_STOPPED,PLAYLIST_RUNNING,PLAYLIST_PAUSED } playlist_status_t;
   
   struct playlist_t
   {
       VLC_COMMON_MEMBERS
   
       playlist_item_array_t items; 
   
       playlist_item_array_t current; 
       int                   i_current_index; 
   
       /* Predefined items */
       playlist_item_t  root;
       playlist_item_t *p_playing;
       playlist_item_t *p_media_library;
   };
   
   /* A bit of macro magic to generate an enum out of the following list,
    * and later, to generate a list of static functions out of the same list.
    * There is also SORT_RANDOM, which is always last and handled specially.
    */
   #define VLC_DEFINE_SORT_FUNCTIONS \
       DEF( SORT_ID )\
       DEF( SORT_TITLE )\
       DEF( SORT_TITLE_NODES_FIRST )\
       DEF( SORT_ARTIST )\
       DEF( SORT_GENRE )\
       DEF( SORT_DURATION )\
       DEF( SORT_TITLE_NUMERIC )\
       DEF( SORT_ALBUM )\
       DEF( SORT_TRACK_NUMBER )\
       DEF( SORT_DESCRIPTION )\
       DEF( SORT_RATING )\
       DEF( SORT_URI )\
       DEF( SORT_DISC_NUMBER )\
       DEF( SORT_DATE )
   
   #define DEF( s ) s,
   enum
   {
       VLC_DEFINE_SORT_FUNCTIONS
       SORT_RANDOM,
       NUM_SORT_FNS=SORT_RANDOM
   };
   #undef  DEF
   #ifndef VLC_INTERNAL_PLAYLIST_SORT_FUNCTIONS
   #undef  VLC_DEFINE_SORT_FUNCTIONS
   #endif
   
   enum
   {
       ORDER_NORMAL = 0,
       ORDER_REVERSE = 1,
   };
   
   #define PLAYLIST_END           -1
   
   enum pl_locked_state
   {
       pl_Locked = true,
       pl_Unlocked = false
   };
   
   /*****************************************************************************
    * Prototypes
    *****************************************************************************/
   
   /* Helpers */
   #define PL_LOCK playlist_Lock( p_playlist )
   #define PL_UNLOCK playlist_Unlock( p_playlist )
   #define PL_ASSERT_LOCKED playlist_AssertLocked( p_playlist )
   
   enum {
       PLAYLIST_PLAY,      
       PLAYLIST_VIEWPLAY,  
       PLAYLIST_TOGGLE_PAUSE, 
       PLAYLIST_STOP,      
       PLAYLIST_SKIP,      
       PLAYLIST_PAUSE,     
       PLAYLIST_RESUME,    
   };
   
   #define playlist_Play(p) playlist_Control(p,PLAYLIST_PLAY, pl_Unlocked )
   #define playlist_TogglePause(p) \
           playlist_Control(p, PLAYLIST_TOGGLE_PAUSE, pl_Unlocked)
   #define playlist_Stop(p) playlist_Control(p,PLAYLIST_STOP, pl_Unlocked )
   #define playlist_Next(p) playlist_Control(p,PLAYLIST_SKIP, pl_Unlocked, 1)
   #define playlist_Prev(p) playlist_Control(p,PLAYLIST_SKIP, pl_Unlocked, -1)
   #define playlist_Skip(p,i) playlist_Control(p,PLAYLIST_SKIP, pl_Unlocked,  (i) )
   #define playlist_Pause(p) \
           playlist_Control(p, PLAYLIST_PAUSE, pl_Unlocked)
   #define playlist_Resume(p) \
           playlist_Control(p, PLAYLIST_RESUME, pl_Unlocked)
   
   VLC_API void playlist_Lock( playlist_t * );
   
   VLC_API void playlist_Unlock( playlist_t * );
   
   VLC_API void playlist_AssertLocked( playlist_t * );
   VLC_API void playlist_Deactivate( playlist_t * );
   
   VLC_API void playlist_Control( playlist_t *p_playlist, int i_query, int b_locked, ...  );
   
   static inline void playlist_ViewPlay(playlist_t *pl, playlist_item_t *node,
                                        playlist_item_t *item)
   {
       playlist_Control(pl, PLAYLIST_VIEWPLAY, pl_Locked, node, item);
   }
   
   VLC_API input_thread_t * playlist_CurrentInput( playlist_t *p_playlist ) VLC_USED;
   VLC_API input_thread_t *playlist_CurrentInputLocked( playlist_t *p_playlist ) VLC_USED;
   
   VLC_API vlc_tick_t playlist_GetNodeDuration( playlist_item_t * );
   
   VLC_API void playlist_Clear( playlist_t *, bool );
   
   /* Playlist sorting */
   VLC_API int playlist_TreeMove( playlist_t *, playlist_item_t *, playlist_item_t *, int );
   VLC_API int playlist_TreeMoveMany( playlist_t *, int, playlist_item_t **, playlist_item_t *, int );
   VLC_API int playlist_RecursiveNodeSort( playlist_t *, playlist_item_t *,int, int );
   
   VLC_API playlist_item_t * playlist_CurrentPlayingItem( playlist_t * ) VLC_USED;
   VLC_API int playlist_Status( playlist_t * );
   
   VLC_API int playlist_Export( playlist_t *p_playlist, const char *psz_name,
                                bool b_playlist, const char *psz_type );
   
   VLC_API int playlist_Import( playlist_t *p_playlist, const char *psz_file );
   
   /********************** Services discovery ***********************/
   
   VLC_API int playlist_ServicesDiscoveryAdd(playlist_t *, const char *);
   VLC_API int playlist_ServicesDiscoveryRemove(playlist_t *, const char *);
   VLC_API bool playlist_IsServicesDiscoveryLoaded( playlist_t *,const char *) VLC_DEPRECATED;
   VLC_API int playlist_ServicesDiscoveryControl( playlist_t *, const char *, int, ... );
   
   /********************** Renderer ***********************/
   VLC_API int playlist_SetRenderer( playlist_t* p_pl, vlc_renderer_item_t* p_item );
   
   
   /********************************************************
    * Item management
    ********************************************************/
   
   /******************** Item addition ********************/
   VLC_API int playlist_Add( playlist_t *, const char *, bool );
   VLC_API int playlist_AddExt( playlist_t *, const char *, const char *, bool, int, const char *const *, unsigned, bool );
   VLC_API int playlist_AddInput( playlist_t *, input_item_t *, bool, bool );
   VLC_API playlist_item_t * playlist_NodeAddInput( playlist_t *, input_item_t *, playlist_item_t *, int );
   VLC_API int playlist_NodeAddCopy( playlist_t *, playlist_item_t *, playlist_item_t *, int );
   
   /********************************** Item search *************************/
   VLC_API playlist_item_t * playlist_ItemGetById(playlist_t *, int ) VLC_USED;
   VLC_API playlist_item_t *playlist_ItemGetByInput(playlist_t *,
                                                    const input_item_t * )
   VLC_USED;
   
   VLC_API int playlist_LiveSearchUpdate(playlist_t *, playlist_item_t *, const char *, bool );
   
   /********************************************************
    * Tree management
    ********************************************************/
   /* Node management */
   VLC_API playlist_item_t * playlist_NodeCreate( playlist_t *, const char *, playlist_item_t * p_parent, int i_pos, int i_flags );
   VLC_API playlist_item_t * playlist_ChildSearchName(playlist_item_t*, const char* ) VLC_USED;
   VLC_API void playlist_NodeDelete( playlist_t *, playlist_item_t * );
   
   /**************************
    * Audio output management
    **************************/
   
   VLC_API audio_output_t *playlist_GetAout( playlist_t * );
   
   VLC_API float playlist_VolumeGet( playlist_t * );
   VLC_API int playlist_VolumeSet( playlist_t *, float );
   VLC_API int playlist_VolumeUp( playlist_t *, int, float * );
   #define playlist_VolumeDown(a, b, c) playlist_VolumeUp(a, -(b), c)
   VLC_API int playlist_MuteSet( playlist_t *, bool );
   VLC_API int playlist_MuteGet( playlist_t * );
   
   static inline int playlist_MuteToggle( playlist_t *pl )
   {
       int val = playlist_MuteGet( pl );
       if (val >= 0)
           val = playlist_MuteSet( pl, !val );
       return val;
   }
   
   VLC_API void playlist_EnableAudioFilter( playlist_t *, const char *, bool );
   
   /***********************************************************************
    * Inline functions
    ***********************************************************************/
   static inline bool playlist_IsEmpty( playlist_t *p_playlist )
   {
       PL_ASSERT_LOCKED;
       return p_playlist->items.i_size == 0;
   }
   
   static inline int playlist_CurrentSize( playlist_t *p_playlist )
   {
       PL_ASSERT_LOCKED;
       return p_playlist->current.i_size;
   }
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif
