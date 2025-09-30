# AudioUnit Chugin CMakeLists.txt

add_chugin(
    LINK_LIBS
        "$<$<PLATFORM_ID:Darwin>:-framework CoreFoundation>"
        "$<$<PLATFORM_ID:Darwin>:-framework AudioToolbox>"        
)
