macro(set_target_warnings target)

    # In addition to COMPILE_WARNING_AS_ERROR, which is turned on as a good
    # practice, make sure no warning is left ignored.
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
    endif()

    # On MacOS, CMake can generate XCode project files. However, by default, no
    # schemes are created, and XCode itself generates a scheme for each CMake
    # target – usually we only want a scheme for our main target. Therefore, we
    # set the XCODE_GENERATE_SCHEME property. We will also already enable frame
    # capture for GPU debugging.
    if(XCODE)
        set_target_properties(${target} PROPERTIES
                XCODE_GENERATE_SCHEME ON
                XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
    endif()

endmacro()
