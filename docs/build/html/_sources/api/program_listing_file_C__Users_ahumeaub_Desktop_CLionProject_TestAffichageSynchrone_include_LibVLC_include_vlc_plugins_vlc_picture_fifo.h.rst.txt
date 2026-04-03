
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_picture_fifo.h:

Program Listing for File vlc_picture_fifo.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_picture_fifo.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_picture_fifo.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_picture_fifo.h: picture fifo definitions
    *****************************************************************************
    * Copyright (C) 2009 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Laurent Aimar <fenrir _AT_ videolan _DOT_ org>
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
   
   #ifndef VLC_PICTURE_FIFO_H
   #define VLC_PICTURE_FIFO_H 1
   
   
   #include <vlc_picture.h>
   
   typedef struct picture_fifo_t picture_fifo_t;
   
   VLC_API picture_fifo_t * picture_fifo_New( void ) VLC_USED;
   
   VLC_API void picture_fifo_Delete( picture_fifo_t * );
   
   VLC_API picture_t * picture_fifo_Pop( picture_fifo_t * ) VLC_USED;
   
   VLC_API picture_t * picture_fifo_Peek( picture_fifo_t * ) VLC_USED;
   
   VLC_API void picture_fifo_Push( picture_fifo_t *, picture_t * );
   
   VLC_API void picture_fifo_Flush( picture_fifo_t *, vlc_tick_t date, bool flush_before );
   
   VLC_API void picture_fifo_OffsetDate( picture_fifo_t *, vlc_tick_t delta );
   
   
   #endif /* VLC_PICTURE_FIFO_H */
   
