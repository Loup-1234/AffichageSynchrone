
.. _program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_tls.h:

Program Listing for File vlc_tls.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_tls.h>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_tls.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*****************************************************************************
    * vlc_tls.h:
    *****************************************************************************
    * Copyright (C) 2004-2016 Rémi Denis-Courmont
    * Copyright (C) 2005-2006 VLC authors and VideoLAN
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
   
   #ifndef VLC_TLS_H
   # define VLC_TLS_H
   
   
   # include <vlc_network.h>
   
   typedef struct vlc_tls
   {
       int (*get_fd)(struct vlc_tls *);
       ssize_t (*readv)(struct vlc_tls *, struct iovec *, unsigned);
       ssize_t (*writev)(struct vlc_tls *, const struct iovec *, unsigned);
       int (*shutdown)(struct vlc_tls *, bool duplex);
       void (*close)(struct vlc_tls *);
   
       struct vlc_tls *p;
   } vlc_tls_t;
   
   
   typedef struct vlc_tls_creds
   {
       VLC_COMMON_MEMBERS
   
       module_t *module;
       void *sys;
   
       vlc_tls_t *(*open)(struct vlc_tls_creds *, vlc_tls_t *sock,
                          const char *host, const char *const *alpn);
       int  (*handshake)(struct vlc_tls_creds *, vlc_tls_t *session,
                         const char *hostname, const char *service,
                         char ** /*restrict*/ alp);
   } vlc_tls_creds_t;
   
   VLC_API vlc_tls_creds_t *vlc_tls_ClientCreate(vlc_object_t *);
   
   VLC_API vlc_tls_creds_t *vlc_tls_ServerCreate(vlc_object_t *, const char *cert,
                                                 const char *key);
   
   static inline int vlc_tls_SessionHandshake (vlc_tls_creds_t *crd,
                                               vlc_tls_t *tls)
   {
       return crd->handshake(crd, tls, NULL, NULL, NULL);
   }
   
   VLC_API void vlc_tls_Delete(vlc_tls_creds_t *);
   
   VLC_API vlc_tls_t *vlc_tls_ClientSessionCreate(vlc_tls_creds_t *creds,
                                                  vlc_tls_t *sock,
                                                  const char *host,
                                                  const char *service,
                                                  const char *const *alpn,
                                                  char **alp);
   
   VLC_API vlc_tls_t *vlc_tls_ServerSessionCreate(vlc_tls_creds_t *creds,
                                                  vlc_tls_t *sock,
                                                  const char *const *alpn);
   
   
   VLC_API void vlc_tls_SessionDelete (vlc_tls_t *);
   
   static inline int vlc_tls_GetFD(vlc_tls_t *tls)
   {
       return tls->get_fd(tls);
   }
   
   VLC_API ssize_t vlc_tls_Read(vlc_tls_t *, void *buf, size_t len, bool waitall);
   
   VLC_API char *vlc_tls_GetLine(vlc_tls_t *);
   
   VLC_API ssize_t vlc_tls_Write(vlc_tls_t *, const void *buf, size_t len);
   
   static inline int vlc_tls_Shutdown(vlc_tls_t *tls, bool duplex)
   {
       return tls->shutdown(tls, duplex);
   }
   
   static inline void vlc_tls_Close(vlc_tls_t *session)
   {
       do
       {
           vlc_tls_t *p = session->p;
   
           vlc_tls_SessionDelete(session);
           session = p;
       }
       while (session != NULL);
   }
   
   VLC_API vlc_tls_t *vlc_tls_SocketOpen(int fd);
   
   VLC_API int vlc_tls_SocketPair(int family, int protocol, vlc_tls_t *[2]);
   
   struct addrinfo;
   
   VLC_API vlc_tls_t *vlc_tls_SocketOpenAddrInfo(const struct addrinfo *ai,
                                                 bool defer_connect);
   
   VLC_API vlc_tls_t *vlc_tls_SocketOpenTCP(vlc_object_t *obj,
                                            const char *hostname, unsigned port);
   
   VLC_API vlc_tls_t *vlc_tls_SocketOpenTLS(vlc_tls_creds_t *crd,
                                            const char *hostname, unsigned port,
                                            const char *service,
                                            const char *const *alpn, char **alp);
   
   VLC_DEPRECATED
   static inline vlc_tls_t *
   vlc_tls_ClientSessionCreateFD(vlc_tls_creds_t *crd, int fd, const char *host,
                                 const char *srv, const char *const *lp, char **p)
   {
       vlc_tls_t *sock = vlc_tls_SocketOpen(fd);
       if (unlikely(sock == NULL))
           return NULL;
   
       vlc_tls_t *tls = vlc_tls_ClientSessionCreate(crd, sock, host, srv, lp, p);
       if (unlikely(tls == NULL))
           free(sock);
       return tls;
   }
   
   
   #endif
