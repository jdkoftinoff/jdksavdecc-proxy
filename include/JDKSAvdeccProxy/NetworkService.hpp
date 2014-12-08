/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "World.hpp"

namespace JDKSAvdeccProxy
{

///
/// \brief The NetworkService class
///
/// Encompasses the AVDECC Proxy Server network layer
///
class NetworkService
{
  public:
    ///
    /// \brief The Settings for the AVDECC Proxy NetworkService
    ///
    struct Settings
    {
        std::string m_listen_host;
        int m_listen_port;
        std::string m_advertise_name;
        int32_t m_priority;
        bool m_advertise_mdns;
        std::string m_avdecc_interface;

        ///
        /// \brief addOptions Initialize configuration options parser
        /// \param options reference to OptionGroups to fill in
        ///
        virtual void addOptions( ::Obbligato::Config::OptionGroups &options );
    };

    ///
    /// \brief NetworkService constructor
    /// \param settings reference to the initialized Settings for the service
    ///
    explicit NetworkService( Settings const &settings, uv_loop_t *uv_loop );

    ///
    /// \brief ~NetworkService
    /// Destroy the network service immediately
    ///
    virtual ~NetworkService() {}

    ///
    /// \brief startService
    /// Start the network service
    ///
    virtual void startService();

    ///
    /// \brief stopService
    /// Stop the network service
    ///
    virtual void stopService();

    class ClientHandler
    {
      public:
        ClientHandler( NetworkService *owner, uv_tcp_t *uv_tcp )
            : m_owner( owner ), m_uv_tcp( uv_tcp ), m_buf()
        {
        }
        virtual ~ClientHandler() { m_owner->removeClient( this ); }

        virtual void readAlloc( size_t suggested_size, uv_buf_t *buf )
        {
            buf->base = &m_buf[0];
            buf->len = m_buf.size();
        }

        virtual void onClientData( ssize_t nread, const uv_buf_t *buf );

      protected:
        NetworkService *m_owner;
        uv_tcp_t *m_uv_tcp;
        std::array<char, 8192> m_buf;
    };

  protected:
    virtual void onNewConnection();

    virtual void onAvdeccData( uv_udp_t *avdecc_network );

    void removeClient( ClientHandler *client );

  protected:
    Settings const &m_settings;
    uv_loop_t *m_uv_loop;
    uv_tcp_t m_tcp_server;
    std::vector<ClientHandler *> m_clients;
};
}
