//
//  main.cpp
//  Environment
//
//  Created by Yan Roo on 2025/2/28.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include "CL/opencl.hpp"

using namespace std;

string loadKernelSource(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open kernel file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    cout << "Hello" << endl;

    // Platform
    vector<cl::Platform> all_platform;
    cl::Platform::get(&all_platform);
    if (all_platform.size() == 0) {
        cout << "No platforms found. Check OpenCL installation!" << endl;
        return 1;
    }
    cl::Device default_device;
    for (int i = 0; i < all_platform.size(); i++) {
        cl::Platform default_platform = all_platform[i];
        cout << "Using platform: "
             << default_platform.getInfo<CL_PLATFORM_NAME>() << endl;

        // Device
        vector<cl::Device> all_device;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_device);
        if (all_device.size() == 0) {
            cout << "No device found. Check OpenCL installation!" << endl;
        }
        for (int j = 0; j < all_device.size(); j++) {
            default_device = all_device[j];
            cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>()
                 << endl;
        }
    }
    cl::Context context({default_device});
    cl::Program::Sources sources;
    // kernel calculates for each element C=A+B
    std::string kernel_code = loadKernelSource("Environment/simple_add.cl");
    sources.push_back({kernel_code.c_str(), kernel_code.length()});

    cl::Program program(context, sources);
    if (program.build({default_device}) != CL_SUCCESS) {
        cout << "Error building"
             << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)
             << endl;
        return 1;
    }

    // create buffers on the device
    cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * 10);

    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

    // create queue to which we will push commands for the device.
    cl::CommandQueue queue(context, default_device);

    queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(A), A);
    queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(B), B);

    // run the kernel
    cl::Kernel kernel_add = cl::Kernel(program, "simple_add");
    kernel_add.setArg(0, buffer_A);
    kernel_add.setArg(1, buffer_B);
    kernel_add.setArg(2, buffer_C);
    queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(10),
                               cl::NullRange);
    queue.finish();

    int C[10];
    // read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(C), C);

    std::cout << " result: \n";
    for (int i = 0; i < 10; i++) {
        std::cout << C[i] << " ";
    }
    return 0;
}
