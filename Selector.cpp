//
// Created by Yewert on 27-Mar-18.
//

#include "MemoryPartDescriptor.h"

void* Selector::getData() const {
  return data;
}

void Selector::setData(void* data) {
  Selector::data = data;
}

MemoryPartDescriptor* Selector::getNext() const {
  return next;
}

void Selector::setNext(MemoryPartDescriptor* next) {
  Selector::next = next;
}