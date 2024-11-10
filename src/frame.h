#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <stddef.h>

void init_frames();
size_t first_free_frame();
size_t allocate_frame(size_t frame);
size_t allocate_first_frame();
void deallocate_frame(size_t frame);
void allocate_n_frames(size_t frame, size_t count);

extern const size_t kernel_start_ptr;
extern const size_t kernel_end_ptr;
extern size_t kernel_start_addr;
extern size_t kernel_end_addr;

#endif
