#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>

#include <cassert>
#include <iostream>
#include <vector>


WGPUAdapter
request_adapter(
        WGPUInstance instance,
        WGPURequestAdapterOptions const *options
) {

    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool request_ended = false;
    };

    UserData user_data;

    auto on_adapter_request_ended = [](
            WGPURequestAdapterStatus status,
            WGPUAdapter adapter,
            char const *message,
            void *p_user_data
    ) {

        UserData &user_data = *reinterpret_cast<UserData *>(p_user_data);

        if (status == WGPURequestAdapterStatus_Success) {
            user_data.adapter = adapter;
        } else {
            std::cout << "Could not get WEBGPU adapter: "
                      << message
                      << std::endl;
        }

        user_data.request_ended = true;

    };

    // Call to the WebGPU request adapter procedure.
    wgpuInstanceRequestAdapter(
            instance,
            options,
            on_adapter_request_ended,
            (void *) &user_data
    );

    assert(user_data.request_ended);

    return user_data.adapter;

}


int main() {

    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window = glfwCreateWindow(2000, 1000,
                                          "Learn WebGPU",
                                          nullptr, nullptr);

    if (!window) {
        std::cerr << "Could not open window!" << std::endl;
        return 1;
    }

    // Create a webgpu instance.
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    // Create an instance using the descriptor.
    WGPUInstance instance = wgpuCreateInstance(&desc);

    // Check that the instance was created.
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }

    WGPUSurface surface = glfwGetWGPUSurface(instance, window);

    std::cout << "Request adapter ..." << std::endl;
    WGPURequestAdapterOptions adapter_options = {};
    adapter_options.nextInChain = nullptr;
    adapter_options.compatibleSurface = surface;
    WGPUAdapter adapter = request_adapter(instance, &adapter_options);
    std::cout << "Got adapter: " << adapter << std::endl;

    std::vector<WGPUFeatureName> features;

    size_t feature_count = wgpuAdapterEnumerateFeatures(adapter, nullptr);

    features.resize(feature_count);

    wgpuAdapterEnumerateFeatures(adapter, features.data());

    std::cout << "Adapter features:" << std::endl;
    for (auto f : features) {
        std::cout << " - " << f << std::endl;
    }

    // Main loop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Release the adapter.
    wgpuAdapterRelease(adapter);

    // Release the surface
    wgpuSurfaceRelease(surface);

    // Release the instance.
    wgpuInstanceRelease(instance);

    // Terminate GLFW.
    glfwTerminate();

    return 0;

}
