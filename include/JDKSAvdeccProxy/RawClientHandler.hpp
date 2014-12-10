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
struct AppMessage;
class NetworkService;



///
/// \brief The RawClientHandler class
///
class RawClientHandler
{
  public:
    RawClientHandler( NetworkService *owner,
                      uv_handle_t *uv_handle );

    virtual ~RawClientHandler();

    ///
    /// \brief startClient
    /// Do any special initializations for the client handler
    ///
    virtual void startClient();

    ///
    /// \brief stopClient
    ///  stop any pending requests for the client handler
    ///
    virtual void stopClient();

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
    virtual void onClientData(
            JDKSAvdeccMCU::Frame const &frame );

    ///
    /// \brief onSentData
    ///
    /// \param req the uv_write_t request object pointer
    /// \param status 0 on success
    ///
    virtual void onSentData( uv_write_t *req, int status );

    ///
    /// \brief onLinkChange
    /// callback when the AVDECC network link status changes
    ///
    /// \param link_up true if the network link is up, false otherwise
    ///
    virtual void onLinkChange(bool link_up);

  protected:

    /// The owner of the Client Connection
    NetworkService *m_owner;

    /// The socket which connects to the incoming AVDECC Proxy Client (APS)
    uv_handle_t *m_uv_handle;

    JDKSAvdeccMCU::FrameWithMTU m_incoming_frame;

    /// The buffer space of outgoing messages
    std::deque<JDKSAvdeccMCU::FrameWithMTU> m_outgoing_app_messages;

};

}
