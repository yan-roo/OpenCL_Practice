__kernel void linear_projection(__global const float *X,
                                __global const float *W, __global float *Q,
                                const int seq_len, const int embed_dim,
                                const int head_dim) {
    // X [batch_size, seq_len, embed_dim] (b=1, s=2, e=3) [[1,2,3],[4,5,6]] -> [1,2,3,4,5,6]
    // W [embed_dim, head_dim] (e=3, h=2) [[1,0],[0,1],[1,1]] -> [1,0,0,1,1,1]
    // Q(Query, output) [batch_size, seq_len, head_dim] (b=1, s=2, h=2)
    // Q1-1 = X1*W1 = [1,2,3] * [1,0,1] = 1+0+3 = 4
    // Q1-2 = X1*W2 = [1,2,3] * [0,1,1] = 0+2+3 = 5
    // Q1 = [4,5]
    // Q2-1 = X2*W1 = [4,5,6] * [1,0,1] = 4+0+6 = 10
    // Q2-2 = X2*W2 = [4,5,6] * [0,1,1] = 0+5+6 = 11
    // Q2 = [10,11]
    // Q = [4,5,10,11]
    
    int token = get_global_id(0);
    int batch = get_global_id(1);
    int batchOffset = batch * seq_len * embed_dim;
    int outputBatchOffset = batch * seq_len * head_dim;
    
    // the start offset of the token that inside the X matrix
    int x_start = batchOffset + token * embed_dim;
    // the start offset of the token that inside the Q(output) matrix
    int q_start = outputBatchOffset + token * head_dim;
    
    for (int i = 0; i < head_dim; i++) {
        float sum = 0.0f;
        for (int j = 0; j < embed_dim; j++) {
            sum += X[x_start + j] * W[j * head_dim + i];
        }
        Q[q_start + i] = sum;
    }
}
