
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_aout_volume.h:

Program Listing for File vlc_aout_volume.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_aout_volume.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_aout_volume.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_aout_volume.h: audio volume module
    *****************************************************************************
    * Copyright (C) 2002-2009 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Christophe Massiot <massiot@via.ecp.fr>
    *          Laurent Aimar <fenrir _AT_ videolan _DOT_ org>
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
   
   #ifndef VLC_AOUT_MIXER_H
   #define VLC_AOUT_MIXER_H 1
   
   #ifdef __cplusplus
   extern "C" {
   #endif
   
   
   typedef struct audio_volume audio_volume_t;
   
   struct audio_volume
   {
       VLC_COMMON_MEMBERS
   
       vlc_fourcc_t format; 
       void (*amplify)(audio_volume_t *, block_t *, float); 
   };
   
   
   #ifdef __cplusplus
   }
   #endif
   
   #endif
