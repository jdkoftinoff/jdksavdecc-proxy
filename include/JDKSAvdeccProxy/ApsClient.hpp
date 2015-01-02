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
#pragma once

#include "World.hpp"

namespace JDKSAvdeccProxy
{

class AosAvdeccHandler;
class ApsAvdeccEvents;

class ApsClientHandler;
class ApsClientEvents;

class ApsconenctionHandler;
class ApsConnectionEvents;

class ApsAvdeccHandler
{
public:
//    ApsAvdeccHandler( unique_ptr<ApsAvdeccEvents> events ) : m_events( events ) {}
//    virtual ~ApsAvdeccHandler() {}

    ///
    /// \brief start
    /// Start the network service
    ///
    virtual void start();

    ///
    /// \brief stop
    /// Stop the network service
    ///
    virtual void stop();

    ///
    /// \brief onAvdeccFrame Avdecc data is received
    /// This method forwards the avdecc data to all ClientHandler objects
    ///
    virtual void onAvdeccFrame( Frame const &frame ) = 0;

    ///
    /// \brief sendAvdeccData Avdecc data needs to be sent to the Avdecc
    /// interface
    /// \param frame The Frame object containing the PDU and header
    ///
    virtual void sendAvdeccFrame( Frame const &frame ) = 0;

protected:

    unique_ptr<ApsAvdeccEvents> m_events;
};

class ApsAvdeccEvents
{
public:
//    ApsConnectionEvents() {}
//    virtual ~ApsConnectionEvents() {}

    ///
    /// \brief start
    /// Start the network service
    ///
    virtual void start() = 0;

    ///
    /// \brief stop
    /// Stop the network service
    ///
    virtual void stop() = 0;

    ///
    /// \brief onAvdeccFrame Avdecc data is received
    /// This method forwards the avdecc data to all ClientHandler objects
    ///
    virtual void onAvdeccFrame( Frame const &frame ) = 0;

    ///
    /// \brief sendAvdeccData Avdecc data needs to be sent to the Avdecc
    /// interface
    /// \param frame The Frame object containing the PDU and header
    ///
    virtual void sendAvdeccFrame( Frame const &frame ) = 0;

    ///
    /// \brief onAvdeccLinkChange
    /// callback when the AVDECC network link status changes
    ///
    /// \param link_up true if the network link is up, false otherwise
    ///
    virtual void onAvdeccLinkChange( bool link_up ) = 0;
};

class ApsClientEvents
{
public:
    ApsClientEvents() {}
    virtual ~ApsClientEvents() {}

    ///
    /// \brief start
    /// Start the network service
    ///
    virtual void start() = 0;

    ///
    /// \brief stop
    /// Stop the network service
    ///
    virtual void stop() = 0;

    virtual int onHttpMessageComplete(
            string const &method,
            string const &url,
            vector< pair< string, string > > const &headers
            ) = 0;

    ///
    /// \brief onAvdeccFrame Avdecc data is received
    /// This method forwards the avdecc data to all ClientHandler objects
    ///
    virtual void onAvdeccFrame( Frame const &frame ) = 0;

};

class ApsConnectionEvents
{
public:
    ApsConnectionEvents() {}
    virtual ~ApsConnectionEvents() {}

    ///
    /// \brief start
    /// Start the network service
    ///
    virtual void start() = 0;

    ///
    /// \brief stop
    /// Stop the network service
    ///
    virtual void stop() = 0;

    ///
    /// \brief onNewConnection A new TCP connection is accepted
    ///
    virtual void onNewConnection() = 0;

    ///
    /// \brief onAvdeccFrame Avdecc data is received
    /// This method forwards the avdecc data to all ClientHandler objects
    ///
    virtual void onAvdeccFrame( Frame const &frame ) = 0;

    ///
    /// \brief sendAvdeccData Avdecc data needs to be sent to the Avdecc
    /// interface
    /// \param frame The Frame object containing the PDU and header
    ///
    virtual void sendAvdeccFrame( Frame const &frame ) = 0;

    ///
    /// \brief onNopTimeout callback for the 10 second NOP timeout
    ///
    virtual void onNopTimeout() = 0;

    ///
    /// \brief onAvdeccLinkChange
    /// callback when the AVDECC network link status changes
    ///
    /// \param link_up true if the network link is up, false otherwise
    ///
    virtual void onAvdeccLinkChange( bool link_up ) = 0;

    ///
    /// \brief sendAppMessageToApc
    ///
    /// Put the AppMessage into the outgoing queues for all APCs
    ///
    /// \param msg
    ///
    virtual void sendAppMessageToApc( AppMessage const &msg ) = 0;

};


}
