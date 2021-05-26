cmake_minimum_required(VERSION 3.19)
project(universal_converter)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (WIN32)
    add_compile_definitions(NOMINMAX)
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if (WIN32)
    set(POCO_EXTRA_OPTIONS poco:enable_netssl_win=False)
else()
    set(POCO_EXTRA_OPTIONS poco:enable_netssl=False)
endif()

conan_cmake_run(
    REQUIRES
        boost/1.75.0
        gmp/6.2.1
        catch2/2.13.4
        poco/1.10.1
        nlohmann_json/3.9.1
        fast-cpp-csv-parser/cci.20200830
    OPTIONS
        ${POCO_EXTRA_OPTIONS}
        poco:enable_net=True
        poco:enable_util=True
        poco:enable_xml=True # enable_util dependency
        poco:enable_json=True # enable_util dependency
        poco:enable_apacheconnector=False
        poco:enable_cppparser=False
        poco:enable_crypto=False
        poco:enable_data=False
        poco:enable_data_mysql=False
        poco:enable_data_odbc=False
        poco:enable_data_postgresql=False
        poco:enable_data_sqlite=False
        poco:enable_encodings=False
        poco:enable_jwt=False
        poco:enable_mongodb=False
        poco:enable_pdf=False
        poco:enable_pagecompiler=False
        poco:enable_pagecompiler_file2page=False
        poco:enable_pocodoc=False
        poco:enable_redis=False
        poco:enable_sevenzip=False
        poco:enable_zip=False
    BUILD missing
    BASIC_SETUP TARGETS CMAKE_TARGETS
)
add_subdirectory("${CMAKE_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_SOURCE_DIR}/server")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests")
