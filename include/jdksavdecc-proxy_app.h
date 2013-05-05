#ifndef JDKSAVDECC_PROXY_APP_H
#define JDKSAVDECC_PROXY_APP_H

/*
 Copyright (c) 2013, J.D. Koftinoff Software, Ltd. <jeffk@jdkoftinoff.com>
 http://www.jdkoftinoff.com/
 All rights reserved.

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "jdksavdecc-proxy_world.h"

#ifndef JDKSAVDECC_PROXY_APP_ENABLE_US
# define JDKSAVDECC_PROXY_APP_ENABLE_US (1)
#endif

/**! Proxy APP PDU message types. See IEEE 1722.1 Annex C.3 */
enum JDKSAvdeccProxyMessageType_e
{
    JDKSAVDECCPROXY_MSG_NOP=0x00,
    JDKSAVDECCPROXY_MSG_ENTITY_ID_REQUEST=0x01,
    JDKSAVDECCPROXY_MSG_ENTITY_ID_RESPONSE=0x02,
    JDKSAVDECCPROXY_MSG_LINK_UP=0x03,
    JDKSAVDECCPROXY_MSG_LINK_DOWN=0x04,
    JDKSAVDECCPROXY_MSG_AVDECC_FROM_APS=0x05,
    JDKSAVDECCPROXY_MSG_AVDECC_FROM_APC=0x06,
    JDKSAVDECCPROXY_MSG_VENDOR=0xff
};

typedef enum JDKSAvdeccProxyMessageType_e JDKSAvdeccProxyMessageType;

/**! Proxy APP PDU. See IEEE 1722.1 Annex C.4 */
struct JDKSAvdeccProxyPDU_s
{
    uint8_t m_version;
    JDKSAvdeccProxyMessageType m_message_type;
    uint16_t m_payload_length;
    uint8_t m_address[6];
    uint16_t m_reserved;
    uint8_t m_payload[1500];
};

typedef struct JDKSAvdeccProxyPDU_s JDKSAvdeccProxyPDU;


/** Initialize a ProxyPDU to be blank */
void JDKSAvdeccProxyPDU_init( JDKSAvdeccProxyPDU *self );


#if JDKSAVDECC_PROXY_APP_ENABLE_US==1

/** Parse a ProxyPDU from a buffer.
 *  returns true on success
 */

bool JDKSAvdeccProxyPDU_parse_buffer( JDKSAvdeccProxyPDU *self, us_buffer_t *buffer );

/** Flatten a ProxyPDU into a buffer. Returns true on success */
bool JDKSAvdeccProxyPDU_flatten_to_buffer( JDKSAvdeccProxyPDU const *self, us_buffer_t *buffer );


#endif



/** Parse a ProxyPDU from an array.
 *  returns number of bytes parsed, or 0 if incomplete or invalid
 */
size_t JDKSAvdeccProxyPDU_parse_array( JDKSAvdeccProxyPDU *self, uint8_t *p, size_t available_bytes );

/** Flatten a ProxyPDU into an array. returns length of stored data or 0 on error or array full */
size_t JDKSAvdeccProxyPDU_flatten_to_array( JDKSAvdeccProxyPDU const *self, uint8_t *p, size_t max_size );


/** Extract a 64 bit entity_id from the payload, or null if it has none */
uint8_t const *JDKSAvdeccProxyPDU_get_entity_id_from_payload( JDKSAvdeccProxyPDU const *self );


/** Create and flatten a NOP message */
bool JDKSAvdeccProxyPDU_set_NOP( JDKSAvdeccProxyPDU *self, us_buffer_t *buffer );

/** Create an ENTITY_ID_REQUEST message */
bool JDKSAvdeccProxyPDU_set_ENTITY_ID_REQUEST( JDKSAvdeccProxyPDU *self, uint8_t const apc_primary_mac[6], uint8_t const entity_id[8] );

/** Create an ENTITY_ID_RESPONSE message */
bool JDKSAvdeccProxyPDU_set_ENTITY_ID_RESPONSE( JDKSAvdeccProxyPDU *self, uint8_t const apc_primary_mac[6], uint8_t const entity_id[8] );

/** Create a LINK_UP message */
bool JDKSAvdeccProxyPDU_set_LINK_UP( JDKSAvdeccProxyPDU *self, uint8_t const network_port_mac[6] );

/** Create a LINK_DOWN message */
bool JDKSAvdeccProxyPDU_set_LINK_DOWN( JDKSAvdeccProxyPDU *self, uint8_t const network_port_mac[6] );

/** Create an AVDECC_FROM_APS message */
bool JDKSAvdeccProxyPDU_set_AVDECC_FROM_APS( JDKSAvdeccProxyPDU *self, uint8_t const original_source_address[6], uint16_t payload_length, uint8_t const *payload );

/** Create a AVDECC_FROM_APC message */
bool JDKSAvdeccProxyPDU_set_AVDECC_FROM_APC( JDKSAvdeccProxyPDU *self, uint8_t const destination_address[6], uint16_t payload_length, uint8_t const *payload );

/** Create a VENDOR message */
bool JDKSAvdeccProxyPDU_set_VENDOR( JDKSAvdeccProxyPDU *self, uint8_t const vendor_message_type[6], uint16_t payload_length, uint8_t const *payload );


/**! Acceptable subtypes for Proxy. See Annex C.4.5 */
enum JDKSAvdeccProxySubtype_e
{
    JDKSAVDECCPROXY_SUBTYPE_ADP = 0x7a,
    JDKSAVDECCPROXY_SUBTYPE_AECP = 0x7b,
    JDKSAVDECCPROXY_SUBTYPE_ACMP = 0x7c
};

typedef enum JDKSAvdeccProxySubtype_e JDKSAvdeccProxySubtype;


#endif
