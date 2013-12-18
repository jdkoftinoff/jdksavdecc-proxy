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
#include "us_daemon.h"
#include "us_logger_syslog.h"
#include "jdksavdecc-proxyd.h"
#include "jdksavdecc_app.h"

#ifndef JDKSAVDECC_PROXYD_IDENTITY
#define JDKSAVDECC_PROXYD_IDENTITY "jdksavdecc-proxyd"
#endif

#ifndef JDKSAVDECC_PROXYD_CONFIG_FILE
/**! The global configuration file */
#define JDKSAVDECC_PROXYD_CONFIG_FILE "/etc/" JDKSAVDECC_PROXYD_IDENTITY ".conf"
#endif


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
const char proxyd_option_homedir_default[] = "/var/run/" JDKSAVDECC_PROXYD_IDENTITY;

/**! The daemon's home dir */
const char *proxyd_option_homedir = 0;

/**! The daemon's default pidfile */
const char proxyd_option_pidfile_default[] = "/var/run/jdksavdecc-proxyd/" JDKSAVDECC_PROXYD_IDENTITY ".pid";

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
    {"address", "IP address to listen to ",
        US_GETOPT_STRING, &proxyd_option_address_default, &proxyd_option_address},

    {"port", "IP port to listen to ",
        US_GETOPT_STRING, &proxyd_option_port_default, &proxyd_option_port},

    {"session", "Maximum session memory usage in KBytes",
        US_GETOPT_UINT32, &proxyd_option_session_memory_length_default, &proxyd_option_session_memory_length},

    {0, 0, 0, 0, 0}};

/**! Destroy the logger */
void proxyd_destroy_logger() { us_logger_finish(); }

/**! Initialize the logger */
int proxyd_init_logger() {
    us_logger_stdio_start(stdout, stdin);
    atexit(proxyd_destroy_logger);
    return 1;
}

/**! destroy the global static memory allocator */
void proxyd_destroy_global_allocator() {
    if (proxyd_global_allocator) {
        proxyd_global_allocator->destroy(proxyd_global_allocator);
    }

    free(proxyd_global_allocator_raw_memory);
}

/**! Initialize the global memory allocator */
int proxyd_init_global_allocator() {
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
void proxyd_destroy_options() {
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
    us_getopt_parse_args(&proxyd_option_parser, argv);

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
        r = 0;
        us_log_debug("Server Starting");

        /* Daemonize if necessary */
        us_daemon_daemonize(proxyd_option_daemon,
                            proxyd_option_identity,
                            proxyd_option_homedir,
                            proxyd_option_pidfile,
                            proxyd_option_newuid);

        /* Todo: Start the server */

        us_log_debug("Server Exiting");
    }

    proxyd_bootstrap_allocator.m_base.destroy( &proxyd_bootstrap_allocator.m_base );
    return r;
}
