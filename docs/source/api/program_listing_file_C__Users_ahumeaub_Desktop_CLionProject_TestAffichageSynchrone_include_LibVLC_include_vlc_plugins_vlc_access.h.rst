
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_access.h:

Program Listing for File vlc_access.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_access.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_access.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_access.h: Access descriptor, queries and methods
    *****************************************************************************
    * Copyright (C) 1999-2006 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Laurent Aimar <fenrir@via.ecp.fr>
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
   
   #ifndef VLC_ACCESS_H
   #define VLC_ACCESS_H 1
   
   #include <vlc_stream.h>
   
   
   #define VLC_ACCESS_REDIRECT VLC_ETIMEOUT
   
   VLC_API stream_t *vlc_access_NewMRL(vlc_object_t *obj, const char *mrl);
   
   
   VLC_API int access_vaDirectoryControlHelper( stream_t *p_access, int i_query, va_list args );
   
   #define ACCESS_SET_CALLBACKS( read, block, control, seek ) \
       do { \
           p_access->pf_read = (read); \
           p_access->pf_block = (block); \
           p_access->pf_control = (control); \
           p_access->pf_seek = (seek); \
       } while(0)
   
   
   #endif
