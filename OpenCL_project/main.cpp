#include <OpenCL/opencl.h>
#include <stdio.h>
#include <stdlib.h>

void printPlatformInfo(cl_platform_id platform) {
    char platform_name[1024];
    char platform_vendor[1024];
    char platform_version[1024];

    clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
    clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, NULL);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(platform_version), platform_version, NULL);

    printf("Platform Name: %s\n", platform_name);
    printf("Platform Vendor: %s\n", platform_vendor);
    printf("Platform Version: %s\n", platform_version);
}

void printDeviceInfo(cl_device_id device) {
    char device_name[1024];
    char device_vendor[1024];
    cl_device_type device_type;
    char device_version[1024];

    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(device_vendor), device_vendor, NULL);
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(device_version), device_version, NULL);

    printf("\tDevice Name: %s\n", device_name);
    printf("\tDevice Vendor: %s\n", device_vendor);
    printf("\tDevice Type: %llu\n", device_type);
    printf("\tDevice Version: %s\n", device_version);

    // 打印设备类型
    printf("\tDevice Type: ");
    if (device_type & CL_DEVICE_TYPE_CPU) {
        printf("CPU ");
    }
    if (device_type & CL_DEVICE_TYPE_GPU) {
        printf("GPU ");
    }
    if (device_type & CL_DEVICE_TYPE_ACCELERATOR) {
        printf("Accelerator ");
    }
    if (device_type & CL_DEVICE_TYPE_DEFAULT) {
        printf("Default ");
    }
    printf("\n");
}

int main() {
    cl_uint num_platforms;
    clGetPlatformIDs(0, NULL, &num_platforms);
    printf("Number of platforms: %u\n", num_platforms);

    cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
    clGetPlatformIDs(num_platforms, platforms, NULL);

    for (cl_uint i = 0; i < num_platforms; i++) {
        printf("Platform #%u:\n", i);
        printPlatformInfo(platforms[i]);

        cl_uint num_devices;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
        cl_device_id* devices = (cl_device_id*)malloc(sizeof(cl_device_id) * num_devices);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

        printf("\tNumber of devices: %u\n", num_devices);
        for (cl_uint j = 0; j < num_devices; j++) {
            printDeviceInfo(devices[j]);
        }
        free(devices);
    }

    free(platforms);
    return 0;
}

