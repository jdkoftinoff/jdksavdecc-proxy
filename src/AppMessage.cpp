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

AppMessage::AppMessage()
{
    jdksavdecc_fullappdu_init(&m_appdu);
    jdksavdecc_appdu_set_nop(&m_appdu.base);
}

AppMessage::AppMessage( const AppMessage &other )
    : m_appdu( other.m_appdu )
{
    m_appdu.base.payload = m_appdu.payload_buffer;
}

const AppMessage &AppMessage::operator=( const AppMessage &other )
{
    m_appdu = other.m_appdu;
    m_appdu.base.payload = m_appdu.payload_buffer;
    return *this;
}

void AppMessage::setNOP()
{
    jdksavdecc_appdu_set_nop(&m_appdu.base);
}

void AppMessage::setEntityIdRequest(
    const JDKSAvdeccMCU::Eui48 &apc_primary_mac,
    const JDKSAvdeccMCU::Eui64 &requested_entity_id )
{
    jdksavdecc_appdu_set_entity_id_request(&m_appdu.base,apc_primary_mac,requested_entity_id);
}

void AppMessage::setEntityIdResponse(
    const JDKSAvdeccMCU::Eui48 &apc_primary_mac,
    const JDKSAvdeccMCU::Eui64 &requested_entity_id )
{
    jdksavdecc_appdu_set_entity_id_response(&m_appdu.base,apc_primary_mac,requested_entity_id);
}

void AppMessage::setLinkUp( const JDKSAvdeccMCU::Eui48 &network_port_mac )
{
    jdksavdecc_appdu_set_link_up(&m_appdu.base,network_port_mac);
}

void AppMessage::setLinkDown( const JDKSAvdeccMCU::Eui48 &network_port_mac )
{
    jdksavdecc_appdu_set_link_down(&m_appdu.base,network_port_mac);
}

void AppMessage::setAvdeccFromAps( const JDKSAvdeccMCU::Frame &frame )
{
    jdksavdecc_appdu_set_avdecc_from_aps(&m_appdu.base,frame.getSA(),frame.getPayloadLength(),frame.getPayload());
}

void AppMessage::setAvdeccFromApc( const JDKSAvdeccMCU::Frame &frame )
{
    jdksavdecc_appdu_set_avdecc_from_apc(&m_appdu.base,frame.getSA(),frame.getPayloadLength(),frame.getPayload());
}

void AppMessage::setVendor( const JDKSAvdeccMCU::Eui48 &vendor_message_type,
                            const JDKSAvdeccMCU::FixedBuffer &payload )
{
    jdksavdecc_appdu_set_vendor(&m_appdu.base,vendor_message_type,payload.getLength(),payload.getBuf());
}

AppMessageParser::AppMessageParser()
{
}

ssize_t AppMessageParser::parse(AppMessage *destination_msg, uint8_t octet, std::function<void (const AppMessage &)> handle_msg_cb, std::function<void (ssize_t)> error_cb)
{
    ssize_t r = 0;

    if ( m_data.size() >= JDKSAVDECC_APPDU_HEADER_LEN
                          + JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH )
    {
        m_data.clear();
        m_data.push_back( octet );
        r = -1;
    }
    else
    {
        m_data.push_back( octet );

        if ( m_data.size() >= JDKSAVDECC_APPDU_HEADER_LEN )
        {
            ssize_t r = jdksavdecc_appdu_read(
                &destination_msg->m_appdu.base, &m_data[0], 0, m_data.size() );
            if ( r > 0 )
            {
                handle_msg_cb( *destination_msg );
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

}
