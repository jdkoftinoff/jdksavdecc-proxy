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

namespace JDKSAvdeccProxy
{
class NetworkService;

class ApsClient : public ApsStateMachine
{
  public:
    ApsClient( NetworkService *owner,
               uint16_t &assigned_id_count,
               active_connections_type &active_connections,
               std::string const &path = "/" )
        : ApsStateMachine( &m_my_variables,
                           &m_my_actions,
                           &m_my_events,
                           &m_my_states,
                           assigned_id_count,
                           active_connections )
        , m_my_events( &m_http_server_parser, path )
        , m_owner( owner )
        , m_http_server_parser( &m_http_server_request, &m_my_events )
    {
    }

    virtual void start() {}

    virtual void stop() {}

    virtual void sendAvdeccToL2( Frame const &frame );

    virtual void closeTcpConnection() {}

    virtual void closeTcpServer();

    virtual void sendTcpData( uint8_t const *data, ssize_t len );

  protected:
    StateVariables m_my_variables;
    StateActions m_my_actions;
    States m_my_states;
    StateEvents m_my_events;

    NetworkService *m_owner;

    HttpServerParserSimple m_http_server_parser;
    HttpRequest m_http_server_request;
};
}
