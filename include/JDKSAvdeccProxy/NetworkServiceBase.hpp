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

class NetworkServiceBase
{
  public:
    ///
    /// \brief The Settings base class for the NetworkService
    ///
    struct Settings
    {
        ///
        /// \brief addOptions Initialize configuration options parser
        /// \param options reference to OptionGroups to fill in
        ///
        virtual void addOptions( ::Obbligato::Config::OptionGroups &options,
                                 std::string const &options_prefix ) = 0;
    };

    class TCPClientHandler
    {
      public:

        ///
        /// \brief ~TCPClientHandler Destroy the TCPClientHandler
        ///
        virtual ~TCPClientHandler() {}

        ///
        /// \brief startClient do any special initializations for the client handler
        ///
        virtual void startClient() = 0;

        ///
        /// \brief stopClient stop any pending requests for the client handler
        ///
        virtual void stopClient() = 0;

        ///
        /// \brief readAlloc callback to get data buffer address for incoming
        /// data
        /// \param suggested_size libuv suggested size
        /// \param buf uv_buf_t to be filled in
        ///
        virtual void readAlloc( size_t suggested_size, uv_buf_t *buf ) = 0;

        ///
        /// \brief onClientData callback for data received from client via TCP
        /// \param nread number of bytes read
        /// \param buf pointer to uv_buf_t containing data
        ///
        virtual void onClientData( ssize_t nread, const uv_buf_t *buf ) = 0;
    };

    ///
    /// \brief ~NetworkService
    /// Destroy the network service immediately
    ///
    virtual ~NetworkServiceBase() {}

    ///
    /// \brief startService
    /// Start the network service
    ///
    virtual void startService() = 0;

    ///
    /// \brief stopService
    /// Stop the network service
    ///
    virtual void stopService() = 0;
};
}
