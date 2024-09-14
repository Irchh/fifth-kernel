#include <string.h>

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a-*(const unsigned char*)b;
}

int strncmp(const char* a, const char* b, size_t count) {
    while (count && *a && (*a == *b)) {
        a++;
        b++;
        count--;
    }
    return count == 0 ? 0 : *(const unsigned char*)a-*(const unsigned char*)b;
}
