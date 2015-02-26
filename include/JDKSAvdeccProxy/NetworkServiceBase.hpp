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
#include "HttpServerBlob.hpp"

namespace JDKSAvdeccProxy
{

class ApsClient;
class RawNetworkHandler;

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
                                 string const &options_prefix ) = 0;
    };

    ///
    /// \brief ~NetworkService
    /// Destroy the network service immediately
    ///
    virtual ~NetworkServiceBase() {}

    ///
    /// \brief start
    /// Start the network service
    ///
    virtual void start() = 0;

    ///
    /// \brief stop
    /// Stop the network service
    ///
    virtual void stop() = 0;

    virtual bool onIncomingHttpFileGetRequest( HttpRequest const &request,
                                               HttpResponse *response ) = 0;
    virtual bool onIncomingHttpFileHeadRequest( HttpRequest const &request,
                                                HttpResponse *response ) = 0;
    virtual bool onIncomingHttpCgiGetRequest( HttpRequest const &request,
                                              HttpResponse *response ) = 0;
    virtual bool onIncomingHttpCgiPostRequest( HttpRequest const &request,
                                               HttpResponse *response ) = 0;

    virtual bool error404( const HttpRequest &request, HttpResponse *response )
        = 0;

    virtual const HttpServerBlob *
        getHttpFileHeaders( HttpRequest const &request, HttpResponse *response )
        = 0;

    virtual void removeApsClient( ApsClient *aps_client ) = 0;

    virtual void addRawNetwork( string const &name,
                                shared_ptr<RawNetworkHandler> handler ) = 0;

    virtual void removeRawNetwork( string const &name ) = 0;

    ///
    /// \brief getLoop get the libuv uv_loop_t for this service
    /// \return uv_loop_t pointer
    ///
    virtual uv_loop_t *getLoop() = 0;
};
}
