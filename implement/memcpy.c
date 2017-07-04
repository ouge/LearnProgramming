void* memcpy(void* dst, const void* src, size_t len) {
    if (NULL == dst || NULL == src) { return NULL; }
    void* ret = dst;
    if (dst <= src) {
        // 如果dst在src之前, 从前往后复制
        while (len--) {
            *(char*)dst = *(char*)src;
            dst         = (char*)dst + 1;
            src         = (char*)src + 1;
        }
    } else {
        // 如果dst在src之后, 从后往前复制
        src = (char*)src + len - 1;
        dst = (char*)dst + len - 1;
        while (len--) {
            dst = (char*)dst - 1;
            src = (char*)src - 1;
        }
    }
    return ret;
}