
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_md5.h:

Program Listing for File vlc_md5.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_md5.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_md5.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_md5.h: MD5 hash
    *****************************************************************************
    * Copyright © 2004-2011 VLC authors and VideoLAN
    *
    * Authors: Rémi Denis-Courmont
    *          Rafaël Carré
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
   
   #ifndef VLC_MD5_H
   # define VLC_MD5_H
   
   
   struct md5_s
   {
       uint32_t A, B, C, D;          /* chaining variables */
       uint32_t nblocks;
       uint8_t buf[64];
       int count;
   };
   
   VLC_API void InitMD5( struct md5_s * );
   VLC_API void AddMD5( struct md5_s *, const void *, size_t );
   VLC_API void EndMD5( struct md5_s * );
   
   static inline char * psz_md5_hash( struct md5_s *md5_s )
   {
       char *psz = (char*)malloc( 33 ); /* md5 string is 32 bytes + NULL character */
       if( likely(psz) )
       {
           for( int i = 0; i < 16; i++ )
               sprintf( &psz[2*i], "%02" PRIx8, md5_s->buf[i] );
       }
       return psz;
   }
   
   #endif
