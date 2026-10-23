# engine-level tests that link chuck_lib directly; run by 'make test'.
# macOS only: the Windows link set is untested
if(NOT APPLE)
    return()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CHUCK_TILDE_DIR ${CMAKE_SOURCE_DIR}/source/projects/chuck_tilde)

# the maxclock, maxin and maxbuffer sources have no Max dependency, so the
# test builds them
add_executable(bind_test bind_test.cpp
    ${CHUCK_TILDE_DIR}/maxclock.cpp
    ${CHUCK_TILDE_DIR}/maxin.cpp
    ${CHUCK_TILDE_DIR}/maxbuffer.cpp
)

target_compile_definitions(bind_test PRIVATE
    BIND_EXAMPLES_DIR="${CMAKE_SOURCE_DIR}/examples/max"
)

target_include_directories(bind_test PRIVATE
    ${CMAKE_SOURCE_DIR}/source/projects/chuck/core
    ${CHUCK_TILDE_DIR}
)

# same frameworks as the chuck host (source/projects/chuck/host)
target_link_libraries(bind_test
    PRIVATE
    chuck_lib
    "-framework CoreAudio"
    "-framework CoreMIDI"
    "-framework CoreFoundation"
    "-framework IOKit"
    "-framework Carbon"
    "-framework AppKit"
    "-framework Foundation"
    "-F/System/Library/PrivateFrameworks"
    "-weak_framework MultitouchSupport"
    -lm
)
