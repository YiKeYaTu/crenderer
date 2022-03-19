kernel void test1(
    global int* nums
) {
    size_t gid = get_global_id(0);

    for (int i = 0; i < 1000; i ++) {
        nums[gid] += 1 * (i + 20);
    }
}

kernel void test2(
    global int* nums
) {
    size_t gid = get_global_id(0);
    // printf("%u\n", nums[gid]);
}