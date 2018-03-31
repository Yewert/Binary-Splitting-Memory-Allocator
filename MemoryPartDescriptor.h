//
// Created by Yewert on 27-Mar-18.
//

#ifndef MEMORYALLOCATOR_MEMORYPARTDESCRIPTOR_H
#define MEMORYALLOCATOR_MEMORYPARTDESCRIPTOR_H

#include "Selector.h"

class MemoryPartDescriptor {

private:
    Selector body{
            Selector()
    };
public:
    Selector &getBody();

private:
    bool isFree{true};
    unsigned char log2OfSize{};
 public:
  void setLog2OfSize(unsigned char log2OfSize);
 public:
    unsigned char getLog2OfSize() const;

    bool IsFree() const;

    void setNext(MemoryPartDescriptor *next);

    void setDataAddress(void *data);

    MemoryPartDescriptor() = default;

    explicit MemoryPartDescriptor(unsigned char log2OfSize);
    explicit MemoryPartDescriptor(unsigned char log2OfSize, void *data);
};


#endif //MEMORYALLOCATOR_MEMORYPARTDESCRIPTOR_H