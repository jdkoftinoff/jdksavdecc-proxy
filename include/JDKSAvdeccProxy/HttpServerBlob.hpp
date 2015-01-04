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
    static inline std::vector<uint8_t> load( std::string const &fname )
    {
        std::vector<uint8_t> r;
        std::ifstream f( fname, std::ios::binary | std::ios::in );
        f.seekg( 0, std::ios::end );
        r.resize( f.tellg() );
        f.read( (char *)r.data(), r.size() );
        return r;
    }


    HttpServerBlob( std::string const &mime_type, std::string const &content )
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
        , m_content(0)
        , m_content_length(0)
    {}

    HttpServerBlob( std::string const &mime_type,
                    uint8_t const * content,
                    size_t content_length )
        : m_mime_type( mime_type )
        , m_content( content )
        , m_content_length( content_length )
    {
    }

    HttpServerBlob( const HttpServerBlob &other )
        : m_mime_type( other.m_mime_type )
        , m_content( other.m_content )
        , m_content_length( other.m_content_length )
    {
    }

    HttpServerBlob &operator=( const HttpServerBlob &other )
    {
        m_mime_type = other.m_mime_type;
        m_content = other.m_content;
        m_content_length = other.m_content_length;
        return *this;
    }

    std::string m_mime_type;
    uint8_t const * m_content;
    size_t m_content_length;
};

}

