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

#include "JDKSAvdeccProxy/World.hpp"
#include "Obbligato/Config.hpp"
#include "JDKSAvdeccProxy/JDKSAvdeccProxy.hpp"

namespace JDKSAvdeccProxy
{

void NetworkService::Settings::addOptions(
    ::Obbligato::Config::OptionGroups &options,
    std::string const &options_prefix )
{
    options.add( options_prefix.c_str(), "Avdecc Proxy Settings" )
        .add( "listen_host",
              "127.0.0.1",
              "Hostname or IP address to listen on for incoming connections",
              m_listen_host )
        .add( "listen_port",
              "17221",
              "TCP Port to listen on for incoming connections",
              m_listen_port )
        .add( "advertise_name",
              "JDKSAvdeccProxy",
              "Name to use for advertising via mdns-sd",
              m_advertise_name )
        .add( "priority",
              "1",
              "Server Priority advertized via mdns-sd",
              m_priority )
        .add( "advertise_mdns", "0", "Advertise via mdns-sd", m_advertise_mdns )
        .add( "avdecc_interface",
              "en0",
              "Name of the Network Interface to use for AVDECC messages",
              m_avdecc_interface )
        .add( "max_clients",
              "16",
              "Maximum number of simultaneous clients",
              m_max_clients );
}

NetworkService::NetworkService( const NetworkService::Settings &settings,
                                HttpServerFiles const &server_files,
                                uv_loop_t *uv_loop )
    : m_settings( settings )
    , m_uv_loop( uv_loop )
    , m_num_clients_created( 0 )
    , m_assigned_id_count( 0 )
    , m_active_ids()
    , m_raw_network_handler( this, uv_loop )
{
    for ( int i = 0; i < settings.m_max_clients; ++i )
    {
        m_available_client_handlers.push_back( std::make_shared<ApsClient>(
            this, m_assigned_id_count, m_active_ids, "/", server_files ) );
    }

    // initialize the uv tcp server object
    uv_tcp_init( m_uv_loop, &m_tcp_server );
    m_tcp_server.data = this;
}

void NetworkService::start()
{
    int r = 0;

    // try ipv6 first
    {
        sockaddr_in6 ip6_bind_addr;
        r = uv_ip6_addr( m_settings.m_listen_host.c_str(),
                         m_settings.m_listen_port,
                         &ip6_bind_addr );
        if ( !r )
        {
            r = uv_tcp_bind( &m_tcp_server, (sockaddr *)&ip6_bind_addr, 0 );
        }
    }

    // if unable, try ipv4
    if ( r )
    {
        sockaddr_in ip4_bind_addr;
        r = uv_ip4_addr( m_settings.m_listen_host.c_str(),
                         m_settings.m_listen_port,
                         &ip4_bind_addr );
        if ( !r )
        {
            r = uv_tcp_bind( &m_tcp_server, (sockaddr *)&ip4_bind_addr, 0 );
        }
    }

    // if we were able to bind, try listen
    if ( !r )
    {
        r = uv_listen( (uv_stream_t *)&m_tcp_server,
                       128,
                       []( uv_stream_t *server, int status )
                       {
            if ( !status )
            {
                NetworkService *self = (NetworkService *)server->data;
                self->onNewConnection();
            }
        } );
    }

    // if we got some sort of error, log it and throw
    if ( r )
    {
        std::ostringstream s;

        s << "NetworkService::startService() error listening to: "
          << m_settings.m_listen_host << ":" << m_settings.m_listen_port
          << " : " << uv_err_name( r );

        throw std::runtime_error( s.str() );
    }

    // create a single 250 ms timer for updating state machines
    uv_timer_init( m_uv_loop, &m_tick_timer );
    m_tick_timer.data = this;
    uv_timer_start( &m_tick_timer,
                    []( uv_timer_t *timer )
                    {
                        NetworkService *self = (NetworkService *)timer->data;
                        self->onTick();
                    },
                    0,
                    250 );
}

void NetworkService::stop()
{
    uv_timer_stop( &m_tick_timer );
    uv_close( (uv_handle_t *)&m_tcp_server, NULL );

    while ( m_active_client_handlers.size() > 0 )
    {
        std::shared_ptr<ApsClient> aps = m_active_client_handlers.back();
        aps->stop();
        m_available_client_handlers.push_back( aps );
        m_active_client_handlers.pop_back();
    }
}

void NetworkService::onNewConnection()
{
    uv_tcp_t *client = (uv_tcp_t *)malloc( sizeof( uv_tcp_t ) );
    uv_tcp_init( m_uv_loop, client );

    if ( uv_accept( (uv_stream_t *)&m_tcp_server, (uv_stream_t *)client ) == 0 )
    {
        if ( m_available_client_handlers.size() > 0 )
        {
            auto aps = m_available_client_handlers.back();
            m_available_client_handlers.pop_back();
            // TODO:
            // aps->setLinkMac( m_raw_network_handler.getMac() );
            aps->setup();
            client->data = (void *)aps.get();
            aps->setTcp( client );
            m_active_client_handlers.push_back( aps );
            aps->run();
        }
        else
        {
            uv_close( (uv_handle_t *)client, NULL );
        }
    }
    else
    {
        uv_close( (uv_handle_t *)client, NULL );
    }
}

void NetworkService::onAvdeccData( ssize_t nread,
                                   const uv_buf_t *buf,
                                   const sockaddr *addr,
                                   unsigned flags )
{
}

void NetworkService::sendAvdeccData( const JDKSAvdeccMCU::Frame &frame ) {}

void NetworkService::onTick()
{
    // TODO: poll link status and notify ApsClients any link status change

    // Notify all ApsClients about time
    jdksavdecc_timestamp_in_microseconds ts
        = get_current_time_in_microseconds();
    uint32_t time_in_seconds = ts / 1000000;
    for ( auto i = m_active_client_handlers.begin();
          i != m_active_client_handlers.end();
          ++i )
    {
        ( *i )->onTimeTick( time_in_seconds );
    }
}

void NetworkService::sendAvdeccToL2( ApsClient *from, Frame const &frame )
{
    // TODO: send the avdecc message to the L2 network
}

void NetworkService::removeApsClient( ApsClient *client )
{
    for ( auto i = m_active_client_handlers.begin();
          i != m_active_client_handlers.end();
          ++i )
    {
        if ( i->get() == client )
        {
            m_available_client_handlers.push_back( *i );
            m_active_client_handlers.erase( i );
            break;
        }
    }
}

uv_loop_t *NetworkService::getLoop() { return m_uv_loop; }
}
