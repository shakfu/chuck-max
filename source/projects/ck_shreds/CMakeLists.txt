# ck_shreds: table of chuck~'s shreds (id, name, elapsed, remove), fed by the
# reply outlet. Links neither ChucK nor MSP beyond the SDK.
# macOS only for now: untested on the Windows build.
if (NOT APPLE)
    return()
endif ()

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)
# max-pretarget forces 10.11; restore the project minimum
set(CMAKE_OSX_DEPLOYMENT_TARGET "${CM_OSX_DEPLOYMENT_TARGET}" CACHE STRING "Minimum OS X deployment version" FORCE)

# Headless model, shared by the external and the tests.
add_library(ck_shreds_core STATIC core/table.cpp)
target_include_directories(ck_shreds_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(ck_shreds_core PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON POSITION_INDEPENDENT_CODE ON)
target_compile_options(ck_shreds_core PRIVATE -Wall -Wextra)

add_library(${PROJECT_NAME} MODULE ck_shreds.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${MAX_SDK_INCLUDES}" "${MAX_SDK_MSP_INCLUDES}")
# The `(method)` casts of class_addmethod trip -Wcast-function-type-mismatch.
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wno-unused-parameter -Wno-cast-function-type-mismatch)
target_link_libraries(${PROJECT_NAME} PRIVATE ck_shreds_core "-Wl,-exported_symbol,_ext_main")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)

# The harness (test.h, main.cpp) is ck_code's.
set(CK_CODE_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ck_code/tests)
add_executable(ck_shreds_tests ${CK_CODE_TESTS_DIR}/main.cpp tests/table_test.cpp)
target_include_directories(ck_shreds_tests PRIVATE ${CK_CODE_TESTS_DIR})
target_link_libraries(ck_shreds_tests PRIVATE ck_shreds_core)
set_target_properties(ck_shreds_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
