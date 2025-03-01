//
//  main.cpp
//  VecAddCompare
//
//  Created by Yan Roo on 2025/3/1.
//
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <CL/opencl.hpp>

using namespace std;

string loadKernelSource(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open kernel file: " + filename);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    // Set test data size (adjust according to your needs)
    const int array_size = 1000000; // e.g. one million

    vector<int> A(array_size), B(array_size), C_cpu(array_size),
        C_opencl(array_size);
    for (int i = 0; i < array_size; i++) {
        A[i] = (random() % 19) - 10;
        B[i] = (random() % 19) - 10;
    }

    // --- Direct C++ addition ---
    auto start_cpu = chrono::high_resolution_clock::now();
    for (int i = 0; i < array_size; i++) {
        C_cpu[i] = A[i] + B[i];
    }
    auto end_cpu = chrono::high_resolution_clock::now();
    auto duration_cpu =
        chrono::duration_cast<chrono::microseconds>(end_cpu - start_cpu);
    cout << "C++ addition " << duration_cpu.count() << " microseconds"
         << endl;

    // --- OpenCL addition ---
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        cerr << "No OpenCL platforms found." << endl;
        return 1;
    }

    cl::Device default_device;
    for (const auto &platform : platforms) {
        vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        if (!devices.empty()) {
            default_device = devices[0];
            cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>()
                 << endl;
            cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>()
                 << endl;
            break;
        }
    }

    cl::Context context({default_device});
    cl::CommandQueue queue(context, default_device);

    cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * array_size);
    cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * array_size);
    cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * array_size);

    queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * array_size,
                             A.data());
    queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * array_size,
                             B.data());

    string kernel_code = loadKernelSource("Environment/simple_add.cl");

    cl::Program::Sources sources;
    sources.push_back({kernel_code.c_str(), kernel_code.length()});
    cl::Program program(context, sources);
    if (program.build({default_device}) != CL_SUCCESS) {
        cerr << "Error building: "
             << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)
             << endl;
        return 1;
    }

    cl::Kernel kernel_add(program, "simple_add");
    kernel_add.setArg(0, buffer_A);
    kernel_add.setArg(1, buffer_B);
    kernel_add.setArg(2, buffer_C);

    auto start_opencl = chrono::high_resolution_clock::now();
    queue.enqueueNDRangeKernel(kernel_add, cl::NullRange,
                               cl::NDRange(array_size), cl::NullRange);
    queue.finish(); // Wait for all commands to complete
    auto end_opencl = chrono::high_resolution_clock::now();
    auto duration_opencl =
        chrono::duration_cast<chrono::microseconds>(end_opencl - start_opencl);
    cout << "OpenCL addition " << duration_opencl.count()
         << " microseconds" << endl;

    queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * array_size,
                            C_opencl.data());

    // Verify if the results match
    bool correct = true;
    for (int i = 0; i < array_size; i++) {
        if (C_cpu[i] != C_opencl[i]) {
            correct = false;
            break;
        }
    }
    cout << "Results are " << (correct ? "correct" : "incorrect") << endl;

    return 0;
}
