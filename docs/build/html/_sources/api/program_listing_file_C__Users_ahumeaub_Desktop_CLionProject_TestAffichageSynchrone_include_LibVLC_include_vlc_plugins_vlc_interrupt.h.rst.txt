
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_interrupt.h:

Program Listing for File vlc_interrupt.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_interrupt.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_interrupt.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_interrupt.h:
    *****************************************************************************
    * Copyright (C) 2015 Remlab T:mi
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
   
   
   #ifndef VLC_INTERRUPT_H
   # define VLC_INTERRUPT_H 1
   # include <vlc_threads.h>
   # ifndef _WIN32
   #  include <sys/socket.h> /* socklen_t */
   # else
   #  include <ws2tcpip.h>
   # endif
   
   struct pollfd;
   struct iovec;
   struct sockaddr;
   struct msghdr;
   
   
   VLC_API int vlc_sem_wait_i11e(vlc_sem_t *);
   
   VLC_API int vlc_mwait_i11e(vlc_tick_t);
   
   static inline int vlc_msleep_i11e(vlc_tick_t delay)
   {
       return vlc_mwait_i11e(mdate() + delay);
   }
   
   VLC_API int vlc_poll_i11e(struct pollfd *, unsigned, int);
   
   VLC_API ssize_t vlc_readv_i11e(int fd, struct iovec *, int);
   VLC_API ssize_t vlc_writev_i11e(int fd, const struct iovec *, int);
   VLC_API ssize_t vlc_read_i11e(int fd, void *, size_t);
   VLC_API ssize_t vlc_write_i11e(int fd, const void *, size_t);
   
   VLC_API ssize_t vlc_recvmsg_i11e(int fd, struct msghdr *, int flags);
   VLC_API ssize_t vlc_sendmsg_i11e(int fd, const struct msghdr *, int flags);
   
   VLC_API ssize_t vlc_recvfrom_i11e(int fd, void *, size_t, int flags,
                                   struct sockaddr *, socklen_t *);
   VLC_API ssize_t vlc_sendto_i11e(int fd, const void *, size_t, int flags,
                                 const struct sockaddr *, socklen_t);
   
   static inline ssize_t vlc_recv_i11e(int fd, void *buf, size_t len, int flags)
   {
       return vlc_recvfrom_i11e(fd, buf, len, flags, NULL, NULL);
   }
   
   static inline
   ssize_t vlc_send_i11e(int fd, const void *buf, size_t len, int flags)
   {
       return vlc_sendto_i11e(fd, buf, len, flags, NULL, 0);
   }
   
   VLC_API int vlc_accept_i11e(int fd, struct sockaddr *, socklen_t *, bool);
   
   VLC_API void vlc_interrupt_register(void (*cb)(void *), void *opaque);
   
   VLC_API int vlc_interrupt_unregister(void);
   
   typedef struct vlc_interrupt vlc_interrupt_t;
   
   VLC_API vlc_interrupt_t *vlc_interrupt_create(void) VLC_USED;
   
   VLC_API void vlc_interrupt_destroy(vlc_interrupt_t *);
   
   VLC_API vlc_interrupt_t *vlc_interrupt_set(vlc_interrupt_t *);
   
   VLC_API void vlc_interrupt_raise(vlc_interrupt_t *);
   
   VLC_API void vlc_interrupt_kill(vlc_interrupt_t *);
   
   VLC_API bool vlc_killed(void) VLC_USED;
   
   VLC_API void vlc_interrupt_forward_start(vlc_interrupt_t *to,
                                            void *data[2]);
   
   VLC_API int vlc_interrupt_forward_stop(void *const data[2]);
   
   #endif
