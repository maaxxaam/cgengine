#include "allocstructs.h"
#include "vmalloc.h"

void AllocatedBuffer::destroy() {
    VMAlloc.destroyBuffer(*this);
}

void AllocatedImage::destroy() {
    VMAlloc.destroyImage(*this);
}
