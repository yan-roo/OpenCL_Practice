//
//  main.cpp
//  Environment
//
//  Created by Yan Roo on 2025/2/28.
//

#include <iostream>
#include <vector>
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include "CL/opencl.hpp"

using namespace std;

int main(){
    cout << "Hello" << endl;
    vector<cl::Platform> all_platform;
    return 0;
}
