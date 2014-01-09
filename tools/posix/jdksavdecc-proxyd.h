#ifndef JDKSAVDECC_PROXYD_H
#define JDKSAVDECC_PROXYD_H

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
#include "jdksavdecc-proxy_server.h"
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

/**! Initialize the logger */
int proxyd_init_logger( void );

/**! Destroy the logger */
void proxyd_destroy_logger( void );

/**! destroy the global static memory allocator */
void proxyd_destroy_global_allocator(void);

/**! Initialize the global memory allocator */
int proxyd_init_global_allocator(void);

/**! Destroy the option lists */
void proxyd_destroy_options(void);

/**! Initialize the option lists, read a config file, and parse the command line */
int proxyd_init_options(const char *config_file, const char **argv);

/**! Run the proxy process */
void proxyd_run( void );

/**! The multiple rawnet management object */
extern us_rawnet_multi_t proxyd_net;

/**! The curret printer */
extern us_print_t *proxyd_printer;

/**! The global allocator which uses the simple allocator */
extern us_allocator_t *proxyd_global_allocator;

/**! The help option */
extern bool proxyd_option_help;

/**! The dump option */
extern bool proxyd_option_dump;

/**! The actual log level */
extern int16_t proxyd_option_log_level;

/**! The syslog option */
extern bool proxyd_option_syslog;

/**! The actual max static memory allocator size in K Bytes */
extern uint32_t proxyd_option_memory_length;

/**! The daemon option */
extern bool proxyd_option_daemon;

/**! The daemon's identity name for logging */
extern const char *proxyd_option_identity;

/**! The daemon's home dir */
extern const char *proxyd_option_homedir;

/**! The daemon's pidfile */
extern const char *proxyd_option_pidfile;

/**! The daemon's newuid */
extern const char *proxyd_option_newuid;

/**! The IP address to listen to */
extern const char *proxyd_option_address;

/**! The IP port to listen to */
extern const char *proxyd_option_port;

/**! The actual max per session memory allocator size in K Bytes */
extern uint32_t proxyd_option_session_memory_length;

/**! The current option collections */
extern us_getopt_t proxyd_option_parser;

extern us_getopt_option_t proxyd_global_options[];

/**! The options beggining with proxyd */
extern us_getopt_option_t proxyd_options[];


#endif
