#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

// OpenCV headers
#include <opencv2/opencv.hpp>

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
    string imagePath = "BlurFilter/cameraman.tif";

    // Load Image (Grayscale)
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        cerr << "Failed to load image: " << imagePath << endl;
        return 1;
    }

    int filter_size = 3;
    int width = image.cols;
    int height = image.rows;
    int imageSize = width * height;

    // Transform OpenCV Mat to vector<uchar>
    // In OpenCL, buffers expect contiguous memory, like a 1D array, for
    // efficient data transfer.
    vector<unsigned char> inputImage(image.data, image.data + imageSize);
    vector<unsigned char> outputImage(imageSize, 0);

    try {
        vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) {
            cerr << "No OpenCL platforms found." << endl;
            return 1;
        }

        cl::Device defaultDevice;
        for (const auto &platform : platforms) {
            vector<cl::Device> devices;
            platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
            if (!devices.empty()) {
                defaultDevice = devices[0];
                cout << "Using platform: "
                     << platform.getInfo<CL_PLATFORM_NAME>() << endl;
                cout << "Using device: "
                     << defaultDevice.getInfo<CL_DEVICE_NAME>() << endl;
                break;
            }
        }

        cl::Context context({defaultDevice});
        cl::CommandQueue queue(context, defaultDevice);

        cl::Buffer bufferIn(context, CL_MEM_READ_ONLY,
                            sizeof(unsigned char) * imageSize);
        cl::Buffer bufferOut(context, CL_MEM_WRITE_ONLY,
                             sizeof(unsigned char) * imageSize);

        // CL_TRUE: blocking
        queue.enqueueWriteBuffer(bufferIn, CL_TRUE, 0,
                                 sizeof(unsigned char) * imageSize,
                                 inputImage.data());

        // load kernel code
        string kernelCode = loadKernelSource("BlurFilter/blur_filter.cl");
        cl::Program::Sources sources;
        sources.push_back({kernelCode.c_str(), kernelCode.length()});

        // build and compile kernel code
        cl::Program program(context, sources);
        if (program.build({defaultDevice}) != CL_SUCCESS) {
            cerr << "Error building: "
                 << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDevice)
                 << endl;
            return 1;
        }

        cl::Kernel kernelBlur(program, "blur_filter");
        kernelBlur.setArg(0, bufferIn);
        kernelBlur.setArg(1, bufferOut);
        kernelBlur.setArg(2, filter_size);
        kernelBlur.setArg(3, width);
        kernelBlur.setArg(4, height);

        // Define global NDRange to handle every pixel
        cl::NDRange global(width, height);
        cl::NDRange local =
            cl::NullRange; // OpenCL auto decide the workgroup size

        auto startOpenCL = chrono::high_resolution_clock::now();
        queue.enqueueNDRangeKernel(kernelBlur, cl::NullRange, global, local);
        queue.finish();
        auto endOpenCL = chrono::high_resolution_clock::now();
        auto durationOpenCL = chrono::duration_cast<chrono::milliseconds>(
            endOpenCL - startOpenCL);
        cout << "OpenCL kernel execution took " << durationOpenCL.count()
             << " milliseconds." << endl;

        // Load the image data from the device
        queue.enqueueReadBuffer(bufferOut, CL_TRUE, 0,
                                sizeof(unsigned char) * imageSize,
                                outputImage.data());

        // Restore the output back to OpenCV Mat
        cv::Mat result(height, width, CV_8UC1, outputImage.data());

        cv::imshow("Original Image", image);
        cv::imshow("Blurred Image", result);
        cv::waitKey(0);
    } catch (const exception &e) {
        cerr << "Runtime error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
