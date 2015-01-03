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

void ApsClient::sendAvdeccToL2( Frame const &frame )
{
    ApsStateMachine::sendAvdeccToL2( frame );
    // TODO:
}

void ApsClient::closeTcpServer()
{
    ApsStateMachine::closeTcpServer();
    // TODO:
}

void ApsClient::sendTcpData( const uint8_t *data, ssize_t len )
{
    ApsStateMachine::sendTcpData( data, len );
    // TODO:
    uv_write_t write_req;

    // uv_write(&write_req,this->m_)
    m_owner->getLoop();
}

void ApsClient::sendHttpResponse( const HttpResponse &response )
{
    // TODO:
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
    std::vector<uint8_t> const *r = 0;

    HttpServerFiles::const_iterator item
        = m_builtin_files.find( request.m_path );
    if ( item != m_builtin_files.end() )
    {
        std::ostringstream content_length;
        content_length << std::dec << item->second.m_content.size();

        response->m_headers.push_back( "Connection: Close" );
        response->m_headers.push_back( "Content-Type: "
                                       + item->second.m_mime_type );
        response->m_headers.push_back( "Content-Length: "
                                       + content_length.str() );
        response->m_version = "HTTP/1.1";
        response->m_status_code = "200";
        response->m_reason_phrase = "OK";
        r = &item->second.m_content;
    }

    return r;
}
}
