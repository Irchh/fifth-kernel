#include <printf.h>
#include "frame.h"
#include "mem.h"
#include "fdt.h"

uint8_t frames[32768]; // 4096 * 8 = enough data to allocate up to 1 GiB in frames. Each 4096 bytes is equivalent to 128 MiB in 4KiB frames.

size_t kernel_start_addr;
size_t kernel_end_addr;

size_t pages(size_t bytes) {
    return (bytes + 4095)/4096;
}

size_t allocate_frame(size_t frame) {
    if (frame >= sizeof(frames)) {
        return -1;
    }
    size_t byte = frame/8;
    size_t bit = frame%8;
    frames[byte] |= 1<<bit;
    return frame;
}

size_t allocate_first_frame() {
    return allocate_frame(first_free_frame());
}

void deallocate_frame(size_t frame) {
    if (frame >= sizeof(frames)) {
        return;
    }
    size_t byte = frame/8;
    size_t bit = frame%8;
    frames[byte] &= ~(uint8_t)(1<<bit);
}

void allocate_n_frames(size_t start, size_t count) {
    printf("Allocating frames %#zx-%#zx\n", start, start+count-1);

    for (size_t i = start; i < (start+count); i++) {
        allocate_frame(i);
    }
}

void init_frames() {
    kernel_start_addr = (size_t)&kernel_start_ptr;
    kernel_end_addr = (size_t)&kernel_end_ptr;
    for (size_t i = 0; i < sizeof(frames); i++) {
        frames[i] = 0;
    }

    allocate_n_frames(page_of(kernel_start_addr - device_information.ram_start), pages(kernel_end_addr - kernel_start_addr));
    allocate_n_frames(page_of((size_t)device_information.dtb - device_information.ram_start), pages(device_information.dtb_size));
}

size_t first_free_frame() {
    size_t i = 0;
    while (frames[i] == 0xFF) {
        i++;
    }
    size_t j = 0;
    while (((frames[i]>>j)&1) == 1) {
        j++;
    }
    return i*8 + j;
}
