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
#include "jdksavdecc-proxyd.h"

/**! The multiple rawnet management object */
us_rawnet_multi_t proxyd_net;

/**! The printer pointing to stdout */
us_print_file_t proxyd_stdout_printer;

/**! The curret printer */
us_print_t *proxyd_printer = 0;

/**! The raw memory block used by the global allocator */
void *proxyd_global_allocator_raw_memory = 0;

/**! The malloc allocator for bootstrapping */
us_malloc_allocator_t proxyd_bootstrap_allocator;

/**! The simple allocator which uses the raw memory */
us_simple_allocator_t proxyd_simple_allocator;

/**! The global allocator which uses the simple allocator */
us_allocator_t *proxyd_global_allocator = 0;

/**! The help option defaults off */
bool proxyd_option_help_default=false;

/**! The help option */
bool proxyd_option_help;

/**! The dump option defaults off */
bool proxyd_option_dump_default=false;

/**! The dump option */
bool proxyd_option_dump;

/**! The default log level */
int16_t proxyd_option_log_level_default = US_LOG_LEVEL_INFO;

/**! The actual log level */
int16_t proxyd_option_log_level;

/**! The syslog option defaults off */
bool proxyd_option_syslog_default=false;

/**! The syslog option */
bool proxyd_option_syslog;

/**! The default max static memory allocator size in K Bytes */
uint32_t proxyd_option_memory_length_default = 32;

/**! The actual max static memory allocator size in K Bytes */
uint32_t proxyd_option_memory_length;

/**! The daemon option defaults off */
bool proxyd_option_daemon_default=false;

/**! The daemon option */
bool proxyd_option_daemon;

/**! The daemon's default identity name for logging */
const char proxyd_option_identity_default[] = JDKSAVDECC_PROXYD_IDENTITY;

/**! The daemon's identity name for logging */
const char *proxyd_option_identity = 0;

/**! The daemon's default home dir */
const char proxyd_option_homedir_default[] = JDKSAVDECC_PROXYD_PID_PATH_PREFIX "/";

/**! The daemon's home dir */
const char *proxyd_option_homedir = 0;

/**! The daemon's default pidfile */
const char proxyd_option_pidfile_default[] = JDKSAVDECC_PROXYD_PID_PATH_PREFIX "/" JDKSAVDECC_PROXYD_IDENTITY ".pid";

/**! The daemon's pidfile */
const char *proxyd_option_pidfile = 0;

/**! The daemon's default newuid */
const char proxyd_option_newuid_default[] = "root";

/**! The daemon's newuid */
const char *proxyd_option_newuid = 0;


/**! The default IP address to listen to */
const char proxyd_option_address_default[] = "127.0.0.1";

/**! The IP address to listen to */
const char *proxyd_option_address = 0;

/**! The default IP port to listen to */
const char proxyd_option_port_default[] = JDKSAVDECC_APPDU_TCP_PORT_STRING;

/**! The IP port to listen to */
const char *proxyd_option_port = 0;

/**! The default ethernet interface to use for AVDECC frames */
const char proxyd_option_avbif_default[] = JDKSAVDECC_PROXYD_AVBIF_DEFAULT;

/**! The actual ethernet interface to use for AVDECC frames */
const char *proxyd_option_avbif = 0;

/**! The default maximum number of proxy clients via TCP */
uint16_t proxyd_option_maxclients_default = 4;

/**! The actual maximum number of proxy clients via TCP */
uint16_t proxyd_option_maxclients;

/**! The actual max per session memory allocator size in K Bytes */
uint32_t proxyd_option_session_memory_length;

/**! The default max per session memory allocator size in K Bytes */
uint32_t proxyd_option_session_memory_length_default = 32;

/**! The actual max per session memory allocator size in K Bytes */
uint32_t proxyd_option_session_memory_length;


/**! The current option collections */
us_getopt_t proxyd_option_parser;

us_getopt_option_t proxyd_global_options[]
= {
    {"help", "Get list of options",
        US_GETOPT_FLAG, &proxyd_option_help_default, &proxyd_option_help },

    {"dump", "Dump all current options",
        US_GETOPT_FLAG, &proxyd_option_dump_default, &proxyd_option_dump },

    {"detail", "Log detail level (0=none, 1=errors, 2=warnings, 3=info, 4=debug, 5=trace)",
        US_GETOPT_UINT16, &proxyd_option_log_level_default, &proxyd_option_log_level},

    {"syslog", "Log via syslog",
        US_GETOPT_FLAG, &proxyd_option_syslog_default, &proxyd_option_syslog},

    {"memory","Maximum memory usage in KBytes",
        US_GETOPT_UINT32, &proxyd_option_memory_length_default, &proxyd_option_memory_length},

    {"identity", "Identity",
        US_GETOPT_STRING, &proxyd_option_identity_default, &proxyd_option_identity},

    {"home", "Home directory",
        US_GETOPT_STRING, &proxyd_option_homedir_default, &proxyd_option_homedir},

    {"pidfile", "pid file name",
        US_GETOPT_STRING, &proxyd_option_pidfile_default, &proxyd_option_pidfile},

    {"newuid", "New user ID",
        US_GETOPT_STRING, &proxyd_option_newuid_default, &proxyd_option_newuid},

    {0, 0, 0, 0, 0}};

/**! The options beggining with proxyd */
us_getopt_option_t proxyd_options[]
= {
    {"address", "IP address to listen on",
        US_GETOPT_STRING, &proxyd_option_address_default, &proxyd_option_address},

    {"port", "IP port to listen on",
        US_GETOPT_STRING, &proxyd_option_port_default, &proxyd_option_port},

    {"avbport", "The AVB ethernet interface to use for AVDECC messages",
        US_GETOPT_STRING, &proxyd_option_avbif_default, &proxyd_option_avbif},

    {"maxclients", "Maximum number of simultaneous proxy clients",
        US_GETOPT_UINT16, &proxyd_option_maxclients_default, &proxyd_option_maxclients},

    {"session", "Maximum session memory usage in KBytes",
        US_GETOPT_UINT32, &proxyd_option_session_memory_length_default, &proxyd_option_session_memory_length},

    {0, 0, 0, 0, 0}};

/**! Destroy the logger */
void proxyd_destroy_logger( void ) { us_logger_finish(); }

/**! Initialize the logger */
int proxyd_init_logger( void ) {
    us_logger_stdio_start(stdout, stdin);
    atexit(proxyd_destroy_logger);
    return 1;
}

/**! destroy the global static memory allocator */
void proxyd_destroy_global_allocator(void) {
    if (proxyd_global_allocator) {
        proxyd_global_allocator->destroy(proxyd_global_allocator);
    }

    free(proxyd_global_allocator_raw_memory);
}

/**! Initialize the global memory allocator */
int proxyd_init_global_allocator(void) {
    int r = 0;
    proxyd_global_allocator_raw_memory = (uint8_t *)calloc(proxyd_option_memory_length, 1);
    if (proxyd_global_allocator_raw_memory) {
        us_simple_allocator_init(
            &proxyd_simple_allocator, proxyd_global_allocator_raw_memory, proxyd_option_memory_length * 1024);
        proxyd_global_allocator = &proxyd_simple_allocator.m_base;
        atexit(proxyd_destroy_global_allocator);
        r = 1;
    }
    return r;
}

/**! Destroy the option lists */
void proxyd_destroy_options(void) {
    us_getopt_destroy(&proxyd_option_parser);
}

/**! Initialize the option lists, read a config file, and parse the command line */
int proxyd_init_options(const char *config_file, const char **argv) {
    int r=1;
    /* Initialize the options manager */
    us_getopt_init(&proxyd_option_parser, &proxyd_bootstrap_allocator.m_base);

    /* Add the help and dump options with no prefix */
    us_getopt_add_list(&proxyd_option_parser, proxyd_global_options, 0, "" );

    /* Add the proxyd options with the proxyd prefix */
    us_getopt_add_list(&proxyd_option_parser, proxyd_options, "proxyd", "JDKSAVDECC Proxy Daemon");

    /* fill in the defaults and finalize the option lists */
    us_getopt_fill_defaults(&proxyd_option_parser);

    /* Try parse the global config file if any */
    us_getopt_parse_file(&proxyd_option_parser, config_file);

    /* Parse the command line arguments */
    us_getopt_parse_args(&proxyd_option_parser, argv+1);

    /* Remember to destroy the option list at exit */
    atexit(proxyd_destroy_options);

    us_log_debug("Parsed options");

    /* If the user is asking to dump options, then dump the values and exit */
    if( proxyd_option_dump ) {
        us_getopt_dump(&proxyd_option_parser, proxyd_printer, "dump" );
        r=0;
    }

    /* If the user is asking for options, then show the options and exit */
    if( proxyd_option_help ) {
        us_getopt_print(&proxyd_option_parser, proxyd_printer );
        r=0;
    }

    /* Set the logger detail level from the options */
    us_log_level = proxyd_option_log_level;

    /* re-initialize the logger to use syslog if asked to */
    if( proxyd_option_syslog ) {
        us_logger_syslog_start( proxyd_option_identity );
    }

    /* Initialize the global allocator now that we know how much storage to give it */
    proxyd_init_global_allocator();
    return r;
}


/** Each avdeccproxy_http_server_handler_t is used to keep track of the
 *  conversations with one avdecc proxy client
 */
typedef struct avdeccproxy_http_server_handler_s {
    us_http_server_handler_t m_base;
    us_reactor_handler_rawnet_t *m_rawnet;
} avdeccproxy_http_server_handler_t;


typedef struct avdeccproxy_http_server_handler_params_s {
    us_webapp_director_t *web_director;
    us_reactor_handler_rawnet_t *rawnet;
    void *extra;
} avdeccproxy_http_server_handler_params_t;


/** avdeccproxy_http_server_handler_create is called to allocate the storage for
 *  an avdecc_http_server_handler.
 */
us_reactor_handler_t *avdeccproxy_http_server_handler_create(us_allocator_t *allocator) {
    return (us_reactor_handler_t *)us_new(allocator, avdeccproxy_http_server_handler_t);
}


/** avdeccproxy_http_server_handler_init is called whenever an accepted incoming TCP connection
 *  occurs. TODO: use session allocator.
 */
bool avdeccproxy_http_server_handler_init(
                                 us_reactor_handler_t *self_,
                                 us_allocator_t *allocator,
                                 int fd,
                                 void *extra
                                 ) {
    bool r=false;
    avdeccproxy_http_server_handler_t *self = (avdeccproxy_http_server_handler_t *)self_;
    avdeccproxy_http_server_handler_params_t *params = (avdeccproxy_http_server_handler_params_t *)extra;

    if( us_http_server_handler_init(
        &self->m_base.m_base.m_base,
        allocator,
        fd,
        0,
        8192,
        params->web_director ) ) {
            self->m_rawnet = params->rawnet;
            // TODO: add hook to support CONNECT method, and state for handling avdecc proxy protocol state machine, and tick.
            r=true;
    }
    return r;
}

/** avdeccproxy_http_server_init is called to initialize the tcp server with the appropriate parameters.
 *  It is called by the reactor once for every listening port/interface/ipv4/ipv6 file descriptor 
 *  that is used
 */
bool avdeccproxy_http_server_init (
    us_reactor_handler_t *self,
    us_allocator_t *allocator,
    int fd,
    void *extra) {
    return us_reactor_handler_tcp_server_init (
               self,
               allocator,
               fd,
               extra,
               avdeccproxy_http_server_handler_create,  /* The function to call to create the client handler object     */
               avdeccproxy_http_server_handler_init     /* The function to call to initialize the client handler object */
           );
}


/** Receive a frame from the ethernet port */
bool proxyd_avtp_packet_received(
    us_reactor_handler_rawnet_t *self,
    const us_packet_t *packet,
    us_packet_queue_t *outgoing_queue) {
    us_log_debug( "Received AVTP Frame size %d",packet->m_length);
    // TODO: pass packet to all active avdecc_http server client handlers
    return true;
}


/**! Run the proxy process */
bool proxyd_run( void ) {
    bool r = false;
    avdeccproxy_http_server_handler_params_t params;
    us_reactor_handler_rawnet_t *rh;
    us_webapp_director_t director;
    us_webapp_director_init( &director, proxyd_global_allocator );
    us_reactor_t reactor;
    r = us_reactor_init (
            &reactor,
            proxyd_global_allocator,
            proxyd_option_maxclients
        );
    if ( r ) {
        us_reactor_handler_t *h=us_reactor_handler_rawnet_create(proxyd_global_allocator);
        r=false;
        if( h ) {
            rh = (us_reactor_handler_rawnet_t *)h;
            if( us_reactor_handler_rawnet_init(
                    r,
                    proxyd_global_allocator,
                    0,
                    proxyd_option_avbif,
                    JDKSAVDECC_AVTP_ETHERTYPE,
                    jdksavdecc_multicast_adp_acmp.value,
                    JDKSPROXYD_AVTP_QUEUE_SIZE,
                    JDKSPROXYD_AVTP_QUEUE_SIZE ) ) {
                rh->packet_received = proxyd_avtp_packet_received;
                us_reactor_add_item(&reactor, h);
            }
        }
    }
    if ( r ) {
        params.extra = 0;
        params.rawnet = rh;
        params.web_director = &director;
        r = us_reactor_create_server (
                &reactor,
                proxyd_global_allocator,
                0, JDKSAVDECC_APPDU_TCP_PORT_STRING,
                SOCK_STREAM,
                &params,
                us_reactor_handler_tcp_server_create,
                avdeccproxy_http_server_handler_init
            );
    }
    if ( r ) {
        while ( !us_platform_sigterm_seen
            && reactor.poll ( &reactor, 2000 ) ) {
            fprintf ( stdout, "tick\n" );
        }
        reactor.destroy ( &reactor );
        r = true;
    }
    director.destroy( &director );
    return r;

}


int main(int argc, const char **argv) {
    (void)argc;
    int r = 255;

    /* create a bootstrap allocator for allocations during setup */
    us_malloc_allocator_init( &proxyd_bootstrap_allocator );

    /* create a printer object tied to stdout for console access */
    us_print_file_init(&proxyd_stdout_printer, stdout );
    proxyd_printer = &proxyd_stdout_printer.m_base;

    /* Initialize sockets, the logger, and parse the options */
    if (us_platform_init_sockets() && proxyd_init_logger()
        && proxyd_init_options(JDKSAVDECC_PROXYD_CONFIG_FILE, argv)) {

        /* Open all ethernet ports for AVTP frames and join ADP/ACMP and identification multicast MAC addresses */
        if( us_rawnet_multi_open(
            &proxyd_net,
            JDKSAVDECC_AVTP_ETHERTYPE,
            jdksavdecc_multicast_adp_acmp.value,
            jdksavdecc_multicast_identification.value) ) {

            /* Also join the JDKS LOG multicast address */
            us_rawnet_multi_join_multicast(
                &proxyd_net,
                jdksavdecc_jdks_multicast_log.value );

            r = 0;
            us_log_debug("Server Starting");

            /* Daemonize if necessary */
            us_daemon_daemonize(proxyd_option_daemon,
                                proxyd_option_identity,
                                proxyd_option_homedir,
                                proxyd_option_pidfile,
                                proxyd_option_newuid);

            /* Run the server */
            if( !proxyd_run() ) {
                us_log_error("Unable to start server");
            }


            us_log_debug("Server Exiting");

            /* Close all the ethernet port */
            us_rawnet_multi_close(&proxyd_net);
        }
    }

    proxyd_bootstrap_allocator.m_base.destroy( &proxyd_bootstrap_allocator.m_base );
    return r;
}
