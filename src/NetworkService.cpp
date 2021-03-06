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
void NetworkService::listen_handler( uv_stream_t *server, int status )
{
    if ( !status )
    {
        NetworkService *self = (NetworkService *)server->data;
        self->onNewConnection();
    }
}

void NetworkService::tick_handler( uv_timer_t *timer )
{
    NetworkService *self = (NetworkService *)timer->data;
    self->onTick();
}

void NetworkService::Settings::addOptions(
    ::Obbligato::Config::OptionGroups &options, string const &options_prefix )
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
    , m_builtin_files( server_files )
{
    for ( int i = 0; i < settings.m_max_clients; ++i )
    {
        m_available_client_handlers.push_back(
            shared_ptr<ApsClient>( new ApsClient( this,
                                                  m_assigned_id_count,
                                                  m_active_ids,
                                                  "/",
                                                  server_files ) ) );
    }

    if ( settings.m_avdecc_interface.length() > 0 )
    {
        m_raw_networks[settings.m_avdecc_interface]
            = shared_ptr<RawNetworkHandler>( new RawNetworkHandler(
                this, uv_loop, settings.m_avdecc_interface ) );
    }

    // initialize the uv tcp server object
    uv_tcp_init( m_uv_loop, &m_tcp_server );
    m_tcp_server.data = this;

    // add the cgi handlers
    addCgiHandler( "/cgi-bin/status",
                   shared_ptr<CgiStatus>( new CgiStatus( this ) ) );

    addCgiHandler( "/cgi-bin/config",
                   shared_ptr<CgiConfig>( new CgiConfig( this ) ) );
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
        r = uv_listen( (uv_stream_t *)&m_tcp_server, 128, listen_handler );
    }

    // if we got some sort of error, log it and throw
    if ( r )
    {
        throw runtime_error(
            formstring( "NetworkService::startService() error listening to: ",
                        m_settings.m_listen_host,
                        ":",
                        m_settings.m_listen_port,
                        " : ",
                        uv_err_name( r ) ) );
    }

    // create a single 250 ms timer for updating state machines
    uv_timer_init( m_uv_loop, &m_tick_timer );
    m_tick_timer.data = this;
    uv_timer_start( &m_tick_timer, tick_handler, 0, 250 );
}

void NetworkService::stop()
{
    uv_timer_stop( &m_tick_timer );
    uv_close( (uv_handle_t *)&m_tcp_server, NULL );

    while ( m_active_client_handlers.size() > 0 )
    {
        shared_ptr<ApsClient> aps = m_active_client_handlers.back();
        aps->stop();
        m_available_client_handlers.push_back( aps );
        m_active_client_handlers.pop_back();
    }
}

void NetworkService::onNewConnection()
{
    if ( m_available_client_handlers.size() > 0 )
    {
        shared_ptr<ApsClient> aps = m_available_client_handlers.back();

        uv_tcp_init( m_uv_loop, aps->getTcp() );
        aps->getTcp()->data = (void *)aps.get();

        if ( uv_accept( (uv_stream_t *)&m_tcp_server,
                        (uv_stream_t *)aps->getTcp() ) == 0 )
        {
            m_available_client_handlers.pop_back();

            aps->setup();
            aps->start();
            m_active_client_handlers.push_back( aps );
            aps->run();
        }
        else
        {
            uv_close( (uv_handle_t *)aps->getTcp(), 0 );
        }
    }
}

void NetworkService::onAvdeccData( ssize_t nread,
                                   const uv_buf_t *buf,
                                   const sockaddr *addr,
                                   unsigned flags )
{
    // TODO: form Frame and pass to all clients
}

void NetworkService::sendAvdeccData( const JDKSAvdeccMCU::Frame &frame ) {}

void NetworkService::onTick()
{
    // TODO: poll link status and notify ApsClients any link status change

    // Notify all ApsClients about time
    jdksavdecc_timestamp_in_microseconds ts
        = get_current_time_in_microseconds();
    uint32_t time_in_seconds = static_cast<uint32_t>( ts / 1000000 );
    for ( vector<shared_ptr<ApsClient> >::iterator i
          = m_active_client_handlers.begin();
          i != m_active_client_handlers.end();
          ++i )
    {
        ( *i )->onTimeTick( time_in_seconds );
        ( *i )->run();
    }

    // Notify all raw networks about time

    for ( map<string, shared_ptr<RawNetworkHandler> >::iterator i
          = m_raw_networks.begin();
          i != m_raw_networks.end();
          ++i )
    {
        i->second->onTimeTick( time_in_seconds );
    }
}

void NetworkService::sendAvdeccToL2( ApsClient *from, Frame const &frame )
{
    // TODO: send the avdecc message to the L2 network
}

bool NetworkService::onIncomingHttpFileGetRequest( const HttpRequest &request,
                                                   HttpResponse *response )
{
    bool r = false;

    shared_ptr<HttpServerBlob> content
        = getHttpFileHeaders( request, response );

    if ( content )
    {
        response->m_content.clear();
        response->m_content.resize( content->getContentLength() );
        memcpy( response->m_content.data(),
                content->getContent(),
                content->getContentLength() );
        r = true;
    }
    return r;
}

bool NetworkService::onIncomingHttpFileHeadRequest( const HttpRequest &request,
                                                    HttpResponse *response )
{
    bool r = false;

    if ( getHttpFileHeaders( request, response ) )
    {
        r = true;
    }
    return r;
}

bool NetworkService::onIncomingHttpCgiGetRequest( const HttpRequest &request,
                                                  HttpResponse *response )
{
    bool r = false;
    shared_ptr<HttpServerCgi> h = findCgiHandler( request.m_path );
    if ( h )
    {
        r = h->onIncomingHttpGetRequest( request, response );
    }
    return r;
}

bool NetworkService::onIncomingHttpCgiPostRequest( const HttpRequest &request,
                                                   HttpResponse *response )
{
    bool r = false;
    shared_ptr<HttpServerCgi> h = findCgiHandler( request.m_path );
    if ( h )
    {
        r = h->onIncomingHttpPostRequest( request, response );
    }
    return r;
}

shared_ptr<HttpServerBlob>
    NetworkService::getHttpFileHeaders( const HttpRequest &request,
                                        HttpResponse *response )
{
    shared_ptr<HttpServerBlob> i = m_builtin_files.find( request.m_path );

    if ( i && i->getContent() )
    {
        response->m_headers.push_back( "Connection: Close" );

        response->m_headers.push_back(
            formstring( "Content-Type: ", i->getMimeType() ) );

        response->m_headers.push_back(
            formstring( "Content-Length: ", i->getContentLength() ) );

        response->m_version = "HTTP/1.1";
        response->m_status_code = "200";
        response->m_reason_phrase = "OK";
    }

    return i;
}

void NetworkService::addRawNetwork( const string &name,
                                    shared_ptr<RawNetworkHandler> handler )
{
    m_raw_networks[name] = handler;
}

void NetworkService::removeRawNetwork( const string &name )
{
    m_raw_networks.erase( name );
}

bool NetworkService::error404( const HttpRequest &request,
                               HttpResponse *response )
{
    response->setContent(
        "<DOCTYPE html><html lang=\"en\"><h1>Not Found</h1></html>" );

    response->addHeader( "Connection", "Close" );

    response->addHeader( "Content-Type", "text/html" );

    response->addHeader(
        formstring( "Content-Length", ": ", response->m_content.size() ) );

    response->m_version = "HTTP/1.1";
    response->m_status_code = "404";
    response->m_reason_phrase = "Not Found";

    return true;
}

void NetworkService::removeApsClient( ApsClient *client )
{
    for ( vector<shared_ptr<ApsClient> >::iterator i
          = m_active_client_handlers.begin();
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

void NetworkService::addCgiHandler( const string &prefix,
                                    shared_ptr<HttpServerCgi> handler )
{
    m_cgi_handlers[prefix] = handler;
}

shared_ptr<HttpServerCgi> NetworkService::findCgiHandler( const string &path )
{
    string truncated_path;

    ssize_t pos = path.find_first_of( '?' );
    if ( pos > 0 )
    {
        truncated_path = path.substr( 0, pos );
    }
    else
    {
        truncated_path = path;
    }
    shared_ptr<HttpServerCgi> r;

    map<string, shared_ptr<HttpServerCgi> >::iterator i
        = m_cgi_handlers.find( truncated_path );
    if ( i != m_cgi_handlers.end() )
    {
        r = i->second;
    }
    return r;
}

bool NetworkService::CgiStatus::onIncomingHttpGetRequest(
    const HttpRequest &request, HttpResponse *response )
{
    response->addHeader( "Content-Type", "application/json" );
    response->addHeader( "Content-Encoding", "utf-8" );
    response->m_status_code = "200";
    response->m_reason_phrase = "OK";
    response->m_version = "HTTP/1.1";
    response->setContent( "{ 1,2,3,4, name: 'a name' }" );
    response->addHeader( "Content-Length",
                         formstring( response->m_content.size() ) );
    return true;
}

bool NetworkService::CgiStatus::onIncomingHttpPostRequest(
    const HttpRequest &request, HttpResponse *response )
{
    return false;
}

bool NetworkService::CgiConfig::onIncomingHttpGetRequest(
    const HttpRequest &request, HttpResponse *response )
{
    response->addHeader( "Content-Type", "application/json" );
    response->addHeader( "Content-Encoding", "utf-8" );
    response->m_status_code = "200";
    response->m_reason_phrase = "OK";
    response->m_version = "HTTP/1.1";
    response->setContent( "{ 5,6,7,8, name: 'some name' }" );
    response->addHeader( "Content-Length",
                         formstring( response->m_content.size() ) );
    return true;
}

bool NetworkService::CgiConfig::onIncomingHttpPostRequest(
    const HttpRequest &request, HttpResponse *response )
{
    return false;
}
}
