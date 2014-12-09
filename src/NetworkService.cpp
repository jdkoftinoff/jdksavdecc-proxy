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
              m_avdecc_interface );
}

NetworkService::NetworkService( const NetworkService::Settings &settings,
                                uv_loop_t *uv_loop )
    : m_settings( settings ), m_uv_loop( uv_loop ), m_num_clients_created( 0 )
{
    // initialize the uv tcp server object
    uv_tcp_init( m_uv_loop, &m_tcp_server );
    m_tcp_server.data = this;
}

void NetworkService::startService()
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

        ob_log_error( s.str() );
        throw std::runtime_error( s.str() );
    }

    // create a single 10 second timer for NOP usages
    uv_timer_init( m_uv_loop, &m_nop_timer );
    m_nop_timer.data = this;
    uv_timer_start( &m_nop_timer,
                    []( uv_timer_t *timer )
                    {
                        NetworkService *self = (NetworkService *)timer->data;
                        self->onNopTimeout();
                    },
                    0,
                    10 * 1000 );
}

void NetworkService::stopService()
{
    uv_timer_stop( &m_nop_timer );
    uv_close( (uv_handle_t *)&m_tcp_server, NULL );

    for ( auto i = m_active_client_handlers.begin();
          i != m_active_client_handlers.end();
          ++i )
    {
        ( *i )->stopClient();
        delete *i;
    }
}

void NetworkService::onNewConnection()
{
    uv_tcp_t *client = (uv_tcp_t *)malloc( sizeof( uv_tcp_t ) );
    uv_tcp_init( m_uv_loop, client );
    client->data = this;

    if ( uv_accept( (uv_stream_t *)&m_tcp_server, (uv_stream_t *)client ) == 0 )
    {
        APCClientHandler *client_handler
            = new APCClientHandler( this, client, m_num_clients_created++ );
        client->data = (void *)client_handler;
        m_active_client_handlers.push_back( client_handler );
        client_handler->startClient();
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

void NetworkService::onNopTimeout()
{
    for ( auto i = m_active_client_handlers.begin();
          i != m_active_client_handlers.end();
          ++i )
    {
        ( *i )->onNopTimeout();
    }
}

NetworkService::APCClientHandler::APCClientHandler( NetworkService *owner,
                                                    uv_tcp_t *uv_tcp,
                                                    int client_id )
    : m_owner( owner )
    , m_uv_tcp( uv_tcp )
    , m_incoming_buf_storage()
    , m_client_id( client_id )
{
    m_outgoing_nop_buf.base = 0;
    m_outgoing_nop_buf.len = 0;

    m_nop_write_request.data = 0;
}

NetworkService::APCClientHandler::~APCClientHandler()
{
    m_owner->removeClient( this );
}

void NetworkService::APCClientHandler::startClient()
{
    uv_read_start(

        // Start reading for this socket
        (uv_stream_t *)m_uv_tcp,

        // When the socket is readable, call readAlloc() to allocate buffer
        // space for the data
        []( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf )
        {
            APCClientHandler *self = (APCClientHandler *)handle->data;
            self->readAlloc( suggested_size, buf );
        },

        // When data for the socket has been read, call the onClientData()
        // method with the data
        []( uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf )
        {
            APCClientHandler *self = (APCClientHandler *)stream->data;
            self->onClientData( nread, buf );
        } );
}

void NetworkService::APCClientHandler::stopClient()
{
    // stop the reading of the socket
    uv_read_stop( (uv_stream_t *)m_uv_tcp );

    // schedule the socket to be closed and delete this client object when it is
    // closed
    uv_close( (uv_handle_t *)m_uv_tcp,
              []( uv_handle_t *h )
              {
        APCClientHandler *self = (APCClientHandler *)h->data;
        delete self;
    } );
}

void NetworkService::APCClientHandler::readAlloc( size_t suggested_size,
                                                  uv_buf_t *buf )
{
    // the buffer is always pre-allocated
    buf->base = &m_incoming_buf_storage[0];
    buf->len = m_incoming_buf_storage.size();
}

void NetworkService::APCClientHandler::onClientData( ssize_t nread,
                                                     const uv_buf_t *buf )
{
    using namespace Obbligato::IOStream;

    if ( nread <= 0 )
    {
        // If the incoming socket is disconnected then stop this client object
        stopClient();
    }
    else
    {
        // incoming data was received, handle it
        std::cout << label_fmt( "client" ) << hex_fmt( m_client_id )
                  << std::endl;
        std::cout << label_fmt( "received data length" ) << hex_fmt( nread )
                  << std::endl;

        for ( ssize_t i = 0; i < nread; ++i )
        {
            uint8_t v = (uint8_t)buf->base[i];
            std::cout << octet_fmt( v );
        }
        std::cout << std::endl;
    }
}

void NetworkService::APCClientHandler::onSentNopData( uv_write_t *req,
                                                      int status )
{
    if ( status )
    {
        // error sending
        using namespace Obbligato::IOStream;
        std::cout << label_fmt( "unable to send NOP for client" )
                  << hex_fmt( m_client_id ) << std::endl;
        stopClient();
    }
    else
    {
        // clear buf for next send
        req->data = 0;
        m_outgoing_nop_buf.base = 0;
    }
}

void NetworkService::APCClientHandler::onNopTimeout()
{
    using namespace Obbligato::IOStream;
    std::cout << label_fmt( "nop timeout for client" ) << hex_fmt( m_client_id )
              << std::endl;

    if ( m_nop_write_request.data == 0 && m_outgoing_nop_buf.base == 0 )
    {
        jdksavdecc_appdu appdu;
        jdksavdecc_appdu_init( &appdu );
        jdksavdecc_appdu_set_nop( &appdu );

        jdksavdecc_appdu_write( &appdu,
                                &m_outgoing_nop_buf_storage[0],
                                0,
                                m_outgoing_nop_buf_storage.size() );

        m_outgoing_nop_buf.base = &m_outgoing_nop_buf_storage[0];
        m_outgoing_nop_buf.len = JDKSAVDECC_APPDU_HEADER_LEN
                                 + appdu.payload_length;

        m_nop_write_request.data = this;
        uv_write( &m_nop_write_request,
                  (uv_stream_t *)m_uv_tcp,
                  &m_outgoing_nop_buf,
                  1,
                  []( uv_write_t *write_req, int status )
                  {
            APCClientHandler *self = (APCClientHandler *)write_req->data;
            self->onSentNopData( write_req, status );
        } );
    }
    else
    {
        std::cout << label_fmt( "unable to send NOP for client" )
                  << hex_fmt( m_client_id ) << std::endl;
    }
}

void NetworkService::APCClientHandler::onAvdeccData(
    const JDKSAvdeccMCU::Frame &incoming_frame )
{
}

void NetworkService::removeClient( NetworkService::APCClientHandler *client )
{
    m_active_client_handlers.erase(
        std::remove( m_active_client_handlers.begin(),
                     m_active_client_handlers.end(),
                     client ) );
}

uv_loop_t *NetworkService::getLoop() { return m_uv_loop; }
}
