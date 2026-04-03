
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_media_library.h:

Program Listing for File vlc_media_library.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_media_library.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_media_library.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_media_library.h: SQL-based media library
    *****************************************************************************
    * Copyright (C) 2008-2010 the VideoLAN Team and AUTHORS
    * $Id$
    *
    * Authors: Antoine Lejeune <phytos@videolan.org>
    *          Jean-Philippe André <jpeg@videolan.org>
    *          Rémi Duraffort <ivoire@videolan.org>
    *          Adrien Maglo <magsoft@videolan.org>
    *          Srikanth Raju <srikiraju at gmail dot com>
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
   
   #ifndef VLC_MEDIA_LIBRARY_H
   # define VLC_MEDIA_LIBRARY_H
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   /*****************************************************************************
    * ML Enums
    *****************************************************************************/
   
   #define ML_PERSON_ARTIST        "Artist"
   #define ML_PERSON_ALBUM_ARTIST  "Album Artist"
   #define ML_PERSON_ENCODER       "Encoder"
   #define ML_PERSON_PUBLISHER     "Publisher"
   
   
   typedef enum
   {
       ML_ALBUM = 1,              
       ML_ALBUM_ID,               
       ML_ALBUM_COVER,            
       /* FIXME: Remove ML_ARTIST */
       ML_ARTIST,                 
       ML_ARTIST_ID,              
       ML_COMMENT,                
       ML_COUNT_MEDIA,            
       ML_COUNT_ALBUM,            
       ML_COUNT_PEOPLE,           
       ML_COVER,                  
       ML_DURATION,               
       ML_DISC_NUMBER,            
       ML_EXTRA,                  
       ML_FIRST_PLAYED,           
       ML_FILESIZE,               
       ML_GENRE,                  
       ML_ID,                     
       ML_IMPORT_TIME,            
       ML_LANGUAGE,               
       ML_LAST_PLAYED,            
       ML_LAST_SKIPPED,           
       ML_ORIGINAL_TITLE,         
       ML_PEOPLE,                 
       ML_PEOPLE_ID,              
       ML_PEOPLE_ROLE,            
       ML_PLAYED_COUNT,           
       ML_PREVIEW,                
       ML_SKIPPED_COUNT,          
       ML_SCORE,                  
       ML_TITLE,                  
       ML_TRACK_NUMBER,           
       ML_TYPE,                   
       ML_URI,                    
       ML_VOTE,                   
       ML_YEAR,                   
       ML_DIRECTORY,              
       ML_MEDIA,                  
       ML_MEDIA_SPARSE,           
       ML_MEDIA_EXTRA,            
   
       /* Some special elements */
       ML_LIMIT     = -1,         
       ML_SORT_DESC = -2,         
       ML_SORT_ASC  = -3,         
       ML_DISTINCT  = -4,         
       ML_END       = -42         
   } ml_select_e;
   
   typedef enum
   {
       ML_UNKNOWN   = 0,       
       ML_AUDIO     = 1 << 0,  
       ML_VIDEO     = 1 << 1,  
       ML_STREAM    = 1 << 2,  
       ML_NODE      = 1 << 3,  
       ML_REMOVABLE = 1 << 4,  
   } ml_type_e;
   
   typedef enum {
       ML_TYPE_INT,        
       ML_TYPE_PSZ,        
       ML_TYPE_TIME,       
       ML_TYPE_MEDIA,      
   } ml_result_type_e;
   
   #ifdef __cplusplus
   }
   #endif /* C++ */
   
   #endif /* VLC_MEDIA_LIBRARY_H */
