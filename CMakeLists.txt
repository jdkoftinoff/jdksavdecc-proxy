cmake_minimum_required (VERSION 2.8) 
project (jdksavdecc-proxy)
enable_testing()

set(LIBS ${LIBS} ${CHECK_LIBRARIES} jdksavdecc-proxy)

include_directories( "include" "microsupport/include" "jdksavdecc-c/include" )

file(GLOB_RECURSE JDKSAVDECC_PROXY_INCLUDES "include/*.h" )

file(GLOB_RECURSE JDKSAVDECC_PROXY_SRC "src/*.c" )

file(GLOB_RECURSE MICROSUPPORT_INCLUDES "microsupport/include/*.h" "microsupport/include/*/*.h"  )

file(GLOB_RECURSE JDKSAVDECC_C_INCLUDES "jdksavdecc-c/include/*.h" )

file(GLOB_RECURSE JDKSAVDECC_C_SRC "jdksavdecc-c/src/*.c" )

if(APPLE)
file(GLOB_RECURSE MICROSUPPORT_SRC "microsupport/src/*.c" "microsupport/src/win32/*.c" )
add_definitions(-DTARGET_PLATFORM_MACOSX=1 -DUS_ENABLE_RAW_ETHERNET -US_ENABLE_PCAP)
endif(APPLE)

if(UNIX)
file(GLOB_RECURSE MICROSUPPORT_SRC "microsupport/src/*.c" "microsupport/src/posix/*.c"  "microsupport/src/posix/*.h" )
add_definitions(-DTARGET_PLATFORM_POSIX=1 -DUS_ENABLE_SYSLOG)
endif(UNIX)

if( (NOT APPLE) AND UNIX)
add_definitions(-DTARGET_PLATFORM_LINUX=1 -DUS_ENABLE_RAW_ETHERNET)
endif((NOT APPLE) AND UNIX)

if(WIN32)
file(GLOB_RECURSE MICROSUPPORT_SRC "microsupport/src/*.c" "microsupport/src/macosx/*.c" )
endif(WIN32)

add_library (jdksavdecc-proxy ${JDKSAVDECC_PROXY_SRC} ${JDKSAVDECC_PROXY_INCLUDES} ${MICROSUPPORT_SRC} ${MICROSUPPORT_INCLUDES} ${JDKSAVDECC_C_SRC} ${JDKSAVDECC_C_INCLUDES})

INSTALL(TARGETS jdksavdecc-proxy DESTINATION "lib")

INSTALL(FILES
   ${JDKSAVDECC_PROXY_INCLUDES}
   ${MICROSUPPORT_INCLUDES}
   ${JDKSAVDECC_C_INCLUDES}
   DESTINATION include
)

link_directories( ${JDKSAVDECC_PROXY_BINARY_DIR} )

if(UNIX)
add_executable(jdksavdecc-proxyd tools/posix/jdksavdecc-proxyd)
target_link_libraries(jdksavdecc-proxyd ${LIBS})
endif(UNIX)

