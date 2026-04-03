
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_vout_osd.h:

Program Listing for File vlc_vout_osd.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_vout_osd.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_vout_osd.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_vout_osd.h: vout OSD
    *****************************************************************************
    * Copyright (C) 1999-2010 VLC authors and VideoLAN
    * Copyright (C) 2004-2005 M2X
    * $Id$
    *
    * Authors: Jean-Paul Saman <jpsaman #_at_# m2x dot nl>
    *          Gildas Bazin <gbazin@videolan.org>
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
   
   #ifndef VLC_VOUT_OSD_H
   #define VLC_VOUT_OSD_H 1
   
   #ifdef __cplusplus
   extern "C" {
   #endif
   
   
   enum
   {
       /* Icons */
       OSD_PLAY_ICON = 1,
       OSD_PAUSE_ICON,
       OSD_SPEAKER_ICON,
       OSD_MUTE_ICON,
       /* Sliders */
       OSD_HOR_SLIDER,
       OSD_VERT_SLIDER,
   };
   
   VLC_API int vout_OSDEpg( vout_thread_t *, input_item_t * );
   
   VLC_API void vout_OSDText( vout_thread_t *vout, int channel, int position, vlc_tick_t duration, const char *text );
   
   VLC_API void vout_OSDMessage( vout_thread_t *, int, const char *, ... ) VLC_FORMAT( 3, 4 );
   
   VLC_API void vout_OSDSlider( vout_thread_t *, int, int , short );
   
   VLC_API void vout_OSDIcon( vout_thread_t *, int, short );
   
   #ifdef __cplusplus
   }
   #endif
   
   #endif /* VLC_VOUT_OSD_H */
   
