//
// Created by Yewert on 27-Mar-18.
//

#include "MemoryPartDescriptor.h"

MemoryPartDescriptor::MemoryPartDescriptor(unsigned char log2OfSize) :
    isFree(true), log2OfSize(log2OfSize) {
  this->body.setNext(nullptr);
}

bool MemoryPartDescriptor::IsFree() const {
  return isFree;
}

void MemoryPartDescriptor::setNext(MemoryPartDescriptor *next) {
  MemoryPartDescriptor::isFree = true;
  MemoryPartDescriptor::body.setNext(next);
}

void MemoryPartDescriptor::setDataAddress(void *data) {
  MemoryPartDescriptor::isFree = false;
  MemoryPartDescriptor::body.setData(data);
}

Selector &MemoryPartDescriptor::getBody() {
  return body;
}

unsigned char MemoryPartDescriptor::getLog2OfSize() const {
  return log2OfSize;
}
void MemoryPartDescriptor::setLog2OfSize(unsigned char log2OfSize) {
  MemoryPartDescriptor::log2OfSize = log2OfSize;
}
MemoryPartDescriptor::MemoryPartDescriptor(unsigned char log2OfSize, void *data) {
  MemoryPartDescriptor::log2OfSize = log2OfSize;
  body.setData(data);
}

