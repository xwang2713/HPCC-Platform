if(USE_AWS)
    # minimal CMakeLists.txt for the AWS SDK for C++
    cmake_minimum_required(VERSION 2.8)

    project(aws)
    set(AWS_SDK_CPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/aws-sdk-cpp)
    if (CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)
      set(EXTERNAL_SQS_CXX_FLAGS "-Wno-error=implicit-fallthrough -Wno-error=int-in-bool-context -Wno-error=format-nonliteral" )
    endif()

    include(ExternalProject)
    ExternalProject_Add(
        aws-sdk-cpp
        SOURCE_DIR ${AWS_SDK_CPP_SOURCE_DIR}
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp
        CMAKE_ARGS -DCUSTOM_MEMORY_MANAGEMENT:string=0 -DCMAKE_CXX_FLAGS=${EXTERNAL_SQS_CXX_FLAGS} -DCMAKE_SHARED_LINKER_FLAGS_INIT=-Wl,-rpath-link,${LIB_PATH}
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} aws-cpp-sdk-sqs aws-cpp-sdk-s3
        BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core/libaws-cpp-sdk-core${CMAKE_SHARED_LIBRARY_SUFFIX}
                         ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-sqs/libaws-cpp-sdk-sqs${CMAKE_SHARED_LIBRARY_SUFFIX}
                         ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-s3/libaws-cpp-sdk-s3${CMAKE_SHARED_LIBRARY_SUFFIX}
        INSTALL_COMMAND "")

    include(GNUInstallDirs)
    set(AWS_DEP_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/.deps/install/${CMAKE_INSTALL_LIBDIR})
    add_library(aws-cpp-sdk-core SHARED IMPORTED GLOBAL)
    add_library(aws-cpp-sdk-sqs SHARED IMPORTED GLOBAL)
    add_library(aws-cpp-sdk-s3 SHARED IMPORTED GLOBAL)

    set_property(TARGET aws-cpp-sdk-core
        PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core/libaws-cpp-sdk-core${CMAKE_SHARED_LIBRARY_SUFFIX})
    set_property(TARGET aws-cpp-sdk-sqs
        PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-sqs/libaws-cpp-sdk-sqs${CMAKE_SHARED_LIBRARY_SUFFIX})
    set_property(TARGET aws-cpp-sdk-s3
        PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-s3/libaws-cpp-sdk-s3${CMAKE_SHARED_LIBRARY_SUFFIX})
    add_dependencies(aws-cpp-sdk-core aws-sdk-cpp)
    add_dependencies(aws-cpp-sdk-sqs aws-sdk-cpp)
    add_dependencies(aws-cpp-sdk-s3 aws-sdk-cpp)

    if ( NOT PLUGIN )
        install(CODE "set(ENV{LD_LIBRARY_PATH} \"\$ENV{LD_LIBRARY_PATH}:${AWS_DEP_LIBDIR}:${PROJECT_BINARY_DIR}:${PROJECT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core:${PROJECT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-sqs:${PROJECT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-s3\")")
        install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/aws-sdk-cpp/LICENSE.txt
            DESTINATION lib
            RENAME aws-sdk-cpp-LICENSE.txt)
        install(CODE "file(RPATH_CHANGE
            FILE \"${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core/libaws-cpp-sdk-core${CMAKE_SHARED_LIBRARY_SUFFIX}\"
            OLD_RPATH \"${AWS_DEP_LIBDIR}:\"
            NEW_RPATH \"${LIB_PATH}\")")
        install(CODE "file(RPATH_CHANGE
            FILE \"${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-sqs/libaws-cpp-sdk-sqs${CMAKE_SHARED_LIBRARY_SUFFIX}\"
            OLD_RPATH \"${PROJECT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core:${AWS_DEP_LIBDIR}:\"
            NEW_RPATH \"${LIB_PATH}\")")
        install(CODE "file(RPATH_CHANGE
            FILE \"${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-s3/libaws-cpp-sdk-s3${CMAKE_SHARED_LIBRARY_SUFFIX}\"
            OLD_RPATH \"${PROJECT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core:${AWS_DEP_LIBDIR}:\"
            NEW_RPATH \"${LIB_PATH}\")")
        install(PROGRAMS
            ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-core/libaws-cpp-sdk-core${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-sqs/libaws-cpp-sdk-sqs${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/aws-cpp-sdk-s3/libaws-cpp-sdk-s3${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${AWS_DEP_LIBDIR}/libaws-c-common${CMAKE_SHARED_LIBRARY_SUFFIX}.1.0.0
            ${AWS_DEP_LIBDIR}/libaws-c-common${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${AWS_DEP_LIBDIR}/libaws-c-common${CMAKE_SHARED_LIBRARY_SUFFIX}.0unstable
            ${AWS_DEP_LIBDIR}/libaws-c-event-stream${CMAKE_SHARED_LIBRARY_SUFFIX}.1.0.0
            ${AWS_DEP_LIBDIR}/libaws-c-event-stream${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${AWS_DEP_LIBDIR}/libaws-c-event-stream${CMAKE_SHARED_LIBRARY_SUFFIX}.0unstable
            ${AWS_DEP_LIBDIR}/libaws-checksums${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION lib)
    endif ( NOT PLUGIN )
endif()
