
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_services_discovery.h:

Program Listing for File vlc_services_discovery.h
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_services_discovery.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_services_discovery.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_services_discovery.h : Services Discover functions
    *****************************************************************************
    * Copyright (C) 1999-2004 VLC authors and VideoLAN
    * $Id$
    *
    * Authors: Pierre d'Herbemont <pdherbemont # videolan.org>
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
   
   #ifndef VLC_SERVICES_DISCOVERY_H_
   #define VLC_SERVICES_DISCOVERY_H_
   
   #include <vlc_input.h>
   #include <vlc_probe.h>
   
   
   # ifdef __cplusplus
   extern "C" {
   # endif
   
   
   struct services_discovery_owner_t
   {
       void *sys; 
       void (*item_added)(struct services_discovery_t *sd, input_item_t *parent,
                          input_item_t *item, const char *category);
       void (*item_removed)(struct services_discovery_t *sd, input_item_t *item);
   };
   
   struct services_discovery_t
   {
       VLC_COMMON_MEMBERS
       module_t *          p_module;             
   
       char *psz_name;                           
       config_chain_t *p_cfg;                    
   
       const char *description; 
   
       int ( *pf_control ) ( services_discovery_t *, int, va_list );
   
       services_discovery_sys_t *p_sys;          
   
       struct services_discovery_owner_t owner; 
   };
   
   enum services_discovery_category_e
   {
       SD_CAT_DEVICES = 1,           
       SD_CAT_LAN,                   
       SD_CAT_INTERNET,              
       SD_CAT_MYCOMPUTER             
   };
   
   enum services_discovery_command_e
   {
       SD_CMD_SEARCH = 1,          
       SD_CMD_DESCRIPTOR           
   };
   
   enum services_discovery_capability_e
   {
       SD_CAP_SEARCH = 1           
   };
   
   typedef struct
   {
       char *psz_short_desc;       
       char *psz_icon_url;         
       char *psz_url;              
       int   i_capabilities;       
   } services_discovery_descriptor_t;
   
   
   /***********************************************************************
    * Service Discovery
    ***********************************************************************/
   
   static inline int vlc_sd_control( services_discovery_t *p_sd, int i_control, va_list args )
   {
       if( p_sd->pf_control )
           return p_sd->pf_control( p_sd, i_control, args );
       else
           return VLC_EGENERIC;
   }
   
   /* Get the services discovery modules names to use in Create(), in a null
    * terminated string array. Array and string must be freed after use. */
   VLC_API char ** vlc_sd_GetNames( vlc_object_t *, char ***, int ** ) VLC_USED;
   #define vlc_sd_GetNames(obj, pln, pcat ) \
           vlc_sd_GetNames(VLC_OBJECT(obj), pln, pcat)
   
   VLC_API services_discovery_t *vlc_sd_Create(vlc_object_t *parent,
       const char *chain, const struct services_discovery_owner_t *owner)
   VLC_USED;
   
   VLC_API void vlc_sd_Destroy( services_discovery_t * );
   
   static inline void services_discovery_AddItem(services_discovery_t *sd,
                                                 input_item_t *item)
   {
       sd->owner.item_added(sd, NULL, item, NULL);
   }
   
   static inline void services_discovery_AddSubItem(services_discovery_t *sd,
                                                    input_item_t *parent,
                                                    input_item_t *item)
   {
       sd->owner.item_added(sd, parent, item, NULL);
   }
   
   VLC_DEPRECATED
   static inline void services_discovery_AddItemCat(services_discovery_t *sd,
                                                    input_item_t *item,
                                                    const char *category)
   {
       sd->owner.item_added(sd, NULL, item, category);
   }
   
   static inline void services_discovery_RemoveItem(services_discovery_t *sd,
                                                    input_item_t *item)
   {
       sd->owner.item_removed(sd, item);
   }
   
   /* SD probing */
   
   VLC_API int vlc_sd_probe_Add(vlc_probe_t *, const char *, const char *, int category);
   
   #define VLC_SD_PROBE_SUBMODULE \
       add_submodule() \
           set_capability( "services probe", 100 ) \
           set_callbacks( vlc_sd_probe_Open, NULL )
   
   #define VLC_SD_PROBE_HELPER(name, longname, cat) \
   static int vlc_sd_probe_Open (vlc_object_t *obj) \
   { \
       return vlc_sd_probe_Add ((struct vlc_probe_t *)obj, name, \
                                longname, cat); \
   }
   
   # ifdef __cplusplus
   }
   # endif
   
   #endif
