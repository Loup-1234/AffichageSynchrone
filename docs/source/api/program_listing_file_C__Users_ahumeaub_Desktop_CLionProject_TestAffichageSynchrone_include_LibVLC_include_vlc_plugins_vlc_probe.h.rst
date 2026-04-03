
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_probe.h:

Program Listing for File vlc_probe.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_probe.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_probe.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_probe.h: service probing interface
    *****************************************************************************
    * Copyright (C) 2009 Rémi Denis-Courmont
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
   
   #ifndef VLC_PROBE_H
   # define VLC_PROBE_H 1
   
   # include <stdlib.h>
   
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   void *vlc_probe (vlc_object_t *, const char *, size_t *);
   #define vlc_probe(obj, cap, pcount) \
           vlc_probe(VLC_OBJECT(obj), cap, pcount)
   
   struct vlc_probe_t
   {
       VLC_COMMON_MEMBERS
   
       void  *list;
       size_t count;
   };
   
   typedef struct vlc_probe_t vlc_probe_t;
   
   static inline int vlc_probe_add(vlc_probe_t *obj, const void *data,
                                   size_t len)
   {
       char *tab = (char *)realloc (obj->list, (obj->count + 1) * len);
   
       if (unlikely(tab == NULL))
           return VLC_ENOMEM;
       memcpy(tab + (obj->count * len), data, len);
       obj->list = tab;
       obj->count++;
       return VLC_SUCCESS;
   }
   
   # define VLC_PROBE_CONTINUE VLC_EGENERIC
   # define VLC_PROBE_STOP     VLC_SUCCESS
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif
