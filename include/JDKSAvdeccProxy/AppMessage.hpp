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

struct AppMessage
{
    AppMessage();

    AppMessage( AppMessage const &other );

    AppMessage const &operator=( AppMessage const &other );

    void clear() { m_data.clear(); }

    void setNOP();

    void setEntityIdRequest( Eui48 const &apc_primary_mac,
                             Eui64 const &requested_entity_id );

    void setEntityIdResponse( Eui48 const &apc_primary_mac,
                              Eui64 const &requested_entity_id );

    void setLinkUp( Eui48 const &network_port_mac );

    void setLinkDown( Eui48 const &network_port_mac );

    void setAvdeccFromAps( Frame const &frame );

    void setAvdeccFromApc( Frame const &frame );

    void setVendor( Eui48 const &vendor_message_type,
                    FixedBuffer const &payload );

    ssize_t parse( uint8_t octet,
                   std::function<void(jdksavdecc_appdu const &)> handle_msg_cb,
                   std::function<void( ssize_t )> error_cb );

    jdksavdecc_appdu m_app_header;
    std::vector<uint8_t> m_data;
};
}
