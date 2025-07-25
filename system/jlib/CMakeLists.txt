################################################################################
#    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
################################################################################

# Component: jlib
#####################################################
# Description:
# ------------
#    Cmake Input File for jlib
#####################################################

project( jlib )

INCLUDE(CheckLibraryExists)

if (NOT WIN32 AND NOT WIN64)
CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_LIBDL)
CHECK_LIBRARY_EXISTS(crypt crypt "" HAVE_LIBCRYPT)
endif ()

if(NOT TARGET lzma)
  add_subdirectory(${HPCC_SOURCE_DIR}/system/lzma ${CMAKE_BINARY_DIR}/system/lzma)
endif(NOT TARGET lzma)

find_package(lz4 CONFIG REQUIRED)

if(NOT TARGET libbase58)
  add_subdirectory(${HPCC_SOURCE_DIR}/system/libbase58 ${CMAKE_BINARY_DIR}/system/libbase58)
endif(NOT TARGET libbase58)

find_package(yaml CONFIG REQUIRED)

find_package(Protobuf REQUIRED)
find_package(CURL CONFIG REQUIRED)
if (USE_OPENTEL_GRPC)
    find_package(gRPC CONFIG REQUIRED)
endif ()
find_package(nlohmann_json CONFIG REQUIRED)
find_package(opentelemetry-cpp CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STRICT_CXX_FLAGS}")

set (    SRCS
    ${SCM_GENERATED_INCLUDES}
         jargv.cpp
         jarray.cpp
         javahash.cpp
         jbsocket.cpp
         jbuff.cpp
         jcomp.cpp
         jconfig.cpp
         jcontainerized.cpp
         jcrc.cpp
         jdebug.cpp
         jencrypt.cpp
         jevent.cpp
         jexcept.cpp
         jfile.cpp
         jflz.cpp
         jhash.cpp
         jiface.cpp
         jio.cpp
         jiter.cpp
         jkeyboard.cpp
         jlib.cpp
         jlog.cpp
         jlz4.cpp
         jlzbase.cpp
         jlzma.cpp
         jlzw.cpp
         jmd5.cpp
         jmemleak.cpp
         jmetrics.cpp
         jmisc.cpp
         jmutex.cpp
         jobserve.cpp
         jprop.cpp
         jptree.cpp
         jqueue.cpp
         jregexp.cpp
         jrowstream.cpp
         jsecrets.cpp
         jsem.cpp
         jset.cpp
         jsmartsock.cpp
         jsocket.cpp
         jsort.cpp
         jsort2.cpp
         jstats.cpp
         jstream.cpp
         jstring.cpp
         jsuperhash.cpp
         jtask.cpp
         jthread.cpp
         jtime.cpp
         junicode.cpp
         jutil.cpp
         jtrace.cpp
         jzstd.cpp
         ${HPCC_SOURCE_DIR}/system/globalid/lnuid.cpp
         ${HPCC_SOURCE_DIR}/system/codesigner/codesigner.cpp
         ${HPCC_SOURCE_DIR}/system/codesigner/gpgcodesigner.cpp
         sourcedoc.xml
    )

set (    INCLUDES
        ${HPCC_SOURCE_DIR}/system/codesigner/codesigner.hpp
        jaio.hpp
        jarray.hpp
        jatomic.hpp
        javahash.hpp
        javahash.tpp
        jbuff.hpp
        jcomp.hpp
        jcomp.ipp
        jconfig.hpp
        jcontainerized.hpp
        jcrc.hpp
        jdebug.hpp
        jelogtype.hpp
        jencrypt.hpp
        jerror.hpp
        jevent.hpp
        jexcept.hpp
        jfcmp.hpp
        jfile.hpp
        jfile.ipp
        jflz.hpp
        jhash.hpp
        jhash.ipp
        jheap.hpp
        jheap.ipp
        jiface.hpp
        jio.hpp
        jio.ipp
        jisem.hpp
        jiter.hpp
        jiter.ipp
        jiter.tpp
        jkeyboard.hpp
        jlib.hpp
        jliball.hpp
        jlog.hpp
        jlog.ipp
        jlz4.hpp
        jlzbase.hpp
        jlzma.hpp
        jlzw.hpp
        jlzw.ipp
        jmd5.hpp
        jmisc.hpp
        jmutex.hpp
        jobserve.hpp
        jobserve.ipp
        jpqueue.hpp
        jprop.hpp
        jptree.hpp
        jptree.ipp
        jqueue.hpp
        jqueue.tpp
        jregexp.hpp
        jrespool.tpp
        jrowstream.hpp
        jscm.hpp
        jsecrets.hpp
        jsem.hpp
        jset.hpp
        jsmartsock.hpp
        jsmartsock.ipp
        jsocket.hpp
        jsort.hpp
        jsorta.hpp
        jstatcodes.h
        jstats.h
        jstream.hpp
        jstream.ipp
        jstring.hpp
        jsuperhash.hpp
        jtask.hpp
        jthread.hpp
        jtime.hpp
        jtime.ipp
        junicode.hpp
        jutil.hpp
        jtrace.hpp
        jzstd.hpp
        ${HPCC_SOURCE_DIR}/system/httplib/httplib.h
        ${HPCC_SOURCE_DIR}/system/security/shared/opensslcommon.hpp
        ${HPCC_SOURCE_DIR}/system/security/cryptohelper/cryptocommon.cpp
        ${HPCC_SOURCE_DIR}/system/security/cryptohelper/digisign.cpp
        ${HPCC_SOURCE_DIR}/system/security/cryptohelper/pke.cpp
        ${HPCC_SOURCE_DIR}/system/security/cryptohelper/ske.cpp
        )

set_source_files_properties(jmd5.cpp jsort.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    set_property (SOURCE jencrypt.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -fno-strict-aliasing")
endif ()
if (CMAKE_COMPILER_IS_GNUCC)
    set_property (SOURCE jlzw.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-array-bounds")   # Compiler erroneously reports array bounds errors in some cases
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0)
        set_property (SOURCE jencrypt.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-array-bounds")   # Compiler erroneously reports array bounds errors in some cases
    endif ()
endif ()

include_directories (
         ${HPCC_SOURCE_DIR}/system/jlib
         ${HPCC_SOURCE_DIR}/system/win32
         ${HPCC_SOURCE_DIR}/system/include
         ${HPCC_SOURCE_DIR}/system/lzma
         ${HPCC_SOURCE_DIR}/system/globalid
         ${HPCC_SOURCE_DIR}/system/security/shared
         ${HPCC_SOURCE_DIR}/system/security/cryptohelper
         ${HPCC_SOURCE_DIR}/system/httplib
         ${OPENTELEMETRY_CPP_INCLUDE_DIRS}
         ${CMAKE_CURRENT_BINARY_DIR}  # for generated jelog.h file
         ${CMAKE_BINARY_DIR}
         ${CMAKE_BINARY_DIR}/oss
    )

# ensure httplib uses poll rather than select - otherwise it fail if too many sockets have been opened.
ADD_DEFINITIONS( -DCPPHTTPLIB_USE_POLL )

ADD_DEFINITIONS( -D_USRDLL -DJLIB_EXPORTS )
HPCC_ADD_LIBRARY( jlib SHARED ${SRCS} ${INCLUDES} )

target_link_libraries ( jlib
        lzma
        lz4::lz4
        libbase58
        yaml
       )

if ( ${HAVE_LIBDL} )
target_link_libraries ( jlib dl)
endif ( ${HAVE_LIBDL} )

if ( ${HAVE_LIBCRYPT} )
target_link_libraries ( jlib crypt)
endif ( ${HAVE_LIBCRYPT} )

target_link_libraries ( jlib
        
        opentelemetry-cpp::ostream_span_exporter
        opentelemetry-cpp::api #                               - Imported target of opentelemetry-cpp::api
        opentelemetry-cpp::sdk #                              - Imported target of opentelemetry-cpp::sdk
        opentelemetry-cpp::ext #                              - Imported target of opentelemetry-cpp::ext
        opentelemetry-cpp::logs #                             - Imported target of opentelemetry-cpp::logs
        opentelemetry-cpp::in_memory_span_exporter #          - Imported target of opentelemetry-cpp::in_memory_span_exporter
        opentelemetry-cpp::ostream_log_record_exporter #      - Imported target of opentelemetry-cpp::ostream_log_record_exporter
        ###These Otel import targets are not trivial to find, developers can save time by uncommenting as needed
        #   opentelemetry-cpp::version                           - Imported target of opentelemetry-cpp::version
        #   opentelemetry-cpp::common                            - Imported target of opentelemetry-cpp::common
        #   opentelemetry-cpp::trace                             - Imported target of opentelemetry-cpp::trace
        #   opentelemetry-cpp::metrics                           - Imported target of opentelemetry-cpp::metrics
        #   opentelemetry-cpp::otlp_recordable                   - Imported target of opentelemetry-cpp::otlp_recordable
        #   opentelemetry-cpp::otlp_grpc_log_record_exporter     - Imported target of opentelemetry-cpp::otlp_grpc_log_record_exporter
        #   opentelemetry-cpp::otlp_grpc_metrics_exporter        - Imported target of opentelemetry-cpp::otlp_grpc_metrics_exporter
        #   opentelemetry-cpp::otlp_http_client                  - Imported target of opentelemetry-cpp::otlp_http_client
        opentelemetry-cpp::otlp_http_exporter #                  - Imported target of opentelemetry-cpp::otlp_http_exporter
        #   opentelemetry-cpp::otlp_http_log_record_exporter     - Imported target of opentelemetry-cpp::otlp_http_log_record_exporter
        #   opentelemetry-cpp::otlp_http_metric_exporter         - Imported target of opentelemetry-cpp::otlp_http_metric_exporter
        #   opentelemetry-cpp::ostream_metrics_exporter          - Imported target of opentelemetry-cpp::ostream_metrics_exporter
        #   opentelemetry-cpp::ostream_span_exporter             - Imported target of opentelemetry-cpp::ostream_span_exporter
        #   opentelemetry-cpp::elasticsearch_log_record_exporter - Imported target of opentelemetry-cpp::elasticsearch_log_record_exporter
        #   opentelemetry-cpp::etw_exporter                      - Imported target of opentelemetry-cpp::etw_exporter
        #   opentelemetry-cpp::zpages                            - Imported target of opentelemetry-cpp::zpages
        #   opentelemetry-cpp::http_client_curl                  - Imported target of opentelemetry-cpp::http_client_curl
        #   opentelemetry-cpp::opentracing_shim                  - Imported target of opentelemetry-cpp::opentracing_shim
        )

IF (USE_OPENSSL)
target_link_libraries ( jlib ${OPENSSL_LIBRARIES})
endif (USE_OPENSSL)

IF (WIN32 AND USE_ZLIB)
target_link_libraries ( jlib ZLIB::ZLIB )
endif (WIN32 AND USE_ZLIB)

if (USE_OPENTEL_GRPC)
    target_link_libraries ( jlib 
        opentelemetry-cpp::otlp_grpc_client #                 - Imported target of opentelemetry-cpp::otlp_grpc_client
        opentelemetry-cpp::otlp_grpc_exporter #               - Imported target of opentelemetry-cpp::otlp_grpc_exporter
    )
endif ()
if (WIN32)
 target_link_libraries ( jlib ws2_32 mpr.lib Winmm.lib psapi.lib )
elseif (("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin" ))
else ()
 target_link_libraries ( jlib rt )
endif ()

target_link_libraries(jlib zstd::libzstd)

if (NOT PLUGIN)
    if (WIN32)
        FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}" WINDOWS_CMAKE_CURRENT_BINARY_DIR)
        FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}\\jelog.mc" WINDOWS_CMAKE_CURRENT_SOURCE_DIR)
        add_custom_command ( DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/jelog.mc
                         OUTPUT ${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>/jelog.dll ${CMAKE_CURRENT_BINARY_DIR}/jelog.h
                         COMMAND echo mc -r ${WINDOWS_CMAKE_CURRENT_BINARY_DIR} -h ${WINDOWS_CMAKE_CURRENT_BINARY_DIR} ${WINDOWS_CMAKE_CURRENT_SOURCE_DIR}
                         COMMAND echo rc -r -fo ${CMAKE_CURRENT_BINARY_DIR}/jelog.res ${CMAKE_CURRENT_BINARY_DIR}/jelog.rc
                         COMMAND echo link -dll -noentry -machine:IX86 -out:${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>/jelog.dll ${CMAKE_CURRENT_BINARY_DIR}/jelog.res
                         COMMAND mc -r ${WINDOWS_CMAKE_CURRENT_BINARY_DIR} -h ${WINDOWS_CMAKE_CURRENT_BINARY_DIR} ${WINDOWS_CMAKE_CURRENT_SOURCE_DIR}
                         COMMAND rc -r -fo ${CMAKE_CURRENT_BINARY_DIR}/jelog.res ${CMAKE_CURRENT_BINARY_DIR}/jelog.rc
                         COMMAND link -dll -noentry -machine:IX86 -out:${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>/jelog.dll ${CMAKE_CURRENT_BINARY_DIR}/jelog.res
                        )
        add_custom_target ( jelog ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>/jelog.dll )
        add_dependencies( jlib jelog )
        install ( PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/$<CONFIG>/jelog.dll DESTINATION ${EXEC_DIR} )
    endif ()
    install ( TARGETS jlib RUNTIME DESTINATION ${EXEC_DIR} LIBRARY DESTINATION ${LIB_DIR} CALC_DEPS )
    install ( FILES ${CMAKE_CURRENT_SOURCE_DIR}/jscm.hpp DESTINATION componentfiles/cl/include COMPONENT Runtime )
endif()
