
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_strings.h:

Program Listing for File vlc_strings.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_strings.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_strings.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_strings.h: String functions
    *****************************************************************************
    * Copyright (C) 2006 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Antoine Cellerier <dionoea at videolan dot org>
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
   
   #ifndef VLC_STRINGS_H
   #define VLC_STRINGS_H 1
   
   
   static inline int vlc_ascii_toupper( int c )
   {
       if ( c >= 'a' && c <= 'z' )
           return c + ( 'A' - 'a' );
       else
           return c;
   }
   
   static inline int vlc_ascii_tolower( int c )
   {
       if ( c >= 'A' && c <= 'Z' )
           return c + ( 'a' - 'A' );
       else
           return c;
   }
   
   static inline int vlc_ascii_strcasecmp( const char *psz1, const char *psz2 )
   {
       const char *s1 = psz1;
       const char *s2 = psz2;
       int d = vlc_ascii_tolower( *s1 ) - vlc_ascii_tolower( *s2 );
       while ( *s1 && d == 0)
       {
           s1++;
           s2++;
           d = vlc_ascii_tolower( *s1 ) - vlc_ascii_tolower( *s2 );
       }
   
       return d;
   }
   
   static inline int vlc_ascii_strncasecmp( const char *psz1, const char *psz2, size_t n )
   {
       const char *s1 = psz1;
       const char *s2 = psz2;
       const char *s1end = psz1 + n;
       int d = vlc_ascii_tolower( *s1 ) - vlc_ascii_tolower( *s2 );
       while ( *s1 && s1 < s1end && d == 0)
       {
           s1++;
           s2++;
           d = vlc_ascii_tolower( *s1 ) - vlc_ascii_tolower( *s2 );
       }
   
       if (s1 == s1end)
           return 0;
       else
           return d;
   }
   
   VLC_API void vlc_xml_decode(char *st);
   
   VLC_API char *vlc_xml_encode(const char *str) VLC_MALLOC;
   
   VLC_API char * vlc_b64_encode_binary( const uint8_t *, size_t );
   VLC_API char * vlc_b64_encode( const char * );
   
   VLC_API size_t vlc_b64_decode_binary_to_buffer( uint8_t *p_dst, size_t i_dst_max, const char *psz_src );
   VLC_API size_t vlc_b64_decode_binary( uint8_t **pp_dst, const char *psz_src );
   VLC_API char * vlc_b64_decode( const char *psz_src );
   
   VLC_API char *vlc_strftime( const char * );
   
   VLC_API char *vlc_strfinput( input_thread_t *, const char * ) VLC_MALLOC;
   
   static inline char *str_format( input_thread_t *input, const char *fmt )
   {
       char *s1 = vlc_strftime( fmt );
       char *s2 = vlc_strfinput( input, s1 );
       free( s1 );
       return s2;
   }
   
   VLC_API int vlc_filenamecmp(const char *, const char *);
   
   void filename_sanitize(char *);
   
   
   #endif
