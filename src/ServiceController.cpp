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

#include "JDKSAvdeccProxy/ServiceController.hpp"

namespace JDKSAvdeccProxy
{

ServiceController::ServiceController(
    Obbligato::Config::OptionGroups &option_groups )
    : m_option_groups( option_groups )
{
    m_proxy_settings.addOptions( option_groups, "avdecc_proxy" );

    m_loop = uv_default_loop();
}

ServiceController::~ServiceController() {}

bool ServiceController::init( int argc, const char **argv )
{
    bool r = true;

    m_proxy_settings.addOptions( m_option_groups, "avdecc_proxy" );

    if ( !m_option_groups.parse(
             argv + 1, "JDKSAvdeccProxyServer", "Version 0.3", std::cout ) )
    {
        throw std::runtime_error( "unable to parse settings" );
    }

    setupServerFiles();

    return r;
}

bool ServiceController::run()
{
    bool r = true;

    try
    {
        NetworkService service( m_proxy_settings, m_server_files, m_loop );
        service.start();

        uv_run( m_loop, UV_RUN_DEFAULT );

        service.stop();
    }
    catch ( std::runtime_error const &e )
    {
        ob_log_error( "exception: runtime_error caught: ", e.what() );
    }
    catch ( std::logic_error const &e )
    {
        ob_log_error( "exception: logic_error caught: ", e.what() );
    }
    catch ( std::exception const &e )
    {
        ob_log_error( "exception caught: ", e.what() );
    }

    return r;
}

void ServiceController::setupServerFiles()
{
    m_server_files["/"] = JDKSAvdeccProxy::HttpServerBlob(
        "text/html",
        "<html><head><title>Index</title></head><body><h1>Hello</h1></body>" );
}
}
