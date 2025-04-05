__kernel void dot_product_attention(__global const float *Q,
                                    __global const float *K,
                                    __global float *score, const int seq_len,
                                    const int head_dim, const float scale,
                                    __local float *localK) {

    // Q/K [batch, seq_len, head_dim]
    // score [batch, seq_len, seq_len]
    // scale (1/sqrt(head_dim))
    // localK: to store a tile of K, tile_size * head_dim

    int q_index = get_global_id(0); // [0, seq_len]
    int batch = get_global_id(1);

    int batchOffset = batch * seq_len * head_dim;
    int q_start = batchOffset + q_index * head_dim;

    // tile_size is determined by the local work-group size (first dimension)
    int tile_size = get_local_size(0);

    for (int tile = 0; tile < seq_len; tile += tile_size) {
        int local_id = get_local_id(0);
        int global_k = tile + local_id;
        for (int d = 0; d < head_dim; d++) {
            int local_index = local_id * head_dim + d;
            if (global_k < seq_len)
                localK[local_index] = K[batchOffset + global_k * head_dim + d];
            else
                localK[local_index] = 0.0f;
        }
        // sync workgroup to make sure localK is filled
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int k = 0; k < tile_size; k++) {
            if (tile + k >= seq_len)
                break;
            float dot = 0.0f;
            for (int d = 0; d < head_dim; d++)
                dot += Q[q_start + d] * localK[k * head_dim + d];
            int score_index =
                batch * (seq_len * seq_len) + q_index * seq_len + (tile + k);
            score[score_index] = dot * scale;
        }
        // sync before loading the next tile
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}
