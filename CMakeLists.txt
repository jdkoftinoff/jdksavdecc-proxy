cmake_minimum_required (VERSION 2.8)
set(PROJECT jdksavdecc-proxy)

IF(BIICODE)
    ADD_BIICODE_TARGETS()
ELSE(NOT BIICODE)

    set(PROJECT jdksavdecc-proxy)
    project(${PROJECT} C CXX)

    set(CXX11 "OFF")
    set(LIBUV "ON")

    option(JDKSAVDECC_PROXY_STANDALONE "JDKSAVDECC Proxy Standalone Build" "ON")

    if( JDKSAVDECC_PROXY_STANDALONE )

        add_subdirectory("libuv")
        add_subdirectory("uvrawpkt")
        add_subdirectory("jdksavdecc-mcu")
        add_subdirectory("Obbligato")
        add_subdirectory("http-parser")
        include_directories(
            "include"
            "Obbligato/include"
            "uvrawpkt/include"
            "libuv/include"
            "http-parser"
            "jdksavdecc-mcu/include"
            "jdksavdecc-mcu/jdksavdecc-c/include"
        )

    else()

        include_directories(
            "include"
            "../Obbligato/include"
            "../uvrawpkt/include"
            "../libuv/include"
            "../http-parser/http-parser"
            "../jdksavdecc-mcu/include"
            "../jdksavdecc-mcu/jdksavdecc-c/include"
        )

    endif()

    if( NOT CMAKE_BUILD_TYPE )
      set( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
           "Choose the type of build, options are: None Debug Release RelWithDebInfo
    MinSizeRel."
           FORCE )
    endif()

    set(LIBS ${LIBS} jdksavdecc-proxy Obbligato jdksavdecc-mcu uv uvrawpkt)

    if( WIN32 )
        include_directories($ENV{PCAP_INCLUDE})
        set(LIBS ${LIBS} wpcap psapi)
        link_directories($ENV{PCAP_LIBRARY})
    elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
        set(LIBS ${LIBS} pcap )
    elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
       set( LIBS ${LIBS} pcap m rt )
    endif()


    INCLUDE (common.cmake)

    if( WIN32 )
        add_executable(JDKSAvdeccProxyServer "tools/Win32/JDKSAvdeccProxyServer.cpp")
        target_link_libraries(JDKSAvdeccProxyServer ${LIBS})
    elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
        add_executable(JDKSAvdeccProxyServer "tools/MacOSX/JDKSAvdeccProxyServer.cpp")
        target_link_libraries(JDKSAvdeccProxyServer ${LIBS})
    elseif( UNIX )
        add_executable(JDKSAvdeccProxyServer "tools/Linux/JDKSAvdeccProxyServer.cpp")
        target_link_libraries(JDKSAvdeccProxyServer ${LIBS})
    endif()
ENDIF(BIICODE)

    
