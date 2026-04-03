
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_picture.h:

Program Listing for File vlc_picture.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_picture.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_picture.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_picture.h: picture definitions
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
   
   #ifndef VLC_PICTURE_H
   #define VLC_PICTURE_H 1
   
   
   #include <vlc_es.h>
   
   typedef struct plane_t
   {
       uint8_t *p_pixels;                        
   
       /* Variables used for fast memcpy operations */
       int i_lines;           
       int i_pitch;           
   
       int i_pixel_pitch;
   
       /* Variables used for pictures with margins */
       int i_visible_lines;            
       int i_visible_pitch;            
   
   } plane_t;
   
   #define PICTURE_PLANE_MAX (VOUT_MAX_PLANES)
   
   typedef struct picture_context_t
   {
       void (*destroy)(struct picture_context_t *);
       struct picture_context_t *(*copy)(struct picture_context_t *);
   } picture_context_t;
   
   struct picture_t
   {
       video_frame_format_t format;
   
       plane_t         p[PICTURE_PLANE_MAX];     
       int             i_planes;                
   
       vlc_tick_t      date;                                  
       bool            b_force;
   
       bool            b_progressive;          
       bool            b_top_field_first;             
       unsigned int    i_nb_fields;                  
       picture_context_t *context;      
   
       picture_sys_t * p_sys;
   
       struct picture_t *p_next;
   };
   
   VLC_API picture_t * picture_New( vlc_fourcc_t i_chroma, int i_width, int i_height, int i_sar_num, int i_sar_den ) VLC_USED;
   
   VLC_API picture_t * picture_NewFromFormat( const video_format_t *p_fmt ) VLC_USED;
   
   typedef struct
   {
       picture_sys_t *p_sys;
       void (*pf_destroy)(picture_t *);
   
       /* Plane resources
        * XXX all fields MUST be set to the right value.
        */
       struct
       {
           uint8_t *p_pixels;  
           int i_lines;        
           int i_pitch;        
       } p[PICTURE_PLANE_MAX];
   
   } picture_resource_t;
   
   VLC_API picture_t * picture_NewFromResource( const video_format_t *, const picture_resource_t * ) VLC_USED;
   
   VLC_API picture_t *picture_Hold( picture_t *p_picture );
   
   VLC_API void picture_Release( picture_t *p_picture );
   
   VLC_API void picture_CopyProperties( picture_t *p_dst, const picture_t *p_src );
   
   VLC_API void picture_Reset( picture_t * );
   
   VLC_API void picture_CopyPixels( picture_t *p_dst, const picture_t *p_src );
   VLC_API void plane_CopyPixels( plane_t *p_dst, const plane_t *p_src );
   
   VLC_API void picture_Copy( picture_t *p_dst, const picture_t *p_src );
   
   VLC_API picture_t *picture_Clone(picture_t *pic);
   
   VLC_API int picture_Export( vlc_object_t *p_obj, block_t **pp_image, video_format_t *p_fmt, picture_t *p_picture, vlc_fourcc_t i_format, int i_override_width, int i_override_height );
   
   VLC_API int picture_Setup( picture_t *, const video_format_t * );
   
   
   /*****************************************************************************
    * Shortcuts to access image components
    *****************************************************************************/
   
   /* Plane indices */
   enum
   {
       Y_PLANE = 0,
       U_PLANE = 1,
       V_PLANE = 2,
       A_PLANE = 3,
   };
   
   /* Shortcuts */
   #define Y_PIXELS     p[Y_PLANE].p_pixels
   #define Y_PITCH      p[Y_PLANE].i_pitch
   #define U_PIXELS     p[U_PLANE].p_pixels
   #define U_PITCH      p[U_PLANE].i_pitch
   #define V_PIXELS     p[V_PLANE].p_pixels
   #define V_PITCH      p[V_PLANE].i_pitch
   #define A_PIXELS     p[A_PLANE].p_pixels
   #define A_PITCH      p[A_PLANE].i_pitch
   
   
   #endif /* VLC_PICTURE_H */
