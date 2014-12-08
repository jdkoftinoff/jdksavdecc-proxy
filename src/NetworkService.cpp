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
    ::Obbligato::Config::OptionGroups &options )
{
    options.add( "avdecc_proxy", "Avdecc Proxy Settings" )
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
    : m_settings( settings ), m_uv_loop( uv_loop )
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
}

void NetworkService::stopService() {}

void NetworkService::onNewConnection()
{
    uv_tcp_t *client = (uv_tcp_t *)malloc( sizeof( uv_tcp_t ) );
    uv_tcp_init( m_uv_loop, client );
    client->data = this;

    if ( uv_accept( (uv_stream_t *)&m_tcp_server, (uv_stream_t *)client ) == 0 )
    {
        ClientHandler *client_handler = new ClientHandler( this, client );
        client->data = (void *)client_handler;
        m_clients.push_back( client_handler );
        uv_read_start(
            (uv_stream_t *)client,
            []( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf )
            {
                ClientHandler *self = (ClientHandler *)handle->data;
                self->readAlloc( suggested_size, buf );
            },
            []( uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf )
            {
                ClientHandler *self = (ClientHandler *)stream->data;
                if ( nread > 0 )
                {
                    self->onClientData( nread, buf );
                }
                else
                {
                    uv_close( (uv_handle_t *)stream,
                              []( uv_handle_t *h )
                              {
                        ClientHandler *self = (ClientHandler *)h->data;
                        delete self;
                    } );
                }
            } );
    }
    else
    {
        uv_close( (uv_handle_t *)client, NULL );
    }
}

void NetworkService::ClientHandler::onClientData( ssize_t nread,
                                                  const uv_buf_t *buf )
{
    using namespace Obbligato::IOStream;
    std::cout << label_fmt( "client data received" ) << std::endl;
    std::cout << label_fmt( "length" ) << hex_fmt( nread ) << std::endl;

    for ( ssize_t i = 0; i < nread; ++i )
    {
        uint8_t v = (uint8_t)buf->base[i];
        std::cout << hex_fmt( v );
    }
    std::cout << std::endl;
}

void NetworkService::onAvdeccData( uv_udp_t *avdecc_network ) {}

void NetworkService::removeClient( NetworkService::ClientHandler *client )
{
    m_clients.erase(
        std::remove( m_clients.begin(), m_clients.end(), client ) );
}
}
