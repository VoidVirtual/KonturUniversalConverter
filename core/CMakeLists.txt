cmake_minimum_required(VERSION 3.19)
project(core)

include("Utilities")

add_library(${PROJECT_NAME} STATIC)
setup_udecoder_target(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CONAN_PKG::boost
        CONAN_PKG::fast-cpp-csv-parser
        CONAN_PKG::gmp
        CONAN_PKG::nlohmann_json
        CONAN_PKG::poco
)
