//
//  simple_add.cpp
//  Environment
//
//  Created by Yan Roo on 2025/2/28.
//

__kernel void simple_add(__global const int *A, __global const int *B,
                         __global int *C) {
    int id = get_global_id(0);
    C[id] = A[id] + B[id];
}
