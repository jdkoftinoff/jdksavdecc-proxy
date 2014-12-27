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

namespace JDKSAvdeccProxy
{

class APCClientHandler;
class RawNetworkHandler;
class NetworkService;

///
/// \brief The APCClientHandler class
///
/// Handles all interactions with an AVDECC Proxy Client via
/// the incoming TCP connection
///
class APCClientHandler : public AppMessageHandler
{
  public:
    ///
    /// \brief APCClientHandler Constructor
    ///
    /// Construct an AVDECC Proxy Server ClientHandler object,
    /// attached to the owning NetworkService, and the
    /// uv_tcp socket that is attached to the incoming client
    ///
    /// \param owner Owner of this client
    /// \param uv_tcp TCP socket to the incoming client
    ///
    APCClientHandler( NetworkService *owner, uv_tcp_t *uv_tcp, int client_id );

    ///
    /// \brief ~ClientHandler Destructor
    ///
    /// de-registers itself from the owner
    ///
    virtual ~APCClientHandler();

    ///
    /// \brief start
    /// Do any special initializations for the client handler
    ///
    virtual void start();

    ///
    /// \brief stop
    ///  stop any pending requests for the client handler
    ///
    virtual void stop();

    ///
    /// \brief readAlloc
    /// callback to get data buffer address for incoming data
    /// \param suggested_size libuv suggested size
    /// \param buf uv_buf_t to be filled in
    ///
    virtual void readAlloc( size_t suggested_size, uv_buf_t *buf );

    ///
    /// \brief onClientData
    /// callback for data received from client via TCP
    ///
    /// \param nread number of bytes read
    /// \param buf pointer to uv_buf_t containing data
    ///
    virtual void onClientData( ssize_t nread, const uv_buf_t *buf );

    ///
    /// \brief onSentNopData
    /// called when a NOP message was actually transmitted
    ///
    /// \param req the uv_write_t request object pointer
    /// \param status 0 on success
    ///
    virtual void onSentNopData( uv_write_t *req, int status );

    ///
    /// \brief onNopTimeout callback for the 10 second NOP timeout
    ///
    virtual void onNopTimeout();

    ///
    /// \brief onAvdeccData
    /// callback received AVDECC message from AVDECC network
    ///
    /// \param incoming_frame The AVDECC received data
    ///
    virtual void onAvdeccData( JDKSAvdeccMCU::Frame const &incoming_frame );

    ///
    /// \brief onAvdeccLinkChange
    /// callback when the AVDECC network link status changes
    ///
    /// \param link_up true if the network link is up, false otherwise
    ///
    virtual void onAvdeccLinkChange( bool link_up );

    ///
    /// \brief sendAppMessageToApc
    ///
    /// Put the AppMessage into the outgoing queue
    ///
    /// \param msg
    ///
    virtual void sendAppMessageToApc( AppMessage const &msg );

  protected:

    virtual void onAppNop( AppMessage const &msg );

    virtual void onAppEntityIdRequest( AppMessage const &msg );

    virtual void onAppEntityIdResponse( AppMessage const &msg );

    virtual void onAppLinkUp( AppMessage const &msg );

    virtual void onAppLinkDown( AppMessage const &msg );

    virtual void onAppAvdeccFromAps( AppMessage const &msg );

    virtual void onAppAvdeccFromApc( AppMessage const &msg );

    virtual void onAppVendor( AppMessage const &msg );


  protected:

    virtual int onHttpMessageBegin();

    virtual int onHttpUrl( const char *buf, size_t len );

    virtual int onHttpStatus( const char *buf, size_t len );

    virtual int onHttpHeaderField( const char *buf, size_t len );

    virtual int onHttpHeaderValue( const char *buf, size_t len );

    virtual int onHttpHeadersComplete();

    virtual int onHttpBody( const char *buf, size_t len );

    virtual int onHttpMessageComplete();

    http_parser m_http_parser;

    bool m_parsing_http;
    std::string m_http_url;
    std::string m_http_status;
    std::string m_http_header_field;
    std::string m_http_header_value;
    std::vector< std::pair< std::string, std::string > > m_http_headers;

  protected:
    /// The owner of the Client Connection
    NetworkService *m_owner;

    /// The socket which connects to the incoming AVDECC Proxy Client (APS)
    uv_tcp_t *m_uv_tcp;

    /// The incoming buffer space
    std::array<uint8_t, 8192> m_incoming_buf_storage;

    /// The APP message parser to read messges from APC
    AppMessageParser m_incoming_app_parser;

    /// The queue for outgoing APP messages to the APC
    std::deque<AppMessage> m_outgoing_app_messages;

    http_parser_settings m_http_parser_settings;


    /// Identifier for this client object
    int m_client_id;
};
}
