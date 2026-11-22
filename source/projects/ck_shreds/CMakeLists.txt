# ck_shreds: table of chuck~'s shreds (id, name, elapsed, remove), fed by the
# reply outlet. Uses only the Max SDK.

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)
# max-pretarget forces 10.11; restore the project minimum
set(CMAKE_OSX_DEPLOYMENT_TARGET "${CM_OSX_DEPLOYMENT_TARGET}" CACHE STRING "Minimum OS X deployment version" FORCE)

if (MSVC)
    # NOMINMAX: <windows.h>, pulled in by the Max SDK, otherwise defines
    # min/max macros that break std::min/std::max.
    set(CK_SHREDS_WARN /W3 /utf-8)
    set(CK_SHREDS_DEFS NOMINMAX)
else ()
    set(CK_SHREDS_WARN -Wall -Wextra -Wno-unused-parameter)
    set(CK_SHREDS_DEFS)
endif ()

# Headless model, shared by the external and the tests.
add_library(ck_shreds_core STATIC core/table.cpp)
target_include_directories(ck_shreds_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(ck_shreds_core PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON POSITION_INDEPENDENT_CODE ON)
target_compile_options(ck_shreds_core PRIVATE ${CK_SHREDS_WARN})
target_compile_definitions(ck_shreds_core PUBLIC ${CK_SHREDS_DEFS})

add_library(${PROJECT_NAME} MODULE ck_shreds.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${MAX_SDK_INCLUDES}" "${MAX_SDK_MSP_INCLUDES}")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE ${CK_SHREDS_WARN})
    target_link_libraries(${PROJECT_NAME} PRIVATE ck_shreds_core)
else ()
    # The `(method)` casts of class_addmethod trip -Wcast-function-type-mismatch.
    target_compile_options(${PROJECT_NAME} PRIVATE ${CK_SHREDS_WARN} -Wno-cast-function-type-mismatch)
    target_link_libraries(${PROJECT_NAME} PRIVATE ck_shreds_core "-Wl,-exported_symbol,_ext_main")
endif ()
include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)

# The harness (test.h, main.cpp) is ck_code's.
set(CK_CODE_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ck_code/tests)
add_executable(ck_shreds_tests ${CK_CODE_TESTS_DIR}/main.cpp tests/table_test.cpp)
target_include_directories(ck_shreds_tests PRIVATE ${CK_CODE_TESTS_DIR})
target_link_libraries(ck_shreds_tests PRIVATE ck_shreds_core)
target_compile_options(ck_shreds_tests PRIVATE ${CK_SHREDS_WARN})
set_target_properties(ck_shreds_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
