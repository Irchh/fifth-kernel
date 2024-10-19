#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <stddef.h>

void init_frames();
size_t first_free_frame();
void allocate_frame(size_t frame);
void deallocate_frame(size_t frame);
void allocate_n_frames(size_t frame, size_t count);

#endif
