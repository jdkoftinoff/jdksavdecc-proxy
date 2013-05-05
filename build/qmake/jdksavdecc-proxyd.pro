TOP= ../..

! include( common.pri ) {
  error( need common.pri file )
}

DEFINES+=US_ENABLE_RAW_ETHERNET=1
macx:DEFINES+=US_ENABLE_PCAP=1


TARGET=jdksavdecc-proxy

HEADERS +=  \
            $$TOP/tools/jdksavdecc-proxyd.h \
            $$MICROSUPPORT/include/us_world.h \
            $$MICROSUPPORT/include/us_options.h \
            $$MICROSUPPORT/include/us_allocator.h \
            $$MICROSUPPORT/include/us_allocator_heap.h \
            $$MICROSUPPORT/include/us_buffer.h \
            $$MICROSUPPORT/include/us_cgi.h \
            $$MICROSUPPORT/include/us_getopt.h \
            $$MICROSUPPORT/include/us_http.h \
            $$MICROSUPPORT/include/us_http_server.h \
            $$MICROSUPPORT/include/us_logger.h \
            $$MICROSUPPORT/include/us_logger_stdio.h \
            $$MICROSUPPORT/include/us_net.h \
            $$MICROSUPPORT/include/us_packet.h \
            $$MICROSUPPORT/include/us_packet_queue.h \
            $$MICROSUPPORT/include/us_parse.h \
            $$MICROSUPPORT/include/us_platform.h \
            $$MICROSUPPORT/include/us_print.h \
            $$MICROSUPPORT/include/us_queue.h \
            $$MICROSUPPORT/include/us_rawnet.h \
            $$MICROSUPPORT/include/us_reactor.h \
            $$MICROSUPPORT/include/us_reactor_handler_rawnet.h \
            $$MICROSUPPORT/include/us_reactor_handler_tcp.h \
            $$MICROSUPPORT/include/us_reactor_handler_udp.h \
            $$MICROSUPPORT/include/us_webapp.h


SOURCES +=  \
            $$TOP/src/jdksavdecc-proxy_world.c \
            $$TOP/src/jdksavdecc-proxy.c \
            $$TOP/src/jdksavdecc-proxy_server.c \
            $$TOP/src/jdksavdecc-proxy_app.c \
            $$TOP/src/jdksavdecc-proxy_aps.c \
            $$TOP/src/jdksavdecc-proxy_apc.c \
            $$TOP/tools/jdksavdecc-proxyd.c \
            $$MICROSUPPORT/src/us_allocator.c \
            $$MICROSUPPORT/src/us_allocator_heap.c \
            $$MICROSUPPORT/src/us_buffer.c \
            $$MICROSUPPORT/src/us_cgi.c \
            $$MICROSUPPORT/src/us_getopt.c \
            $$MICROSUPPORT/src/us_http.c \
            $$MICROSUPPORT/src/us_http_server.c \
            $$MICROSUPPORT/src/us_logger.c \
            $$MICROSUPPORT/src/us_logger_stdio.c \
            $$MICROSUPPORT/src/us_net.c \
            $$MICROSUPPORT/src/us_packet.c \
            $$MICROSUPPORT/src/us_packet_queue.c \
            $$MICROSUPPORT/src/us_parse.c \
            $$MICROSUPPORT/src/us_platform.c \
            $$MICROSUPPORT/src/us_print.c \
            $$MICROSUPPORT/src/us_queue.c \
            $$MICROSUPPORT/src/us_rawnet.c \
            $$MICROSUPPORT/src/us_reactor.c \
            $$MICROSUPPORT/src/us_reactor_handler_rawnet.c \
            $$MICROSUPPORT/src/us_reactor_handler_tcp.c \
            $$MICROSUPPORT/src/us_reactor_handler_udp.c \
            $$MICROSUPPORT/src/us_webapp.c





