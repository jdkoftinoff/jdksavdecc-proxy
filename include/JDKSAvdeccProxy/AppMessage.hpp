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
    AppMessage()
    {
        m_data.reserve( JDKSAVDECC_APPDU_HEADER_LEN
                        + JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH );
        jdksavdecc_appdu_init( &m_app_header );
    }

    AppMessage( AppMessage const &other )
        : m_app_header( other.m_app_header ), m_data( other.m_data )
    {
        m_app_header.payload = &m_data[JDKSAVDECC_APPDU_HEADER_LEN];
    }

    AppMessage const &operator=( AppMessage const &other )
    {
        m_app_header = other.m_app_header;
        m_data = other.m_data;
        m_app_header.payload = &m_data[JDKSAVDECC_APPDU_HEADER_LEN];
        return *this;
    }

    void clear() { m_data.clear(); }

    ssize_t parse( uint8_t octet,
                   std::function<void(jdksavdecc_appdu const &)> handle_msg_cb,
                   std::function<void( ssize_t )> error_cb )
    {
        ssize_t r = 0;
        m_data.push_back( octet );

        if ( m_data.size() >= JDKSAVDECC_APPDU_HEADER_LEN
                              + JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH )
        {
            r = -1;
        }
        else
        {
            if ( m_data.size() >= JDKSAVDECC_APPDU_HEADER_LEN )
            {
                // validate header
                jdksavdecc_appdu appmsg;

                ssize_t r = jdksavdecc_appdu_read(
                    &appmsg, &m_data[0], 0, m_data.size() );
                if ( r > 0 )
                {
                    handle_msg_cb( appmsg );
                    clear();
                }
                else if ( r < 0 )
                {
                    if ( error_cb )
                    {
                        error_cb( r );
                    }
                }
            }
        }

        return r;
    }
    jdksavdecc_appdu m_app_header;
    std::vector<uint8_t> m_data;
};


}
