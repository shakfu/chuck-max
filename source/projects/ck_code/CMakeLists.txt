# ck_code: in-patch ChucK editor for [chuck~], tree-sitter highlighting.
# Links neither ChucK nor MSP: it emits addcode / replacecode / remove messages.
# macOS only: platform/ uses AppKit for the clipboard and file panels.
if (NOT APPLE)
    return()
endif ()

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)
# max-pretarget forces 10.11; restore the project minimum
set(CMAKE_OSX_DEPLOYMENT_TARGET "${CM_OSX_DEPLOYMENT_TARGET}" CACHE STRING "Minimum OS X deployment version" FORCE)

set(TS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tree-sitter/lib)
set(TS_CK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tree-sitter-chuck/src)

# Vendored C sources, warnings off.
add_library(ck_code_treesitter STATIC
    ${TS_DIR}/src/lib.c
    ${TS_CK_DIR}/parser.c
)
target_include_directories(ck_code_treesitter
    PUBLIC ${TS_DIR}/include
    PRIVATE ${TS_DIR}/src ${TS_CK_DIR})
target_compile_options(ck_code_treesitter PRIVATE -w)
set_target_properties(ck_code_treesitter PROPERTIES C_STANDARD 11 POSITION_INDEPENDENT_CODE ON)

# Headless core, shared by the external and the tests.
file(GLOB CK_CODE_CORE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp)
add_library(ck_code_core STATIC ck_syntax.cpp ${CK_CODE_CORE_SOURCES})
target_include_directories(ck_code_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ck_code_core PUBLIC ck_code_treesitter)
set_target_properties(ck_code_core PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON POSITION_INDEPENDENT_CODE ON)
target_compile_options(ck_code_core PRIVATE -Wall -Wextra -Wno-unused-parameter)

file(GLOB CK_CODE_MAX_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ck_code*.cpp)
# ObjC++ compiled as CXX: the Xcode generator finds no OBJCXX compiler.
file(GLOB CK_CODE_PLATFORM_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.mm)
set_source_files_properties(${CK_CODE_PLATFORM_SOURCES} PROPERTIES
    LANGUAGE CXX COMPILE_OPTIONS "-xobjective-c++")
add_library(${PROJECT_NAME} MODULE
    ${CK_CODE_MAX_SOURCES}
    ${CK_CODE_PLATFORM_SOURCES}
    # UI boxes need the symbols jpatcher_syms_init() fills.
    "${MAX_SDK_INCLUDES}/common/jpatcher_syms.c")
target_include_directories(${PROJECT_NAME} PRIVATE "${MAX_SDK_INCLUDES}")
# The `(method)` casts of class_addmethod trip -Wcast-function-type-mismatch.
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wno-unused-parameter -Wno-cast-function-type-mismatch>)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ck_code_core
    "-framework AppKit"
    "-weak_framework UniformTypeIdentifiers"
    # Export only the entry point: vendored symbols stay private.
    "-Wl,-exported_symbol,_ext_main"
    # Resolved by Max at load; missing from this SDK's max-linker-flags.txt.
    "-Wl,-U,_jpopupmenu_addseparator")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)

file(GLOB CK_CODE_TESTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp)
add_executable(ck_code_tests tests/main.cpp ${CK_CODE_TESTS})
target_link_libraries(ck_code_tests PRIVATE ck_code_core)
set_target_properties(ck_code_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
