
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_epg.h:

Program Listing for File vlc_epg.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_epg.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_epg.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_epg.h: Electronic Program Guide
    *****************************************************************************
    * Copyright (C) 2007 VLC authors and VideoLAN
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
   
   #ifndef VLC_EPG_H
   #define VLC_EPG_H 1
   
   
   typedef struct
   {
       int64_t  i_start;    /* Interpreted as a value return by time() */
       uint32_t i_duration; /* Duration of the event in second */
       uint16_t i_id;       /* Unique event id withing the event set */
   
       char    *psz_name;
       char    *psz_short_description;
       char    *psz_description;
       struct               /* Description items in tranmission order */
       {
           char *psz_key;
           char *psz_value;
       } * description_items;
       int i_description_items;
   
       uint8_t i_rating;   /* Parental control, set to 0 when undefined */
   } vlc_epg_event_t;
   
   typedef struct
   {
       char            *psz_name;
       uint32_t         i_id;       /* Unique identifier for this table / events (partial sets) */
       uint16_t         i_source_id;/* Channel / Program reference id this epg relates to */
       size_t            i_event;
       vlc_epg_event_t **pp_event;
       bool             b_present;  /* Contains present/following or similar, and sets below */
       const vlc_epg_event_t *p_current; /* NULL, or equal to one of the entries in pp_event */
   } vlc_epg_t;
   
   VLC_API vlc_epg_event_t * vlc_epg_event_New(uint16_t i_id,
                                               int64_t i_start, uint32_t i_duration);
   
   VLC_API void vlc_epg_event_Delete(vlc_epg_event_t *p_event);
   
   VLC_API vlc_epg_event_t * vlc_epg_event_Duplicate(const vlc_epg_event_t *p_src);
   
   VLC_API vlc_epg_t * vlc_epg_New(uint32_t i_id, uint16_t i_source_id);
   
   VLC_API void vlc_epg_Delete(vlc_epg_t *p_epg);
   
   VLC_API bool vlc_epg_AddEvent(vlc_epg_t *p_epg, vlc_epg_event_t *p_evt);
   
   VLC_API void vlc_epg_SetCurrent(vlc_epg_t *p_epg, int64_t i_start);
   
   VLC_API vlc_epg_t * vlc_epg_Duplicate(const vlc_epg_t *p_src);
   
   #endif
   
