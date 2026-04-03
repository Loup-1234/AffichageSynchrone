
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_subpicture.h:

Program Listing for File vlc_subpicture.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_subpicture.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_subpicture.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_subpicture.h: subpicture definitions
    *****************************************************************************
    * Copyright (C) 1999 - 2009 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Vincent Seguin <seguin@via.ecp.fr>
    *          Samuel Hocevar <sam@via.ecp.fr>
    *          Olivier Aubert <oaubert 47 videolan d07 org>
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
   
   #ifndef VLC_SUBPICTURE_H
   #define VLC_SUBPICTURE_H 1
   
   
   #include <vlc_picture.h>
   #include <vlc_text_style.h>
   
   
   typedef struct subpicture_region_private_t subpicture_region_private_t;
   
   struct subpicture_region_t
   {
       video_format_t  fmt;                          
       picture_t       *p_picture;          
   
       int             i_x;      
       int             i_y;      
       int             i_align;                  
       int             i_alpha;                               
   
       /* Parameters for text regions (p_picture to be rendered) */
       text_segment_t  *p_text;         
       int             i_text_align;    
       bool            b_noregionbg;    
       bool            b_gridmode;      
       bool            b_balanced_text; 
       int             i_max_width;     
       int             i_max_height;    
   
       subpicture_region_t *p_next;                
       subpicture_region_private_t *p_private;  
   };
   
   /* Subpicture region position flags */
   #define SUBPICTURE_ALIGN_LEFT       0x1
   #define SUBPICTURE_ALIGN_RIGHT      0x2
   #define SUBPICTURE_ALIGN_TOP        0x4
   #define SUBPICTURE_ALIGN_BOTTOM     0x8
   #define SUBPICTURE_ALIGN_MASK ( SUBPICTURE_ALIGN_LEFT|SUBPICTURE_ALIGN_RIGHT| \
                                   SUBPICTURE_ALIGN_TOP |SUBPICTURE_ALIGN_BOTTOM )
   VLC_API subpicture_region_t * subpicture_region_New( const video_format_t *p_fmt );
   
   VLC_API void subpicture_region_Delete( subpicture_region_t *p_region );
   
   VLC_API void subpicture_region_ChainDelete( subpicture_region_t *p_head );
   
   VLC_API subpicture_region_t *subpicture_region_Copy( subpicture_region_t *p_region );
   
   typedef struct subpicture_updater_sys_t subpicture_updater_sys_t;
   typedef struct
   {
       int  (*pf_validate)( subpicture_t *,
                            bool has_src_changed, const video_format_t *p_fmt_src,
                            bool has_dst_changed, const video_format_t *p_fmt_dst,
                            vlc_tick_t);
       void (*pf_update)  ( subpicture_t *,
                            const video_format_t *p_fmt_src,
                            const video_format_t *p_fmt_dst,
                            vlc_tick_t );
       void (*pf_destroy) ( subpicture_t * );
       subpicture_updater_sys_t *p_sys;
   } subpicture_updater_t;
   
   typedef struct subpicture_private_t subpicture_private_t;
   
   struct subpicture_t
   {
       int             i_channel;                    
   
       int64_t         i_order;                 
       subpicture_t *  p_next;               
   
       subpicture_region_t *p_region;  
   
       vlc_tick_t      i_start;                  
       vlc_tick_t      i_stop;                         
       bool            b_ephemer;    
       bool            b_fade;                               
   
       bool         b_subtitle;            
       bool         b_absolute;                       
       int          i_original_picture_width;  
       int          i_original_picture_height;
       int          i_alpha;                                  
   
       subpicture_updater_t updater;
   
       subpicture_private_t *p_private;    /* Reserved to the core */
   };
   
   VLC_API subpicture_t * subpicture_New( const subpicture_updater_t * );
   
   VLC_API void subpicture_Delete( subpicture_t *p_subpic );
   
   VLC_API subpicture_t * subpicture_NewFromPicture( vlc_object_t *, picture_t *, vlc_fourcc_t i_chroma );
   
   VLC_API void subpicture_Update( subpicture_t *, const video_format_t *src, const video_format_t *, vlc_tick_t );
   
   VLC_API unsigned picture_BlendSubpicture( picture_t *, filter_t *p_blend, subpicture_t * );
   
   
   #endif /* _VLC_VIDEO_H */
