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

class HttpServerBlob
{
  public:
#if 0
    static inline vector<uint8_t> load( string const &fname )
    {
        vector<uint8_t> r;
        ifstream f( fname, ios::binary | ios::in );
        f.seekg( 0, ios::end );
        r.resize( f.tellg() );
        f.read( (char *)r.data(), r.size() );
        return r;
    }


    HttpServerBlob( string const &mime_type, string const &content )
        : m_mime_type( mime_type ), m_content( content.length() )
    {
        for ( size_t i = 0; i < content.length(); ++i )
        {
            m_content[i] = content[i];
        }
    }
#endif

    HttpServerBlob()
        : m_mime_type()
        , m_content( 0 )
        , m_content_length( 0 )
        , m_allocated( false )
    {
    }

    HttpServerBlob( string const &mime_type,
                    uint8_t const *content,
                    size_t content_length,
                    bool allocated = false )
        : m_mime_type( mime_type )
        , m_content( const_cast<uint8_t *>( content ) )
        , m_content_length( content_length )
        , m_allocated( allocated )
    {
    }

    HttpServerBlob( const HttpServerBlob &other )
        : m_mime_type( other.m_mime_type )
    {
        if ( other.m_allocated )
        {
            m_allocated = true;
            m_content = new uint8_t[other.m_content_length];
            memcpy( m_content, other.m_content, other.m_content_length );
        }
        else
        {
            m_allocated = false;
            m_content = other.m_content;
        }
        m_mime_type = other.m_mime_type;
        m_content_length = other.m_content_length;
    }

    HttpServerBlob( string const &mime_type, string const &content )
        : m_mime_type( mime_type )
        , m_content( new uint8_t[content.length()] )
        , m_content_length( content.length() )
        , m_allocated( true )
    {
    }

    HttpServerBlob( string const &mime_type, vector<uint8_t> const &content )
        : m_mime_type( mime_type )
        , m_content( new uint8_t[content.size()] )
        , m_content_length( content.size() )
        , m_allocated( true )
    {
    }

    HttpServerBlob &operator=( const HttpServerBlob &other )
    {
        if ( this != &other )
        {
            if ( m_allocated )
            {
                delete m_content;
            }
            if ( other.m_allocated )
            {
                m_allocated = true;
                m_content = new uint8_t[other.m_content_length];
                memcpy( m_content, other.m_content, other.m_content_length );
            }
            else
            {
                m_allocated = false;
                m_content = other.m_content;
            }
            m_mime_type = other.m_mime_type;
            m_content_length = other.m_content_length;
        }
        return *this;
    }

    virtual ~HttpServerBlob()
    {
        if ( m_allocated )
        {
            delete m_content;
        }
    }

    string m_mime_type;
    uint8_t *m_content;
    size_t m_content_length;
    bool m_allocated;
};
}
