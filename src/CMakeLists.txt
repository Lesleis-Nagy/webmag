add_executable(webmag main.cpp webgpu-utils.cpp)

set_target_warnings(webmag)

target_link_libraries(webmag
        PRIVATE glfw webgpu glfw3webgpu
)

target_include_directories(webmag
        PRIVATE ${WEBMAG_INCLUDE_DIR}
)

target_copy_webgpu_binaries(webmag)

set_target_properties(webmag
        PROPERTIES VS_DEBUGGER_ENVIRONMENT "DAWN_DEBUG_BREAK_ON_ERROR=1")
