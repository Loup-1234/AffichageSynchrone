
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_aout.h:

Program Listing for File vlc_aout.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_aout.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_aout.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_aout.h : audio output interface
    *****************************************************************************
    * Copyright (C) 2002-2011 VLC authors and VideoLAN
    *
    * Authors: Christophe Massiot <massiot@via.ecp.fr>
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
   
   #ifndef VLC_AOUT_H
   #define VLC_AOUT_H 1
   
   #include <assert.h>
   
   
   /* Buffers which arrive in advance of more than AOUT_MAX_ADVANCE_TIME
    * will be considered as bogus and be trashed */
   #define AOUT_MAX_ADVANCE_TIME           (AOUT_MAX_PREPARE_TIME + CLOCK_FREQ)
   
   /* Buffers which arrive in advance of more than AOUT_MAX_PREPARE_TIME
    * will cause the calling thread to sleep */
   #define AOUT_MAX_PREPARE_TIME           (2 * CLOCK_FREQ)
   
   /* Buffers which arrive after pts - AOUT_MIN_PREPARE_TIME will be trashed
    * to avoid too heavy resampling */
   #define AOUT_MIN_PREPARE_TIME           AOUT_MAX_PTS_ADVANCE
   
   /* Tolerance values from EBU Recommendation 37 */
   #define AOUT_MAX_PTS_ADVANCE            (CLOCK_FREQ / 25)
   
   #define AOUT_MAX_PTS_DELAY              (3 * CLOCK_FREQ / 50)
   
   /* Max acceptable resampling (in %) */
   #define AOUT_MAX_RESAMPLING             10
   
   #include "vlc_es.h"
   
   #define AOUT_FMTS_IDENTICAL( p_first, p_second ) (                          \
       ((p_first)->i_format == (p_second)->i_format)                           \
         && AOUT_FMTS_SIMILAR(p_first, p_second) )
   
   /* Check if i_rate == i_rate and i_channels == i_channels */
   #define AOUT_FMTS_SIMILAR( p_first, p_second ) (                            \
       ((p_first)->i_rate == (p_second)->i_rate)                               \
         && ((p_first)->channel_type == (p_second)->channel_type)            \
         && ((p_first)->i_physical_channels == (p_second)->i_physical_channels)\
         && ((p_first)->i_chan_mode == (p_second)->i_chan_mode) )
   
   #define AOUT_FMT_LINEAR( p_format ) \
       (aout_BitsPerSample((p_format)->i_format) != 0)
   
   #define VLC_CODEC_SPDIFL VLC_FOURCC('s','p','d','i')
   #define VLC_CODEC_SPDIFB VLC_FOURCC('s','p','d','b')
   
   #define AOUT_FMT_SPDIF( p_format ) \
       ( ((p_format)->i_format == VLC_CODEC_SPDIFL)       \
          || ((p_format)->i_format == VLC_CODEC_SPDIFB)   \
          || ((p_format)->i_format == VLC_CODEC_A52)      \
          || ((p_format)->i_format == VLC_CODEC_DTS) )
   
   #define AOUT_FMT_HDMI( p_format )                   \
       ( (p_format)->i_format == VLC_CODEC_EAC3        \
       ||(p_format)->i_format == VLC_CODEC_TRUEHD      \
       ||(p_format)->i_format == VLC_CODEC_MLP         \
       )
   
   /* Values used for the audio-channels object variable */
   #define AOUT_VAR_CHAN_UNSET         0 /* must be zero */
   #define AOUT_VAR_CHAN_STEREO        1
   #define AOUT_VAR_CHAN_RSTEREO       2
   #define AOUT_VAR_CHAN_LEFT          3
   #define AOUT_VAR_CHAN_RIGHT         4
   #define AOUT_VAR_CHAN_DOLBYS        5
   #define AOUT_VAR_CHAN_HEADPHONES    6
   #define AOUT_VAR_CHAN_MONO          7
   
   /*****************************************************************************
    * Main audio output structures
    *****************************************************************************/
   
   /* Size of a frame for S/PDIF output. */
   #define AOUT_SPDIF_SIZE 6144
   
   /* Number of samples in an A/52 frame. */
   #define A52_FRAME_NB 1536
   
   /* FIXME to remove once aout.h is cleaned a bit more */
   #include <vlc_block.h>
   
   struct audio_output
   {
       VLC_COMMON_MEMBERS
   
       struct aout_sys_t *sys; 
   
       int (*start)(audio_output_t *, audio_sample_format_t *fmt);
       void (*stop)(audio_output_t *);
       int (*time_get)(audio_output_t *, vlc_tick_t *delay);
       void (*play)(audio_output_t *, block_t *);
       void (*pause)( audio_output_t *, bool pause, vlc_tick_t date);
       void (*flush)( audio_output_t *, bool wait);
       int (*volume_set)(audio_output_t *, float volume);
       int (*mute_set)(audio_output_t *, bool mute);
       int (*device_select)(audio_output_t *, const char *id);
   
       struct {
           bool headphones; 
       } current_sink_info;
   
       struct {
           void (*volume_report)(audio_output_t *, float);
           void (*mute_report)(audio_output_t *, bool);
           void (*policy_report)(audio_output_t *, bool);
           void (*device_report)(audio_output_t *, const char *);
           void (*hotplug_report)(audio_output_t *, const char *, const char *);
           int (*gain_request)(audio_output_t *, float);
           void (*restart_request)(audio_output_t *, unsigned);
       } event;
   };
   
   typedef enum
   {
       AOUT_CHANIDX_DISABLE = -1,
       AOUT_CHANIDX_LEFT,
       AOUT_CHANIDX_RIGHT,
       AOUT_CHANIDX_MIDDLELEFT,
       AOUT_CHANIDX_MIDDLERIGHT,
       AOUT_CHANIDX_REARLEFT,
       AOUT_CHANIDX_REARRIGHT,
       AOUT_CHANIDX_REARCENTER,
       AOUT_CHANIDX_CENTER,
       AOUT_CHANIDX_LFE,
       AOUT_CHANIDX_MAX
   } vlc_chan_order_idx_t;
   
   static_assert(AOUT_CHANIDX_MAX == AOUT_CHAN_MAX, "channel count mismatch");
   
   #define AOUT_CHAN_REMAP_INIT { \
       AOUT_CHANIDX_LEFT,  \
       AOUT_CHANIDX_RIGHT, \
       AOUT_CHANIDX_MIDDLELEFT, \
       AOUT_CHANIDX_MIDDLERIGHT, \
       AOUT_CHANIDX_REARLEFT, \
       AOUT_CHANIDX_REARRIGHT, \
       AOUT_CHANIDX_REARCENTER, \
       AOUT_CHANIDX_CENTER, \
       AOUT_CHANIDX_LFE, \
   }
   
   static const uint32_t pi_vlc_chan_order_wg4[] =
   {
       AOUT_CHAN_LEFT, AOUT_CHAN_RIGHT,
       AOUT_CHAN_MIDDLELEFT, AOUT_CHAN_MIDDLERIGHT,
       AOUT_CHAN_REARLEFT, AOUT_CHAN_REARRIGHT, AOUT_CHAN_REARCENTER,
       AOUT_CHAN_CENTER, AOUT_CHAN_LFE, 0
   };
   
   #define AOUT_RESTART_FILTERS        0x1
   #define AOUT_RESTART_OUTPUT         (AOUT_RESTART_FILTERS|0x2)
   #define AOUT_RESTART_STEREOMODE     (AOUT_RESTART_OUTPUT|0x4)
   
   /*****************************************************************************
    * Prototypes
    *****************************************************************************/
   
   VLC_API unsigned aout_CheckChannelReorder( const uint32_t *, const uint32_t *,
                                              uint32_t mask, uint8_t *table );
   VLC_API void aout_ChannelReorder(void *, size_t, uint8_t, const uint8_t *, vlc_fourcc_t);
   
   VLC_API void aout_Interleave(void *dst, const void *const *planes,
                                unsigned samples, unsigned channels,
                                vlc_fourcc_t fourcc);
   VLC_API void aout_Deinterleave(void *dst, const void *src, unsigned samples,
                                unsigned channels, vlc_fourcc_t fourcc);
   
   VLC_API bool aout_CheckChannelExtraction( int *pi_selection, uint32_t *pi_layout, int *pi_channels, const uint32_t pi_order_dst[AOUT_CHAN_MAX], const uint32_t *pi_order_src, int i_channels );
   
   VLC_API void aout_ChannelExtract( void *p_dst, int i_dst_channels, const void *p_src, int i_src_channels, int i_sample_count, const int *pi_selection, int i_bits_per_sample );
   
   /* */
   static inline unsigned aout_FormatNbChannels(const audio_sample_format_t *fmt)
   {
       return popcount(fmt->i_physical_channels);
   }
   
   VLC_API unsigned int aout_BitsPerSample( vlc_fourcc_t i_format ) VLC_USED;
   VLC_API void aout_FormatPrepare( audio_sample_format_t * p_format );
   VLC_API void aout_FormatPrint(vlc_object_t *, const char *,
                                 const audio_sample_format_t *);
   #define aout_FormatPrint(o, t, f) aout_FormatPrint(VLC_OBJECT(o), t, f)
   VLC_API const char * aout_FormatPrintChannels( const audio_sample_format_t * ) VLC_USED;
   
   #define AOUT_VOLUME_DEFAULT             256
   #define AOUT_VOLUME_MAX                 512
   
   VLC_API float aout_VolumeGet (audio_output_t *);
   VLC_API int aout_VolumeSet (audio_output_t *, float);
   VLC_API int aout_VolumeUpdate (audio_output_t *, int, float *);
   VLC_API int aout_MuteGet (audio_output_t *);
   VLC_API int aout_MuteSet (audio_output_t *, bool);
   VLC_API char *aout_DeviceGet (audio_output_t *);
   VLC_API int aout_DeviceSet (audio_output_t *, const char *);
   VLC_API int aout_DevicesList (audio_output_t *, char ***, char ***);
   
   static inline void aout_VolumeReport(audio_output_t *aout, float volume)
   {
       aout->event.volume_report(aout, volume);
   }
   
   static inline void aout_MuteReport(audio_output_t *aout, bool mute)
   {
       aout->event.mute_report(aout, mute);
   }
   
   static inline void aout_PolicyReport(audio_output_t *aout, bool cork)
   {
       aout->event.policy_report(aout, cork);
   }
   
   static inline void aout_DeviceReport(audio_output_t *aout, const char *id)
   {
       aout->event.device_report(aout, id);
   }
   
   static inline void aout_HotplugReport(audio_output_t *aout,
                                         const char *id, const char *name)
   {
       aout->event.hotplug_report(aout, id, name);
   }
   
   static inline int aout_GainRequest(audio_output_t *aout, float gain)
   {
       return aout->event.gain_request(aout, gain);
   }
   
   static inline void aout_RestartRequest(audio_output_t *aout, unsigned mode)
   {
       aout->event.restart_request(aout, mode);
   }
   
   /* Audio output filters */
   
   typedef struct
   {
       int remap[AOUT_CHANIDX_MAX];
       bool headphones;
   } aout_filters_cfg_t;
   
   #define AOUT_FILTERS_CFG_INIT (aout_filters_cfg_t) \
       { .remap = AOUT_CHAN_REMAP_INIT, \
         .headphones = false, \
       };
   
   typedef struct aout_filters aout_filters_t;
   typedef struct aout_request_vout aout_request_vout_t;
   
   VLC_API aout_filters_t *aout_FiltersNew(vlc_object_t *,
                                           const audio_sample_format_t *,
                                           const audio_sample_format_t *,
                                           const aout_request_vout_t *,
                                           const aout_filters_cfg_t *cfg) VLC_USED;
   #define aout_FiltersNew(o,inf,outf,rv,remap) \
           aout_FiltersNew(VLC_OBJECT(o),inf,outf,rv,remap)
   VLC_API void aout_FiltersDelete(vlc_object_t *, aout_filters_t *);
   #define aout_FiltersDelete(o,f) \
           aout_FiltersDelete(VLC_OBJECT(o),f)
   VLC_API bool aout_FiltersAdjustResampling(aout_filters_t *, int);
   VLC_API block_t *aout_FiltersPlay(aout_filters_t *, block_t *, int rate);
   VLC_API block_t *aout_FiltersDrain(aout_filters_t *);
   VLC_API void     aout_FiltersFlush(aout_filters_t *);
   VLC_API void     aout_FiltersChangeViewpoint(aout_filters_t *, const vlc_viewpoint_t *vp);
   
   VLC_API vout_thread_t * aout_filter_RequestVout( filter_t *, vout_thread_t *p_vout, const video_format_t *p_fmt );
   
   
   #endif /* VLC_AOUT_H */
