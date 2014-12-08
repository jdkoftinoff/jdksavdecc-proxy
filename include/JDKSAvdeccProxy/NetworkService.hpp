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

///
/// \brief The NetworkService class
///
/// Encompasses the AVDECC Proxy Server network layer
///
class NetworkService
{
  public:
    ///
    /// \brief The Settings for the AVDECC Proxy NetworkService
    ///
    struct Settings
    {
        std::string m_listen_host;
        std::string m_listen_port;
        std::string m_advertise_name;
        int32_t m_priority;
        bool m_advertise_mdns;
        std::string m_avdecc_interface;

        ///
        /// \brief addOptions Initialize configuration options parser
        /// \param options reference to OptionGroups to fill in
        ///
        virtual void addOptions( ::Obbligato::Config::OptionGroups &options );
    };

    NetworkService( Settings &settings ) : m_settings( settings ) {}

    virtual ~NetworkService() {}

    virtual void startService();

    virtual void stopService();

    virtual bool runService();

  protected:
    Settings &m_settings;
};
}
