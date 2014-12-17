cmake_minimum_required (VERSION 2.8)
set(PROJECT jdksavdecc-proxy)
project(${PROJECT} C CXX)

set(CXX11 "ON")

add_subdirectory("libuv")
add_subdirectory("jdksavdecc-mcu")
add_subdirectory("Obbligato")
add_subdirectory("http-parser")


if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif()

include_directories(
    "include"
    "http-parser"
    "Obbligato/include"
    "libuv/include"
    "jdksavdecc-mcu/include"
    "jdksavdecc-mcu/jdksavdecc-c/include"
)

set(LIBS ${LIBS} jdksavdecc-mcu uv Obbligato http-parser)

if( WIN32 )
    include_directories($ENV{PCAP_INCLUDE})
    set(LIBS ${LIBS} wpcap psapi)
    link_directories($ENV{PCAP_LIBRARY})
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set(LIBS ${LIBS} pcap)
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

    
