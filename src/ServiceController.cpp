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

void ServiceController::Settings::addOptions(
    Obbligato::Config::OptionGroups &options,
    const std::string &options_prefix )
{
    m_proxy_settings.addOptions( options, options_prefix + "avdecc_proxy" );
}

ServiceController::ServiceController( Settings const &settings,
                                      uv_loop_t *loop )
    : m_settings( settings ), m_loop( loop )
{
    setupServerFiles();

    m_service.reset ( new NetworkService(
        m_settings.m_proxy_settings, m_server_content, m_loop ) );
    m_service->start();
}

ServiceController::~ServiceController()
{
    if ( m_service.get() )
    {
        m_service->stop();
        m_service.reset();
    }
}

bool ServiceController::run()
{
    try
    {
        uv_run( m_loop, UV_RUN_ONCE );
    }
    catch ( std::runtime_error const &e )
    {
        ob_log_error( "exception: runtime_error caught: ", e.what() );
        return false;
    }
    catch ( std::logic_error const &e )
    {
        ob_log_error( "exception: logic_error caught: ", e.what() );
        return false;
    }
    catch ( std::exception const &e )
    {
        ob_log_error( "exception caught: ", e.what() );
        return false;
    }
    return uv_loop_alive( m_loop ) != 0 ? true : false;
}

void ServiceController::setupServerFiles() { m_server_content.load(); }
}
