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
#include "jdksavdecc-proxy_app.h"

void JDKSAvdeccProxyPDU_init(JDKSAvdeccProxyPDU *self) {
    int i;
    self->m_version = 0;
    self->m_message_type = 0;
    self->m_payload_length = 0;
    for (i = 0; i < 6; ++i) {
        self->m_address[i] = 0;
    }
    self->m_reserved = 0;
}

#if JDKSAVDECC_PROXY_APP_ENABLE_US == 1

bool JDKSAvdeccProxyPDU_parse_buffer(JDKSAvdeccProxyPDU *self, us_buffer_t *buffer) {
    bool r = false;
    if (us_buffer_readable_count(buffer) >= 12) {
        size_t pos = buffer->m_next_out;
        self->m_version = us_buffer_read_byte(buffer);
        self->m_message_type = us_buffer_read_byte(buffer);
    }
    return r;
}

bool JDKSAvdeccProxyPDU_flatten_to_buffer(JDKSAvdeccProxyPDU const *self, us_buffer_t *buffer) {}

#endif

size_t JDKSAvdeccProxyPDU_parse_array(JDKSAvdeccProxyPDU *self, uint8_t *p, size_t available_bytes) {}

size_t JDKSAvdeccProxyPDU_flatten_to_array(JDKSAvdeccProxyPDU const *self, uint8_t *p, size_t max_size) {}

uint8_t const *JDKSAvdeccProxyPDU_get_entity_id_from_payload(JDKSAvdeccProxyPDU const *self) {}

bool JDKSAvdeccProxyPDU_set_NOP(JDKSAvdeccProxyPDU *self, us_buffer_t *buffer) {}

bool JDKSAvdeccProxyPDU_set_ENTITY_ID_REQUEST(JDKSAvdeccProxyPDU *self,
                                              uint8_t const apc_primary_mac[6],
                                              uint8_t const entity_id[8]) {}

bool JDKSAvdeccProxyPDU_set_ENTITY_ID_RESPONSE(JDKSAvdeccProxyPDU *self,
                                               uint8_t const apc_primary_mac[6],
                                               uint8_t const entity_id[8]) {}

bool JDKSAvdeccProxyPDU_set_LINK_UP(JDKSAvdeccProxyPDU *self, uint8_t const network_port_mac[6]) {}

bool JDKSAvdeccProxyPDU_set_LINK_DOWN(JDKSAvdeccProxyPDU *self, uint8_t const network_port_mac[6]) {}

bool JDKSAvdeccProxyPDU_set_AVDECC_FROM_APS(JDKSAvdeccProxyPDU *self,
                                            uint8_t const original_source_address[6],
                                            uint16_t payload_length,
                                            uint8_t const *payload) {}

bool JDKSAvdeccProxyPDU_set_AVDECC_FROM_APC(JDKSAvdeccProxyPDU *self,
                                            uint8_t const destination_address[6],
                                            uint16_t payload_length,
                                            uint8_t const *payload) {}

bool JDKSAvdeccProxyPDU_set_VENDOR(JDKSAvdeccProxyPDU *self,
                                   uint8_t const vendor_message_type[6],
                                   uint16_t payload_length,
                                   uint8_t const *payload) {}
