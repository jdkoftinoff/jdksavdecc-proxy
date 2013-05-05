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

/**! The global configuration file */
#define JDKSAVDECC_PROXYD_CONFIG_FILE "/etc/jdksavdecc-proxyd.conf"

/**! The raw memory block used by the global allocator */
void *proxyd_global_allocator_raw_memory=0;

/**! The simple allocator which uses the raw memory */
us_simple_allocator_t proxyd_simple_allocator;

/**! The global allocator which uses the simple allocator */
us_allocator_t *proxyd_global_allocator=0;

/**! The default log level */
int16_t proxyd_option_log_level_default = 1;

/**! The actual log level */
int16_t proxyd_option_log_level;

/**! The default max static memory allocator size in K Bytes */
uint32_t proxyd_option_memory_length_default=32;

/**! The actual max static memory allocator size in K Bytes */
uint32_t proxyd_option_memory_length;


/**! The default max per session memory allocator size in K Bytes */
uint32_t proxyd_option_session_memory_length_default=32;

/**! The actual max per session memory allocator size in K Bytes */
uint32_t proxyd_option_session_memory_length;


/**! The current option collections */
us_getopt_t proxyd_options;


/**! The options beggining with proxyd */
us_getopt_option_t proxyd_option[] =
{
    { "detail", "Log detail level", US_GETOPT_INT16, &proxyd_option_log_level_default, &proxyd_option_log_level },
    { "memory", "Maximum memory usage in KBytes", US_GETOPT_UINT32, &proxyd_option_memory_length_default, &proxyd_option_memory_length },
    { "session", "Maximum session memory usage in KBytes", US_GETOPT_UINT32, &proxyd_option_session_memory_length_default, &proxyd_option_session_memory_length },
    { 0, 0, 0, 0, 0 }
};

/**! Destroy the logger */
void proxyd_destroy_logger()
{
    us_logger_finish();
}

/**! Initialize the logger */
int proxyd_init_logger()
{
    us_logger_stdio_start( stdout, stdin );
    atexit( proxyd_destroy_logger );
    return 1;
}

/**! destroy the global static memory allocator */
void proxyd_destroy_global_allocator()
{
    if( proxyd_global_allocator )
    {
        proxyd_global_allocator->destroy( proxyd_global_allocator );
    }

    free( proxyd_global_allocator_raw_memory );
}

/**! Initialize the global memory allocator */
int proxyd_init_global_allocator()
{
    int r=0;
    proxyd_global_allocator_raw_memory = (uint8_t *)calloc(proxyd_option_memory_length,1);
    if( proxyd_global_allocator_raw_memory )
    {
        us_simple_allocator_init(&proxyd_simple_allocator,proxyd_global_allocator_raw_memory,proxyd_option_memory_length*1024);
        proxyd_global_allocator = &proxyd_simple_allocator.m_base;
        atexit(proxyd_destroy_global_allocator);
        r=1;
    }
    return r;
}

/**! Destroy the option lists */
void proxyd_destroy_options()
{
    us_getopt_destroy(&proxyd_options);
}


/**! Initialize the option lists, read a config file, and parse the command line */
int proxyd_init_options(const char *config_file, const char **argv)
{
    us_getopt_init(&proxyd_options,proxyd_global_allocator);
    us_getopt_add_list(&proxyd_options,proxyd_option,"proxyd","JDKSAVDECC Proxy Daemon" );
    us_getopt_parse_file(&proxyd_options,config_file);
    us_getopt_parse_args(&proxyd_options,argv);
    atexit(proxyd_destroy_options);

    us_log_debug( "Parsed options" );

    return 1;
}


int main( int argc, const char **argv )
{
    (void)argc;
    int r=255;

    if( us_platform_init_sockets()
            && proxyd_init_global_allocator()
            && proxyd_init_logger()
            && proxyd_init_options(JDKSAVDECC_PROXYD_CONFIG_FILE,argv) )
    {
        r=0;
        us_log_debug("Server Starting");



        us_log_debug("Server Exiting");
    }

    return r;
}
