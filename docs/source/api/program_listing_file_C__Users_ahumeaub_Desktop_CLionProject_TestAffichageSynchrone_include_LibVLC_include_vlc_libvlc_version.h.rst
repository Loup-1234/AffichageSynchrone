
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_version.h:

Program Listing for File libvlc_version.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_version.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\libvlc_version.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * libvlc_version.h
    *****************************************************************************
    * Copyright (C) 2010 Rémi Denis-Courmont
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
   
   
   #ifndef LIBVLC_VERSION_H
   # define LIBVLC_VERSION_H 1
   
   # define LIBVLC_VERSION_MAJOR    (3)
   
   # define LIBVLC_VERSION_MINOR    (0)
   
   # define LIBVLC_VERSION_REVISION (23)
   
   # define LIBVLC_VERSION_EXTRA    (0)
   
   # define LIBVLC_VERSION(maj,min,rev,extra) \
            ((maj << 24) | (min << 16) | (rev << 8) | (extra))
   
   # define LIBVLC_VERSION_INT \
            LIBVLC_VERSION(LIBVLC_VERSION_MAJOR, LIBVLC_VERSION_MINOR, \
                           LIBVLC_VERSION_REVISION, LIBVLC_VERSION_EXTRA)
   
   #endif
