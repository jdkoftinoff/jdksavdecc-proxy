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

#include "JDKSAvdeccProxy/JDKSAvdeccProxy.hpp"

namespace JDKSAvdeccProxy
{

void ApsClient::onUvReadAllocate( uv_handle_t *handle,
                                  size_t suggested_size,
                                  uv_buf_t *buf )
{
    ApsClient *self = reinterpret_cast<ApsClient *>( handle->data );
    buf->base = &self->m_incoming_buffer[0];
    buf->len = sizeof( self->m_incoming_buffer );
}

void ApsClient::onUvRead( uv_stream_t *stream,
                          ssize_t nread,
                          const uv_buf_t *buf )
{
    ApsClient *self = reinterpret_cast<ApsClient *>( stream->data );
    if ( nread <= 0 )
    {
        self->onTcpConnectionClosed();
        self->run();
        self->stop();
    }
    else
    {
        if ( self->onIncomingTcpData( reinterpret_cast<uint8_t *>( buf->base ),
                                      nread ) == 0 )
        {
            self->onTcpConnectionClosed();
        }
        self->run();
    }
}

void ApsClient::start()
{
    uv_read_start( (uv_stream_t *)&m_tcp, onUvReadAllocate, onUvRead );

    run();
    onIncomingTcpConnection();
    run();
}

void ApsClient::stop()
{
    m_my_states.clear();
    m_my_actions.clear();
    m_my_events.clear();

    m_owner->removeApsClient( this );
}

void ApsClient::sendAvdeccToL2( Frame const &frame )
{
    ApsStateMachine::sendAvdeccToL2( frame );
    // TODO:
}

void ApsClient::closeTcpConnection() {}

void ApsClient::closeTcpServer()
{
    ApsStateMachine::closeTcpServer();
    // TODO:
}

void ApsClient::onUvWrite( uv_write_t *write_req, int status )
{
    UvWriteContext *write_context
        = reinterpret_cast<UvWriteContext *>( write_req->data );

    ApsClient *self = write_context->m_apsclient;

    delete write_context;
    delete write_req;
    if ( status < 0 )
    {
        uv_close( (uv_handle_t *)&self->m_tcp, onUvClose );
    }
    self->run();
}

void ApsClient::sendTcpData( const uint8_t *data, ssize_t len )
{
    ApsStateMachine::sendTcpData( data, len );

    uv_buf_t buf;
    buf.base = new char[len];
    buf.len = len;
    memcpy( buf.base, data, len );
    uv_write_t *write_req = new uv_write_t;
    write_req->data = new UvWriteContext( this, buf.base );

    uv_write( write_req, (uv_stream_t *)&m_tcp, &buf, 1, onUvWrite );
}

void ApsClient::onUvClose( uv_handle_t *handle )
{
    ApsClient *self = reinterpret_cast<ApsClient *>( handle->data );
    self->closeTcpConnection();
    self->run();
    self->stop();
}

void ApsClient::onUvWriteThenClose( uv_write_t *write_req, int status )
{
    UvWriteContext *write_context
        = reinterpret_cast<UvWriteContext *>( write_req->data );

    ApsClient *self = write_context->m_apsclient;

    delete write_context;
    delete write_req;
    uv_close( (uv_handle_t *)&self->m_tcp, onUvClose );
}

void ApsClient::sendHttpResponse( const HttpResponse &response )
{
    std::vector<uint8_t> data_to_send;
    response.flatten( &data_to_send );

    uv_buf_t buf;
    buf.len = data_to_send.size();
    buf.base = new char[buf.len];
    memcpy( buf.base, data_to_send.data(), buf.len );
    uv_write_t *write_req = new uv_write_t;
    write_req->data = new UvWriteContext( this, buf.base );

    uv_write( write_req, (uv_stream_t *)&m_tcp, &buf, 1, onUvWriteThenClose );
}

ApsClient::StateEventsWithWebServing::StateEventsWithWebServing(
    HttpServerParserSimple *parser,
    std::string connect_path,
    NetworkServiceBase *network_service )
    : StateEvents( parser, connect_path ), m_network_service( network_service )
{
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpGetRequest(
    HttpRequest const &request )
{
    bool r = false;
    HttpResponse response;

    if ( m_network_service->onIncomingHttpFileGetRequest( request, &response ) )
    {
        r = true;
    }
    else if ( m_network_service->onIncomingHttpCgiGetRequest( request,
                                                              &response ) )
    {
        r = true;
    }

    if ( r == false )
    {
        r = m_network_service->error404( request, &response );
    }

    if ( r )
    {
        m_aps_client->sendHttpResponse( response );
    }
    return r;
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpHeadRequest(
    HttpRequest const &request )
{
    bool r = false;
    HttpResponse response;

    if ( m_network_service->onIncomingHttpFileHeadRequest( request,
                                                           &response ) )
    {
        r = true;
    }
    if ( r == false )
    {
        r = m_network_service->error404( request, &response );
    }

    if ( r )
    {
        m_aps_client->sendHttpResponse( response );
    }
    return r;
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpPostRequest(
    HttpRequest const &request )
{
    bool r = false;
    HttpResponse response;

    if ( m_network_service->onIncomingHttpCgiPostRequest( request, &response ) )
    {
        r = true;
    }
    if ( r == false )
    {
        r = m_network_service->error404( request, &response );
    }

    if ( r )
    {
        m_aps_client->sendHttpResponse( response );
    }
    return r;
}
}
