
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_renderer_discovery.h:

Program Listing for File vlc_renderer_discovery.h
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_renderer_discovery.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_renderer_discovery.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_renderer_discovery.h : Renderer Discovery functions
    *****************************************************************************
    * Copyright (C) 2016 VLC authors and VideoLAN
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
   
   #ifndef VLC_RENDERER_DISCOVERY_H
   #define VLC_RENDERER_DISCOVERY_H 1
   
   #include <vlc_input.h>
   #include <vlc_probe.h>
   #include <vlc_url.h>
   
   
   #define VLC_RENDERER_CAN_AUDIO 0x0001
   #define VLC_RENDERER_CAN_VIDEO 0x0002
   
   VLC_API vlc_renderer_item_t *
   vlc_renderer_item_new(const char *psz_type, const char *psz_name,
                         const char *psz_uri, const char *psz_extra_sout,
                         const char *psz_demux_filter, const char *psz_icon_uri,
                         int i_flags) VLC_USED;
   
   VLC_API vlc_renderer_item_t *
   vlc_renderer_item_hold(vlc_renderer_item_t *p_item);
   
   VLC_API void
   vlc_renderer_item_release(vlc_renderer_item_t *p_item);
   
   VLC_API const char *
   vlc_renderer_item_name(const vlc_renderer_item_t *p_item);
   
   VLC_API const char *
   vlc_renderer_item_type(const vlc_renderer_item_t *p_item);
   
   VLC_API const char *
   vlc_renderer_item_demux_filter(const vlc_renderer_item_t *p_item);
   
   VLC_API const char *
   vlc_renderer_item_sout(const vlc_renderer_item_t *p_item);
   
   VLC_API const char *
   vlc_renderer_item_icon_uri(const vlc_renderer_item_t *p_item);
   
   VLC_API int
   vlc_renderer_item_flags(const vlc_renderer_item_t *p_item);
   
   
   typedef struct vlc_renderer_discovery_sys vlc_renderer_discovery_sys;
   struct vlc_renderer_discovery_owner;
   
   VLC_API int
   vlc_rd_get_names(vlc_object_t *p_obj, char ***pppsz_names,
                    char ***pppsz_longnames) VLC_USED;
   #define vlc_rd_get_names(a, b, c) \
           vlc_rd_get_names(VLC_OBJECT(a), b, c)
   
   VLC_API vlc_renderer_discovery_t *
   vlc_rd_new(vlc_object_t *p_obj, const char *psz_name,
              const struct vlc_renderer_discovery_owner *owner) VLC_USED;
   
   VLC_API void vlc_rd_release(vlc_renderer_discovery_t *p_rd);
   
   
   struct vlc_renderer_discovery_owner
   {
       void *sys;
       void (*item_added)(struct vlc_renderer_discovery_t *,
                          struct vlc_renderer_item_t *);
       void (*item_removed)(struct vlc_renderer_discovery_t *,
                            struct vlc_renderer_item_t *);
   };
   
   struct vlc_renderer_discovery_t
   {
       VLC_COMMON_MEMBERS
       module_t *          p_module;
   
       struct vlc_renderer_discovery_owner owner;
   
       char *              psz_name;
       config_chain_t *    p_cfg;
   
       vlc_renderer_discovery_sys *p_sys;
   };
   
   static inline void vlc_rd_add_item(vlc_renderer_discovery_t * p_rd,
                                      vlc_renderer_item_t * p_item)
   {
       p_rd->owner.item_added(p_rd, p_item);
   }
   
   static inline void vlc_rd_remove_item(vlc_renderer_discovery_t * p_rd,
                                         vlc_renderer_item_t * p_item)
   {
       p_rd->owner.item_removed(p_rd, p_item);
   }
   
   VLC_API int
   vlc_rd_probe_add(vlc_probe_t *p_probe, const char *psz_name,
                    const char *psz_longname);
   
   #define VLC_RD_PROBE_HELPER(name, longname) \
   static int vlc_rd_probe_open(vlc_object_t *obj) \
   { \
       return vlc_rd_probe_add((struct vlc_probe_t *)obj, name, longname); \
   }
   
   #define VLC_RD_PROBE_SUBMODULE \
       add_submodule() \
           set_capability("renderer probe", 100) \
           set_callbacks(vlc_rd_probe_open, NULL)
   
   
   #endif
