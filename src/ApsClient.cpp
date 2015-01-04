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

void ApsClient::start()
{
    uv_read_start(
        (uv_stream_t *)m_tcp,
        []( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf )
        {
            ApsClient *self = reinterpret_cast<ApsClient *>( handle->data );
            buf->base = &self->m_incoming_buffer[0];
            buf->len = self->m_incoming_buffer.size();
        },
        []( uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf )
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
                self->onIncomingTcpData(
                    reinterpret_cast<uint8_t *>( buf->base ), nread );
                self->run();
            }
        } );

    run();
    onIncomingTcpConnection();
    run();
}

void ApsClient::stop()
{
    m_owner->removeApsClient(this);
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

void ApsClient::sendTcpData( const uint8_t *data, ssize_t len )
{
    ApsStateMachine::sendTcpData( data, len );

    uv_buf_t buf;
    buf.base = new char[len];
    buf.len = len;
    memcpy( buf.base, data, len );
    uv_write_t *write_req = new uv_write_t;
    write_req->data = this;

    uv_write( write_req,
              (uv_stream_t *)m_tcp,
              &buf,
              1,
              []( uv_write_t *write_req, int status )
              {
        ApsClient *self = reinterpret_cast<ApsClient *>( write_req->data );
        uint8_t *data = reinterpret_cast<uint8_t *>( write_req->bufsml[0].base );
        delete[] data;
        delete write_req;        
        if ( status < 0 )
        {
            self->closeTcpConnection();
            self->stop();
        }
        self->run();
    } );
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
    write_req->data = this;

    uv_write( write_req,
              (uv_stream_t *)m_tcp,
              &buf,
              1,
              []( uv_write_t *write_req, int status )
              {
        ApsClient *self = reinterpret_cast<ApsClient *>( write_req->data );
        uint8_t *data = reinterpret_cast<uint8_t *>( write_req->bufsml[0].base );
        delete[] data;
        delete write_req;

        uv_close( (uv_handle_t *)self->m_tcp,
                  [](uv_handle_t *handle)
        {
            ApsClient *self = reinterpret_cast<ApsClient *>( handle->data );
            self->closeTcpConnection();
            self->run();
            self->stop();
        });
    } );
}

ApsClient::StateEventsWithWebServing::StateEventsWithWebServing(
    HttpServerParserSimple *parser,
    std::string connect_path,
    const HttpServerFiles &builtin_files )
    : StateEvents( parser, connect_path ), m_builtin_files( builtin_files )
{
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpGetRequest(
    HttpRequest const &request )
{
    bool r = false;
    if ( onIncomingHttpFileGetRequest( request ) )
    {
        r = true;
    }
    else if ( onIncomingHttpCgiGetRequest( request ) )
    {
        r = true;
    }
    return r;
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpHeadRequest(
    HttpRequest const &request )
{
    return onIncomingHttpFileHeadRequest( request );
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpPostRequest(
    HttpRequest const &request )
{
    return onIncomingHttpCgiPostRequest( request );
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpCgiGetRequest(
    HttpRequest const &request )
{
    return false;
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpCgiPostRequest(
    HttpRequest const &request )
{
    return false;
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpFileGetRequest(
    HttpRequest const &request )
{
    bool r = false;

    HttpResponse response;

    std::vector<uint8_t> const *content;
    content = getHttpFileHeaders( request, &response );

    if ( content )
    {
        response.m_content = *content;
        m_aps_client->sendHttpResponse( response );
        r = true;
    }
    return r;
}

bool ApsClient::StateEventsWithWebServing::onIncomingHttpFileHeadRequest(
    const HttpRequest &request )
{
    bool r = false;

    HttpResponse response;

    if ( getHttpFileHeaders( request, &response ) )
    {
        m_aps_client->sendHttpResponse( response );
        r = true;
    }
    return r;
}

std::vector<uint8_t> const *
    ApsClient::StateEventsWithWebServing::getHttpFileHeaders(
        const HttpRequest &request, HttpResponse *response )
{
    using ::Obbligato::formstring;

    std::vector<uint8_t> const *r = 0;

    HttpServerFiles::const_iterator item
        = m_builtin_files.find( request.m_path );

    if ( item != m_builtin_files.end() )
    {
        response->m_headers.push_back( "Connection: Close" );

        response->m_headers.push_back(
            formstring( "Content-Type: ", item->second.m_mime_type ) );

        response->m_headers.push_back(
            formstring( "Content-Length: ", item->second.m_content.size() ) );

        response->m_version = "HTTP/1.1";
        response->m_status_code = "200";
        response->m_reason_phrase = "OK";
        r = &item->second.m_content;
    }

    return r;
}
}
