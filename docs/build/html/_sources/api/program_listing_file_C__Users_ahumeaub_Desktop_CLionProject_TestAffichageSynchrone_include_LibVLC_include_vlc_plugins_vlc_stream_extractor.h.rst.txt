
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_stream_extractor.h:

Program Listing for File vlc_stream_extractor.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_stream_extractor.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_stream_extractor.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_stream_extractor.h
    *****************************************************************************
    * Copyright (C) 2016 VLC authors and VideoLAN
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
   
   #ifndef VLC_STREAM_EXTRACTOR_H
   #define VLC_STREAM_EXTRACTOR_H
   
   #ifdef __cplusplus
   extern "C" {
   #endif
   
   
   typedef struct stream_extractor_t {
       VLC_COMMON_MEMBERS
   
       ssize_t  (*pf_read)(struct stream_extractor_t*, void* buf, size_t len);
       block_t* (*pf_block)(struct stream_extractor_t*, bool* eof);
       int      (*pf_seek)(struct stream_extractor_t*, uint64_t);
       int      (*pf_control)(struct stream_extractor_t*, int request, va_list args);
   
       char const* identifier; 
       stream_t* source; 
       void* p_sys;      
   
   } stream_extractor_t;
   
   typedef struct stream_directory_t {
       VLC_COMMON_MEMBERS
   
       int (*pf_readdir)(struct stream_directory_t*, input_item_node_t* );
   
       stream_t* source; 
       void* p_sys; 
   
   } stream_directory_t;
   
   VLC_API stream_t * vlc_stream_NewMRL(vlc_object_t *obj, const char *mrl)
   VLC_USED;
   #define vlc_stream_NewMRL(a, b) vlc_stream_NewMRL(VLC_OBJECT(a), b)
   
   VLC_API char* vlc_stream_extractor_CreateMRL( stream_directory_t*,
                                                 char const* subentry );
   
   
   VLC_API int vlc_stream_extractor_Attach( stream_t** source,
                                            char const* identifier,
                                            char const* module_name );
   
   VLC_API int vlc_stream_directory_Attach( stream_t** source,
                                            char const* module_name );
   
   
   #ifdef __cplusplus
   } /* extern "C" */
   #endif
   #endif /* include-guard */
