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
#include "NetworkServiceBase.hpp"
#include "ApsClient.hpp"
#include "RawNetworkHandler.hpp"
#include "HttpServerFiles.hpp"

namespace JDKSAvdeccProxy
{

class ApsClient;
class RawNetworkHandler;

///
/// \brief The NetworkService class
///
/// Encompasses the AVDECC Proxy Server network layer
///
class NetworkService : public NetworkServiceBase
{
  public:
    ///
    /// \brief The Settings for the AVDECC Proxy NetworkService
    ///
    struct Settings : NetworkServiceBase::Settings
    {
        std::string m_listen_host;
        int m_listen_port;
        std::string m_advertise_name;
        int32_t m_priority;
        bool m_advertise_mdns;
        std::string m_avdecc_interface;
        int m_max_clients;

        ///
        /// \brief addOptions Initialize configuration options parser
        /// \param options reference to OptionGroups to fill in
        ///
        virtual void addOptions( ::Obbligato::Config::OptionGroups &options,
                                 std::string const &options_prefix );
    };

    ///
    /// \brief NetworkService constructor
    /// \param settings reference to the initialized Settings for the service
    ///
    explicit NetworkService( Settings const &settings,
                             HttpServerFiles const &server_files,
                             uv_loop_t *uv_loop );

    ///
    /// \brief ~NetworkService
    /// Destroy the network service immediately
    ///
    virtual ~NetworkService() {}

    ///
    /// \brief start
    /// Start the network service
    ///
    virtual void start();

    ///
    /// \brief stop
    /// Stop the network service
    ///
    virtual void stop();

    ///
    /// \brief onNewConnection A new TCP connection is accepted
    ///
    virtual void onNewConnection();

    virtual void removeApsClient( ApsClient *client );

    ///
    /// \brief onAvdeccData Avdecc data is received
    /// This method forwards the avdecc data to all ClientHandler objects
    ///
    virtual void onAvdeccData( ssize_t nread,
                               const uv_buf_t *buf,
                               const sockaddr *addr,
                               unsigned flags );

    ///
    /// \brief sendAvdeccData Avdecc data needs to be sent to the Avdecc
    /// interface
    /// \param frame The Frame object containing the PDU and header
    ///
    virtual void sendAvdeccData( Frame const &frame );

    ///
    /// \brief onTick callback
    ///
    /// Poll network link status and
    /// run state machines
    ///
    virtual void onTick();

    virtual void sendAvdeccToL2( ApsClient *from, Frame const &frame );

    virtual bool onIncomingHttpFileGetRequest( HttpRequest const &request,
                                               HttpResponse *response );
    virtual bool onIncomingHttpFileHeadRequest( HttpRequest const &request,
                                                HttpResponse *response );
    virtual bool onIncomingHttpCgiGetRequest( HttpRequest const &request,
                                              HttpResponse *response );
    virtual bool onIncomingHttpCgiPostRequest( HttpRequest const &request,
                                               HttpResponse *response );

    virtual bool error404( const HttpRequest &request, HttpResponse *response );

    virtual std::shared_ptr<HttpServerBlob>
        getHttpFileHeaders( HttpRequest const &request,
                            HttpResponse *response );

    virtual void addRawNetwork( std::string const &name,
                                std::shared_ptr<RawNetworkHandler> handler );

    virtual void removeRawNetwork( std::string const &name );

    ///
    /// \brief getLoop get the libuv uv_loop_t for this service
    /// \return uv_loop_t pointer
    ///
    virtual uv_loop_t *getLoop();

  protected:
    Settings const &m_settings;
    uv_loop_t *m_uv_loop;
    uv_tcp_t m_tcp_server;
    uv_timer_t m_tick_timer;
    int m_num_clients_created;

    uint16_t m_assigned_id_count;
    ApsClient::active_connections_type m_active_ids;

    std::vector<std::shared_ptr<ApsClient>> m_active_client_handlers;
    std::vector<std::shared_ptr<ApsClient>> m_available_client_handlers;
    HttpServerFiles const &m_builtin_files;

    std::map<std::string, std::shared_ptr<RawNetworkHandler>> m_raw_networks;
};
}
