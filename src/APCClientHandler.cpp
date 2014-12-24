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
#include "JDKSAvdeccProxy/APCClientHandler.hpp"
#include "JDKSAvdeccProxy/NetworkService.hpp"

namespace JDKSAvdeccProxy
{

APCClientHandler::APCClientHandler( NetworkService *owner,
                                    uv_tcp_t *uv_tcp,
                                    int client_id )
    : m_owner( owner ), m_uv_tcp( uv_tcp ), m_incoming_app_parser(*this)
, m_client_id( client_id )
{
}

APCClientHandler::~APCClientHandler() { m_owner->removeAPCClient( this ); }

void APCClientHandler::startClient()
{
    m_parsing_http = true;
    m_http_url.clear();
    m_http_status.clear();
    m_http_header_field.clear();
    m_http_header_value.clear();
    m_http_headers.clear();

    // Set up the libuv socket callbacks
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

    // Set up the http_parser callbacks
    http_parser_init(&m_http_parser,HTTP_REQUEST);
    m_http_parser.data = this;
    m_http_parser_settings.on_body =
            []( http_parser *p, const char *buf, size_t len )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpBody( buf, len );
            };

    m_http_parser_settings.on_headers_complete =
            []( http_parser *p )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpHeadersComplete();
            };

    m_http_parser_settings.on_header_field =
            []( http_parser *p, const char *buf, size_t len )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpHeaderField( buf, len );
            };

    m_http_parser_settings.on_header_value =
            []( http_parser *p, const char *buf, size_t len )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpHeaderValue( buf, len );
            };

    m_http_parser_settings.on_message_begin =
            []( http_parser *p )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpMessageBegin();
            };

    m_http_parser_settings.on_message_complete =
            []( http_parser *p )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpMessageComplete();
            };

    m_http_parser_settings.on_status =
            []( http_parser *p, const char *buf, size_t len )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpStatus( buf, len );
            };

    m_http_parser_settings.on_url =
            []( http_parser *p, const char *buf, size_t len )
            {
                APCClientHandler *self = (APCClientHandler *)p->data;
                return self->onHttpUrl( buf, len );
            };
}

void APCClientHandler::stopClient()
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

void APCClientHandler::readAlloc( size_t suggested_size, uv_buf_t *buf )
{
    // the buffer is always pre-allocated
    buf->base = (char *)&m_incoming_buf_storage[0];
    buf->len = m_incoming_buf_storage.size();
}

void APCClientHandler::onClientData( ssize_t nread, const uv_buf_t *buf )
{
    using namespace Obbligato::IOStream;

    if ( nread <= 0 )
    {
        // If the incoming socket is disconnected then stop this client object
        stopClient();
    }
    else if( m_parsing_http )
    {
        // incoming data was received, handle it
        std::cout << label_fmt( "client" ) << hex_fmt( m_client_id )
                  << std::endl;
        std::cout << label_fmt( "received data length" ) << hex_fmt( nread )
                  << std::endl;

        // Parse the HTTP request
        ssize_t parsed_count = http_parser_execute( &m_http_parser, &m_http_parser_settings, buf->base, nread );
        if( parsed_count != nread )
        {
            // if we are still parsing the http request then make sure all data was consumed
            if( m_parsing_http || parsed_count <= 0 )
            {
                std::cout << "Error parsing HTTP header";
                stopClient();
            }
            else
            {
                // there was more data here than the http request header, so pass it to the APP parser
                for ( ssize_t i = parsed_count; i < nread; ++i )
                {
                    // Try parse each octet
                    int e = m_incoming_app_parser.parse( (uint8_t)buf->base[i] );
                    if ( e<0 )
                    {
                        std::cout << "Error parsing" << std::endl;
                    }
                }
            }
        }
    }
    else if( !m_parsing_http )
    {
        // incoming data was received, handle it
        std::cout << label_fmt( "client" ) << hex_fmt( m_client_id )
                  << std::endl;
        std::cout << label_fmt( "received data length" ) << hex_fmt( nread )
                  << std::endl;

        for ( ssize_t i = 0; i < nread; ++i )
        {
            // Try parse each octet
            int e = m_incoming_app_parser.parse( (uint8_t)buf->base[i] );
            if ( e<0 )
            {
                std::cout << "Error parsing" << std::endl;
            }
        }

        // debug info
        for ( ssize_t i = 0; i < nread; ++i )
        {
            uint8_t v = (uint8_t)buf->base[i];
            std::cout << octet_fmt( v );
        }
        std::cout << std::endl;
    }
}

void APCClientHandler::onSentNopData( uv_write_t *req, int status )
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
    }
}

void APCClientHandler::onNopTimeout()
{
    if( !m_parsing_http )
    {
        using namespace Obbligato::IOStream;
        std::cout << label_fmt( "nop timeout for client" ) << hex_fmt( m_client_id )
                  << std::endl;

        JDKSAvdeccMCU::AppMessage msg;
        msg.setNOP();
        sendAppMessageToApc( msg );

#if 0
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
#endif
    }
}

void
    APCClientHandler::onAvdeccData( const JDKSAvdeccMCU::Frame &incoming_frame )
{
    if( !m_parsing_http )
    {

    }
}

void APCClientHandler::onAvdeccLinkChange( bool link_up )
{
    if( !m_parsing_http )
    {

    }
}

void APCClientHandler::sendAppMessageToApc(
    const JDKSAvdeccMCU::AppMessage &msg )
{
    if( !m_parsing_http )
    {

    }
}

void APCClientHandler::onAppNop( const AppMessage &msg )
{
}

void APCClientHandler::onAppEntityIdRequest( const AppMessage &msg ) {}

void APCClientHandler::onAppEntityIdResponse( const AppMessage &msg ) {}

void APCClientHandler::onAppLinkUp( const AppMessage &msg ) {}

void APCClientHandler::onAppLinkDown( const AppMessage &msg ) {}

void APCClientHandler::onAppAvdeccFromAps( const AppMessage &msg ) {}

void APCClientHandler::onAppAvdeccFromApc( const AppMessage &msg ) {}

void APCClientHandler::onAppVendor( const AppMessage &msg ) {}

int APCClientHandler::onHttpMessageBegin()
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return 0;
}

int APCClientHandler::onHttpUrl(const char *buf, size_t len)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    m_http_url.append(buf,len);
    std::cout << m_http_url << "\n";
    return 0;
}

int APCClientHandler::onHttpStatus(const char *buf, size_t len)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    m_http_status.append(buf,len);
    std::cout << m_http_status << "\n";
    return 0;
}

int APCClientHandler::onHttpHeaderField(const char *buf, size_t len)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    if( m_http_header_value.length()>0 )
    {
        m_http_headers.push_back( std::make_pair( m_http_header_field, m_http_header_value ) );
        m_http_header_field.clear();
        m_http_header_value.clear();
    }
    m_http_header_field.append(buf,len);
    std::cout << m_http_header_field << "\n";
    return 0;
}

int APCClientHandler::onHttpHeaderValue(const char *buf, size_t len)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    m_http_header_value.append(buf,len);
    std::cout << m_http_header_value << "\n";
    return 0;
}

int APCClientHandler::onHttpHeadersComplete()
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    if( m_http_header_value.length()>0 )
    {
        m_http_headers.push_back( std::make_pair( m_http_header_field, m_http_header_value ) );
        m_http_header_value.clear();
    }
    for( auto i=m_http_headers.begin(); i!=m_http_headers.end(); ++i )
    {
        std::cout << i->first << " " << i->second << "\n";
    }
    return 0;
}

int APCClientHandler::onHttpBody(const char *buf, size_t len)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return 0;
}

int APCClientHandler::onHttpMessageComplete()
{
    int r=0;
    std::cout << __PRETTY_FUNCTION__ << "\n";

    if( m_http_parser.method == HTTP_CONNECT )
    {
        if( m_http_url == "/" )
        {
            m_parsing_http=0;
        }
    }

    if( m_parsing_http )
    {
        r=-1;
        stopClient();
    }

    return r;
}

}
