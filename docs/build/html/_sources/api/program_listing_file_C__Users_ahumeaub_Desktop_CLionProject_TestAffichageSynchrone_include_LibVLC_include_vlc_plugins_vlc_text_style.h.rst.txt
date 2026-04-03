
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_text_style.h:

Program Listing for File vlc_text_style.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_text_style.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_text_style.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_text_style.h: text_style_t definition and helpers.
    *****************************************************************************
    * Copyright (C) 1999-2010 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Derk-Jan Hartman <hartman _AT_ videolan _DOT_ org>
    *          basOS G <noxelia 4t gmail , com>
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
   
   #ifndef VLC_TEXT_STYLE_H
   #define VLC_TEXT_STYLE_H 1
   
   #ifdef __cplusplus
   extern "C" {
   #endif
   
   typedef struct
   {
       /* Family font names */
       char *     psz_fontname;      
       char *     psz_monofontname;  
   
       uint16_t   i_features;        
       uint16_t   i_style_flags;     
   
       /* Font style */
       float      f_font_relsize;    
       int        i_font_size;       
       int        i_font_color;      
       uint8_t    i_font_alpha;      
       int        i_spacing;         
   
       /* Outline */
       int        i_outline_color;   
       uint8_t    i_outline_alpha;   
       int        i_outline_width;   
   
       /* Shadow */
       int        i_shadow_color;    
       uint8_t    i_shadow_alpha;    
       int        i_shadow_width;    
   
       /* Background (and karaoke) */
       int        i_background_color;
       uint8_t    i_background_alpha;
       int        i_karaoke_background_color;
       uint8_t    i_karaoke_background_alpha;
   
       /* Line breaking */
       enum
       {
           STYLE_WRAP_DEFAULT = 0,   
           STYLE_WRAP_CHAR,          
           STYLE_WRAP_NONE,          
       } e_wrapinfo;
   } text_style_t;
   
   #define STYLE_ALPHA_OPAQUE      0xFF
   #define STYLE_ALPHA_TRANSPARENT 0x00
   
   /* Features flags for \ref i_features */
   #define STYLE_NO_DEFAULTS               0x0
   #define STYLE_FULLY_SET                 0xFFFF
   #define STYLE_HAS_FONT_COLOR            (1 << 0)
   #define STYLE_HAS_FONT_ALPHA            (1 << 1)
   #define STYLE_HAS_FLAGS                 (1 << 2)
   #define STYLE_HAS_OUTLINE_COLOR         (1 << 3)
   #define STYLE_HAS_OUTLINE_ALPHA         (1 << 4)
   #define STYLE_HAS_SHADOW_COLOR          (1 << 5)
   #define STYLE_HAS_SHADOW_ALPHA          (1 << 6)
   #define STYLE_HAS_BACKGROUND_COLOR      (1 << 7)
   #define STYLE_HAS_BACKGROUND_ALPHA      (1 << 8)
   #define STYLE_HAS_K_BACKGROUND_COLOR    (1 << 9)
   #define STYLE_HAS_K_BACKGROUND_ALPHA    (1 << 10)
   #define STYLE_HAS_WRAP_INFO             (1 << 11)
   
   /* Style flags for \ref text_style_t */
   #define STYLE_BOLD              (1 << 0)
   #define STYLE_ITALIC            (1 << 1)
   #define STYLE_OUTLINE           (1 << 2)
   #define STYLE_SHADOW            (1 << 3)
   #define STYLE_BACKGROUND        (1 << 4)
   #define STYLE_UNDERLINE         (1 << 5)
   #define STYLE_STRIKEOUT         (1 << 6)
   #define STYLE_HALFWIDTH         (1 << 7)
   #define STYLE_MONOSPACED        (1 << 8)
   #define STYLE_DOUBLEWIDTH       (1 << 9)
   #define STYLE_BLINK_FOREGROUND  (1 << 10)
   #define STYLE_BLINK_BACKGROUND  (1 << 11)
   
   #define STYLE_DEFAULT_FONT_SIZE 20
   #define STYLE_DEFAULT_REL_FONT_SIZE 6.25
   
   
   typedef struct text_segment_t text_segment_t;
   struct text_segment_t {
       char *psz_text;                   
       text_style_t *style;              
       text_segment_t *p_next;           
   };
   
   VLC_API text_style_t * text_style_New( void );
   
   VLC_API text_style_t * text_style_Create( int );
   
   VLC_API text_style_t * text_style_Copy( text_style_t *, const text_style_t * );
   
   VLC_API text_style_t * text_style_Duplicate( const text_style_t * );
   
   VLC_API void text_style_Merge( text_style_t *, const text_style_t *, bool b_override );
   
   VLC_API void text_style_Delete( text_style_t * );
   
   VLC_API text_segment_t *text_segment_New( const char * );
   
   VLC_API text_segment_t *text_segment_NewInheritStyle( const text_style_t* p_style );
   
   VLC_API void text_segment_Delete( text_segment_t * );
   
   VLC_API void text_segment_ChainDelete( text_segment_t * );
   
   VLC_API text_segment_t * text_segment_Copy( text_segment_t * );
   
   static const struct {
       const char *psz_name;
       uint32_t   i_value;
   } p_html_colors[] = {
       /* Official html colors */
       { "Aqua",    0x00FFFF },
       { "Black",   0x000000 },
       { "Blue",    0x0000FF },
       { "Fuchsia", 0xFF00FF },
       { "Gray",    0x808080 },
       { "Green",   0x008000 },
       { "Lime",    0x00FF00 },
       { "Maroon",  0x800000 },
       { "Navy",    0x000080 },
       { "Olive",   0x808000 },
       { "Purple",  0x800080 },
       { "Red",     0xFF0000 },
       { "Silver",  0xC0C0C0 },
       { "Teal",    0x008080 },
       { "White",   0xFFFFFF },
       { "Yellow",  0xFFFF00 },
   
       /* Common ones */
       { "AliceBlue", 0xF0F8FF },
       { "AntiqueWhite", 0xFAEBD7 },
       { "Aqua", 0x00FFFF },
       { "Aquamarine", 0x7FFFD4 },
       { "Azure", 0xF0FFFF },
       { "Beige", 0xF5F5DC },
       { "Bisque", 0xFFE4C4 },
       { "Black", 0x000000 },
       { "BlanchedAlmond", 0xFFEBCD },
       { "Blue", 0x0000FF },
       { "BlueViolet", 0x8A2BE2 },
       { "Brown", 0xA52A2A },
       { "BurlyWood", 0xDEB887 },
       { "CadetBlue", 0x5F9EA0 },
       { "Chartreuse", 0x7FFF00 },
       { "Chocolate", 0xD2691E },
       { "Coral", 0xFF7F50 },
       { "CornflowerBlue", 0x6495ED },
       { "Cornsilk", 0xFFF8DC },
       { "Crimson", 0xDC143C },
       { "Cyan", 0x00FFFF },
       { "DarkBlue", 0x00008B },
       { "DarkCyan", 0x008B8B },
       { "DarkGoldenRod", 0xB8860B },
       { "DarkGray", 0xA9A9A9 },
       { "DarkGrey", 0xA9A9A9 },
       { "DarkGreen", 0x006400 },
       { "DarkKhaki", 0xBDB76B },
       { "DarkMagenta", 0x8B008B },
       { "DarkOliveGreen", 0x556B2F },
       { "Darkorange", 0xFF8C00 },
       { "DarkOrchid", 0x9932CC },
       { "DarkRed", 0x8B0000 },
       { "DarkSalmon", 0xE9967A },
       { "DarkSeaGreen", 0x8FBC8F },
       { "DarkSlateBlue", 0x483D8B },
       { "DarkSlateGray", 0x2F4F4F },
       { "DarkSlateGrey", 0x2F4F4F },
       { "DarkTurquoise", 0x00CED1 },
       { "DarkViolet", 0x9400D3 },
       { "DeepPink", 0xFF1493 },
       { "DeepSkyBlue", 0x00BFFF },
       { "DimGray", 0x696969 },
       { "DimGrey", 0x696969 },
       { "DodgerBlue", 0x1E90FF },
       { "FireBrick", 0xB22222 },
       { "FloralWhite", 0xFFFAF0 },
       { "ForestGreen", 0x228B22 },
       { "Fuchsia", 0xFF00FF },
       { "Gainsboro", 0xDCDCDC },
       { "GhostWhite", 0xF8F8FF },
       { "Gold", 0xFFD700 },
       { "GoldenRod", 0xDAA520 },
       { "Gray", 0x808080 },
       { "Grey", 0x808080 },
       { "Green", 0x008000 },
       { "GreenYellow", 0xADFF2F },
       { "HoneyDew", 0xF0FFF0 },
       { "HotPink", 0xFF69B4 },
       { "IndianRed", 0xCD5C5C },
       { "Indigo", 0x4B0082 },
       { "Ivory", 0xFFFFF0 },
       { "Khaki", 0xF0E68C },
       { "Lavender", 0xE6E6FA },
       { "LavenderBlush", 0xFFF0F5 },
       { "LawnGreen", 0x7CFC00 },
       { "LemonChiffon", 0xFFFACD },
       { "LightBlue", 0xADD8E6 },
       { "LightCoral", 0xF08080 },
       { "LightCyan", 0xE0FFFF },
       { "LightGoldenRodYellow", 0xFAFAD2 },
       { "LightGray", 0xD3D3D3 },
       { "LightGrey", 0xD3D3D3 },
       { "LightGreen", 0x90EE90 },
       { "LightPink", 0xFFB6C1 },
       { "LightSalmon", 0xFFA07A },
       { "LightSeaGreen", 0x20B2AA },
       { "LightSkyBlue", 0x87CEFA },
       { "LightSlateGray", 0x778899 },
       { "LightSlateGrey", 0x778899 },
       { "LightSteelBlue", 0xB0C4DE },
       { "LightYellow", 0xFFFFE0 },
       { "Lime", 0x00FF00 },
       { "LimeGreen", 0x32CD32 },
       { "Linen", 0xFAF0E6 },
       { "Magenta", 0xFF00FF },
       { "Maroon", 0x800000 },
       { "MediumAquaMarine", 0x66CDAA },
       { "MediumBlue", 0x0000CD },
       { "MediumOrchid", 0xBA55D3 },
       { "MediumPurple", 0x9370D8 },
       { "MediumSeaGreen", 0x3CB371 },
       { "MediumSlateBlue", 0x7B68EE },
       { "MediumSpringGreen", 0x00FA9A },
       { "MediumTurquoise", 0x48D1CC },
       { "MediumVioletRed", 0xC71585 },
       { "MidnightBlue", 0x191970 },
       { "MintCream", 0xF5FFFA },
       { "MistyRose", 0xFFE4E1 },
       { "Moccasin", 0xFFE4B5 },
       { "NavajoWhite", 0xFFDEAD },
       { "Navy", 0x000080 },
       { "OldLace", 0xFDF5E6 },
       { "Olive", 0x808000 },
       { "OliveDrab", 0x6B8E23 },
       { "Orange", 0xFFA500 },
       { "OrangeRed", 0xFF4500 },
       { "Orchid", 0xDA70D6 },
       { "PaleGoldenRod", 0xEEE8AA },
       { "PaleGreen", 0x98FB98 },
       { "PaleTurquoise", 0xAFEEEE },
       { "PaleVioletRed", 0xD87093 },
       { "PapayaWhip", 0xFFEFD5 },
       { "PeachPuff", 0xFFDAB9 },
       { "Peru", 0xCD853F },
       { "Pink", 0xFFC0CB },
       { "Plum", 0xDDA0DD },
       { "PowderBlue", 0xB0E0E6 },
       { "Purple", 0x800080 },
       { "RebeccaPurple", 0x663399 },
       { "Red", 0xFF0000 },
       { "RosyBrown", 0xBC8F8F },
       { "RoyalBlue", 0x4169E1 },
       { "SaddleBrown", 0x8B4513 },
       { "Salmon", 0xFA8072 },
       { "SandyBrown", 0xF4A460 },
       { "SeaGreen", 0x2E8B57 },
       { "SeaShell", 0xFFF5EE },
       { "Sienna", 0xA0522D },
       { "Silver", 0xC0C0C0 },
       { "SkyBlue", 0x87CEEB },
       { "SlateBlue", 0x6A5ACD },
       { "SlateGray", 0x708090 },
       { "SlateGrey", 0x708090 },
       { "Snow", 0xFFFAFA },
       { "SpringGreen", 0x00FF7F },
       { "SteelBlue", 0x4682B4 },
       { "Tan", 0xD2B48C },
       { "Teal", 0x008080 },
       { "Thistle", 0xD8BFD8 },
       { "Tomato", 0xFF6347 },
       { "Turquoise", 0x40E0D0 },
       { "Violet", 0xEE82EE },
       { "Wheat", 0xF5DEB3 },
       { "White", 0xFFFFFF },
       { "WhiteSmoke", 0xF5F5F5 },
       { "Yellow", 0xFFFF00 },
       { "YellowGreen", 0x9ACD32 },
   
       { NULL, 0 }
   };
   
   VLC_API unsigned int vlc_html_color( const char *psz_value, bool* ok );
   
   #ifdef __cplusplus
   }
   #endif
   
   #endif /* VLC_TEXT_STYLE_H */
   
