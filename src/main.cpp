#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>

#include <cassert>
#include <iostream>
#include <vector>


WGPUAdapter
request_adapter(
        WGPUInstance instance,
        WGPURequestAdapterOptions const *options) {

    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool request_ended = false;
    };

    UserData user_data;

    auto on_adapter_request_ended = [](
            WGPURequestAdapterStatus status,
            WGPUAdapter adapter,
            char const *message,
            void *p_user_data) {

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

WGPUDevice request_device(
        WGPUAdapter adapter,
        WGPUDeviceDescriptor const *descriptor) {

    struct UserData {
        WGPUDevice device = nullptr;
        bool request_ended = false;
    };

    UserData user_data;

    auto on_device_request_ended = [](
            WGPURequestDeviceStatus status,
            WGPUDevice device,
            char const *message,
            void *p_user_data) {

        UserData &user_data = *reinterpret_cast<UserData *>(p_user_data);
        if (status == WGPURequestDeviceStatus_Success) {
            user_data.device = device;
        } else {
            std::cout << "Could not get WebGPU device: "
                      << message << std::endl;
        }
        user_data.request_ended = true;

    };

    wgpuAdapterRequestDevice(
            adapter,
            descriptor,
            on_device_request_ended,
            (void *) &user_data
    );

    assert(user_data.request_ended);

    return user_data.device;

}


int main() {

    // Create a WebGPU instance.
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    // Create an instance using the descriptor.
    WGPUInstance instance = wgpuCreateInstance(&desc);

    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window = glfwCreateWindow(640, 400,
                                          "Learn WebGPU",
                                          nullptr, nullptr);

    if (!window) {
        std::cerr << "Could not open window!" << std::endl;
        return 1;
    }

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


    std::cout << "Requesting device ..." << std::endl;

    auto on_device_lost_callback = [](
            WGPUDevice const * /*p_device*/,
            WGPUDeviceLostReason reason,
            char const *message,
            void * /*p_user_data*/) {

        std::cout << "Device lost callback: reason " << reason;
        if (message) {
            std::cout << " (" << message << ")";
        }
        std::cout << std::endl;

    };

    WGPUDeviceLostCallbackInfo callback_info = {};
    callback_info.nextInChain = nullptr;
    callback_info.mode = WGPUCallbackMode_WaitAnyOnly;
    callback_info.callback = on_device_lost_callback;

    WGPUDeviceDescriptor device_descriptor = {};
    device_descriptor.nextInChain = nullptr;
    device_descriptor.label = "My Device";
    device_descriptor.requiredFeatureCount = 0;
    device_descriptor.requiredLimits = nullptr;
    device_descriptor.defaultQueue.nextInChain = nullptr;
    device_descriptor.defaultQueue.label = "The default queue";
    device_descriptor.deviceLostCallbackInfo = callback_info;

    WGPUDevice device = request_device(adapter, &device_descriptor);

    auto on_device_error = [](
            WGPUErrorType type,
            char const *message,
            void * /*p_user_data*/) {

        std::cout << "Un-captured device error: type " << type;
        if (message) {
            std::cout << " (" << message << ")";
        }
        std::cout << std::endl;

    };
    wgpuDeviceSetUncapturedErrorCallback(device, on_device_error, nullptr);

    std::cout << "Got device: " << device << std::endl;


    WGPUQueue queue = wgpuDeviceGetQueue(device);

    auto on_queue_work_done = [](
            WGPUQueueWorkDoneStatus status,
            void * /*p_user_data*/) {

        std::cout << "Queued work finished with status: "
                  << status << std::endl;

    };
    wgpuQueueOnSubmittedWorkDone(queue, on_queue_work_done, nullptr);



    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Release the queue.
    wgpuQueueRelease(queue);

    // Release the device
    wgpuDeviceRelease(device);

    // Release the adapter.
    wgpuAdapterRelease(adapter);

    // Release the surface
    wgpuSurfaceRelease(surface);

    // Release the instance.
    wgpuInstanceRelease(instance);

    // Delete the window.
    glfwDestroyWindow(window);

    // Terminate GLFW.
    glfwTerminate();

    return 0;

}
