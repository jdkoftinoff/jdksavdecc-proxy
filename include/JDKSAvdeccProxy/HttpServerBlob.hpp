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

class HttpServerBlobBase
{
public:
    virtual ~HttpServerBlobBase() {}
    virtual string const &getMimeType() const = 0;
    virtual uint8_t const *getContent() const = 0;
    virtual size_t getContentLength() const = 0;
};

class HttpServerBlobRaw : public HttpServerBlobBase
{
  public:
    HttpServerBlobRaw()
        : m_mime_type()
        , m_content( 0 )
        , m_content_length( 0 )
    {
    }

    HttpServerBlobRaw( string const &mime_type,
                       uint8_t const *content,
                       size_t content_length )
        : m_mime_type( mime_type )
        , m_content( content )
        , m_content_length( content_length )
    {
    }

    HttpServerBlobRaw( const HttpServerBlobRaw &other )
        : m_mime_type( other.m_mime_type )
        , m_content( other.m_content )
        , m_content_length( other.m_content_length )
    {
    }

    HttpServerBlobRaw &operator=( const HttpServerBlobRaw &other )
    {
        if ( this != &other )
        {
            m_content = other.m_content;
            m_mime_type = other.m_mime_type;
            m_content_length = other.m_content_length;
        }
        return *this;
    }

    virtual ~HttpServerBlobRaw()
    {
    }

    virtual string const &getMimeType() const
    {
        return m_mime_type;
    }

    virtual uint8_t const *getContent() const
    {
        return m_content;
    }

    virtual size_t getContentLength() const
    {
        return m_content_length;
    }

  private:
    string m_mime_type;
    uint8_t const *m_content;
    size_t m_content_length;
};

class HttpServerBlobVector : public HttpServerBlobBase
{
  public:
    HttpServerBlobVector()
        : m_mime_type()
        , m_content()
    {
    }

    HttpServerBlobVector( string const &mime_type,
                          vector<uint8_t> const *content
                          )
        : m_mime_type( mime_type )
        , m_content( content )
    {
    }

    HttpServerBlobVector( const HttpServerBlobVector &other )
        : m_mime_type( other.m_mime_type )
        , m_content( other.m_content )
    {
    }


    HttpServerBlobVector &operator=( const HttpServerBlobVector &other )
    {
        if ( this != &other )
        {
            m_content = other.m_content;
            m_mime_type = other.m_mime_type;
        }
        return *this;
    }

    virtual ~HttpServerBlobVector()
    {
    }

    virtual string const &getMimeType() const
    {
        return m_mime_type;
    }

    virtual uint8_t const *getContent() const
    {
        return m_content->data();
    }

    virtual size_t getContentLength() const
    {
        return m_content->size();
    }

  private:
    string m_mime_type;
    vector<uint8_t> const *m_content;
};

class HttpServerBlobString : public HttpServerBlobBase
{
  public:
    HttpServerBlobString()
        : m_mime_type()
        , m_content()
    {
    }

    HttpServerBlobString( string const &mime_type,
                          string content
                          )
        : m_mime_type( mime_type )
        , m_content( content )
    {
    }

    HttpServerBlobString( const HttpServerBlobString &other )
        : m_mime_type( other.m_mime_type )
        , m_content( other.m_content )
    {
    }


    HttpServerBlobString &operator=( const HttpServerBlobString &other )
    {
        if ( this != &other )
        {
            m_content = other.m_content;
            m_mime_type = other.m_mime_type;
        }
        return *this;
    }

    virtual ~HttpServerBlobString()
    {
    }

    virtual string const &getMimeType() const
    {
        return m_mime_type;
    }

    virtual uint8_t const *getContent() const
    {
        return reinterpret_cast<const uint8_t *>(m_content.data());
    }

    virtual size_t getContentLength() const
    {
        return m_content.length();
    }

  private:
    string m_mime_type;
    string m_content;
};

}
