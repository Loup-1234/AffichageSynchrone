
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_url.h:

Program Listing for File vlc_url.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_url.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_url.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_url.h: URL related macros
    *****************************************************************************
    * Copyright (C) 2002-2006 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Christophe Massiot <massiot@via.ecp.fr>
    *          Rémi Denis-Courmont
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
   
   #ifndef VLC_URL_H
   # define VLC_URL_H
   
   
   VLC_API char *vlc_path2uri(const char *path, const char *scheme) VLC_MALLOC;
   
   VLC_API char *vlc_uri2path(const char *url) VLC_MALLOC;
   
   VLC_API char *vlc_uri_decode(char *str);
   
   VLC_API char *vlc_uri_decode_duplicate(const char *str) VLC_MALLOC;
   
   VLC_API char *vlc_uri_encode(const char *str) VLC_MALLOC;
   
   VLC_API char *vlc_uri_compose(const vlc_url_t *) VLC_MALLOC;
   
   VLC_API char *vlc_uri_resolve(const char *base, const char *ref) VLC_MALLOC;
   
   VLC_API char *vlc_uri_fixup(const char *) VLC_MALLOC;
   
   struct vlc_url_t
   {
       char *psz_protocol;
       char *psz_username;
       char *psz_password;
       char *psz_host;
       unsigned i_port;
       char *psz_path;
       char *psz_option;
   
       char *psz_buffer; /* to be freed */
       char *psz_pathbuffer; /* to be freed */
   };
   
   VLC_API int vlc_UrlParse(vlc_url_t *url, const char *str);
   
   VLC_API int vlc_UrlParseFixup(vlc_url_t *url, const char *str);
   
   VLC_API void vlc_UrlClean(vlc_url_t *);
   
   
   #endif
