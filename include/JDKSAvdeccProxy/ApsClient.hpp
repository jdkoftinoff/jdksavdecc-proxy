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
#include "HttpServerFiles.hpp"

namespace JDKSAvdeccProxy
{
class NetworkService;

class ApsClient : public ApsStateMachine
{
  public:
    ApsClient( NetworkServiceBase *owner,
               uint16_t &assigned_id_count,
               active_connections_type &active_connections,
               string const &path,
               HttpServerFiles const &http_server_files )
        : ApsStateMachine( &m_my_variables,
                           &m_my_actions,
                           &m_my_events,
                           &m_my_states,
                           assigned_id_count,
                           active_connections )
        , m_my_events( &m_http_server_parser, path, owner )
        , m_owner( owner )
        , m_http_server_parser( &m_http_server_request, &m_my_events )
        , m_http_server_files( http_server_files )
    {
        m_my_events.setApsClient( this );
    }

    uv_tcp_t *getTcp() { return &m_tcp; }

    virtual void start();

    virtual void stop();

    virtual void sendAvdeccToL2( Frame const &frame ) override;

    virtual void closeTcpConnection() override;

    virtual void closeTcpServer() override;

    virtual void sendTcpData( uint8_t const *data, ssize_t len ) override;

    class StateEventsWithWebServing : public ApsStateEvents
    {
      public:
        StateEventsWithWebServing( HttpServerParserSimple *parser,
                                   string connect_path,
                                   NetworkServiceBase *network_service );

        void setApsClient( ApsClient *aps_client )
        {
            m_aps_client = aps_client;
        }

        virtual bool onIncomingHttpGetRequest( HttpRequest const &request ) override;
        virtual bool onIncomingHttpHeadRequest( HttpRequest const &request ) override;
        virtual bool onIncomingHttpPostRequest( HttpRequest const &request ) override;

      protected:
        ApsClient *m_aps_client;
        NetworkServiceBase *m_network_service;
    };

  protected:
    virtual void sendHttpResponse( HttpResponse const &response );

    ApsStateVariables m_my_variables;
    ApsStateActions m_my_actions;
    ApsStates m_my_states;
    StateEventsWithWebServing m_my_events;

    NetworkServiceBase *m_owner;

    HttpServerParserSimple m_http_server_parser;
    HttpRequest m_http_server_request;
    HttpServerFiles const &m_http_server_files;
    uv_tcp_t m_tcp;

    char m_incoming_buffer[8192];

  private:
    static void onUvReadAllocate( uv_handle_t *handle,
                                  size_t suggested_size,
                                  uv_buf_t *buf );

    static void
        onUvRead( uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf );

    static void onUvWrite( uv_write_t *write_req, int status );

    static void onUvWriteThenClose( uv_write_t *write_req, int status );

    static void onUvClose( uv_handle_t *handle );

    struct UvWriteContext
    {
        UvWriteContext( ApsClient *apsclient, uint8_t *data )
            : m_apsclient( apsclient ), m_data( data )
        {
        }

        UvWriteContext( ApsClient *apsclient, char *data )
            : m_apsclient( apsclient )
            , m_data( reinterpret_cast<uint8_t *>( data ) )
        {
        }

        ~UvWriteContext() { delete[] m_data; }

        ApsClient *m_apsclient;
        uint8_t *m_data;
    };
};
}
