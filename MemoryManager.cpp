//
// Created by Yewert on 27-Mar-18.
//

#include "MemoryManager.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

using namespace std;

void* MemoryManager::allocate(size_t amountOfBytes) {
  MemoryPartDescriptor* targetDescriptor = getTargetDescriptor(amountOfBytes);
  if (targetDescriptor != nullptr) {
    targetDescriptor = getMinimizedDescriptor(amountOfBytes, targetDescriptor);
    return targetDescriptor->getBody().getData();
  }
  return nullptr;
}

MemoryPartDescriptor* MemoryManager::getMinimizedDescriptor(size_t amountOfBytes,
                                                            MemoryPartDescriptor* targetDescriptor) const {
  size_t capacity = ((size_t) 1) << (unsigned char) (targetDescriptor->getLog2OfSize() - 1);
  while ((capacity - sizeof(MemoryPartDescriptor)) >= amountOfBytes) {
    MemoryPartDescriptor* newDescriptor = bisectDescriptor(targetDescriptor, capacity);
    targetDescriptor->setNext(nullptr);
    auto newParent = descriptorTable[targetDescriptor->getLog2OfSize() - powerOffset];
    if (newParent == nullptr) {
      descriptorTable[targetDescriptor->getLog2OfSize() - powerOffset] = targetDescriptor;
    } else {
      newParent = getLastDescriptorInTable(newParent);
      newParent->setNext(targetDescriptor);
    }
    targetDescriptor = newDescriptor;
    capacity = ((size_t) 1) << (targetDescriptor->getLog2OfSize() - 1);
  }
  return targetDescriptor;
}

MemoryPartDescriptor* MemoryManager::getLastDescriptorInTable(MemoryPartDescriptor* current) const {
  if (current == nullptr) {
    throw invalid_argument("Argument was nullptr");
  }
  while (current->getBody().getNext() != nullptr) {
    current = current->getBody().getNext();
  }
  return current;
}

MemoryPartDescriptor* MemoryManager::bisectDescriptor(MemoryPartDescriptor* targetDescriptor, size_t capacity) const {
  MemoryPartDescriptor* newDescriptor = reinterpret_cast<MemoryPartDescriptor*>((void*) targetDescriptor + capacity);
  *newDescriptor = MemoryPartDescriptor((unsigned char) (targetDescriptor->getLog2OfSize() - 1), newDescriptor + 1);
  targetDescriptor->setLog2OfSize((unsigned char) (targetDescriptor->getLog2OfSize() - 1));
  return newDescriptor;
}

MemoryPartDescriptor* MemoryManager::getTargetDescriptor(size_t amountOfBytes) const {
  MemoryPartDescriptor* targetDescriptor = nullptr;
  for (int i = 0; i < log2OfTotalMemory - powerOffset + 1; ++i) {
    if (descriptorTable[i] != nullptr
        && isBigEnough(amountOfBytes, descriptorTable[i]->getLog2OfSize())) {
      MemoryPartDescriptor* current;
      MemoryPartDescriptor* previous;
      getLastPairOfDescriptorsInTable(i, current, previous);
      if (previous == current) {
        descriptorTable[i] = nullptr;
      } else {
        previous->getBody().setNext(nullptr);
      }
      current->setDataAddress(sizeof(MemoryPartDescriptor) + reinterpret_cast<void*>(current));
      targetDescriptor = current;
      break;
    }
  }
  return targetDescriptor;
}

bool MemoryManager::isBigEnough(const size_t amountOfBytes, const unsigned char log2OfManagedDataSize) const {
  return amountOfBytes <= ((((size_t) 1) << log2OfManagedDataSize) - sizeof(MemoryPartDescriptor));
}

void MemoryManager::getLastPairOfDescriptorsInTable(const int targetPower,
                                                    MemoryPartDescriptor*&current,
                                                    MemoryPartDescriptor*&previous) const {
  current = descriptorTable[targetPower];
  previous = descriptorTable[targetPower];
  while (current->getBody().getNext() != nullptr) {
    previous = current;
    current = current->getBody().getNext();
  }
}

void MemoryManager::free(void* address) {
  auto correspondingDescriptor = reinterpret_cast<MemoryPartDescriptor*>(address - sizeof(MemoryPartDescriptor));
  correspondingDescriptor->setNext(nullptr);
  while (correspondingDescriptor->getLog2OfSize() != log2OfTotalMemory) {
    size_t relativeAddress = (void*) correspondingDescriptor - startOfMemoryBlock;
    size_t size = ((size_t) 1 << correspondingDescriptor->getLog2OfSize());
    size_t index = relativeAddress == 0 ? 0 : size / relativeAddress;
    MemoryPartDescriptor* brotherDescriptor = nullptr;
    if (index % 2 == 0) {
      brotherDescriptor = reinterpret_cast<MemoryPartDescriptor*>(
          reinterpret_cast<void*>(correspondingDescriptor) + size);
    } else {
      brotherDescriptor = reinterpret_cast<MemoryPartDescriptor*>(
          reinterpret_cast<void*>(correspondingDescriptor) - size);
    }
    if (brotherDescriptor->getLog2OfSize() != correspondingDescriptor->getLog2OfSize()
        || !brotherDescriptor->IsFree()) {
      addToDescriptorTable(correspondingDescriptor);
      return;
    }
    //TODO: delete brother from descriptorTable
    if (index % 2 != 0) {
      auto temp = correspondingDescriptor;
      correspondingDescriptor = brotherDescriptor;
      brotherDescriptor = temp;
    }

    *correspondingDescriptor =
        MemoryPartDescriptor(static_cast<unsigned char>(correspondingDescriptor->getLog2OfSize() + 1));
  }
  correspondingDescriptor->setNext(nullptr);
  descriptorTable[correspondingDescriptor->getLog2OfSize() - powerOffset] = correspondingDescriptor;
}

MemoryManager::MemoryManager(unsigned char log2OfTotalMemory) {
  MemoryManager::log2OfTotalMemory = log2OfTotalMemory;
  descriptorTable = new MemoryPartDescriptor* [log2OfTotalMemory - powerOffset + 1];
  for (int i = 0; i < log2OfTotalMemory - powerOffset; ++i) {
    descriptorTable[i] = nullptr;
  }
  size_t size = (size_t)1 << log2OfTotalMemory;
  startOfMemoryBlock = malloc(size);
  if (startOfMemoryBlock == nullptr)
    throw length_error("not enough space to allocate");
  descriptorTable[log2OfTotalMemory - powerOffset] = new(startOfMemoryBlock) MemoryPartDescriptor(log2OfTotalMemory);
}

void MemoryManager::addToDescriptorTable(MemoryPartDescriptor* newRecord) {
  auto current = descriptorTable[newRecord->getLog2OfSize() - powerOffset];
  if (current == nullptr) {
    descriptorTable[newRecord->getLog2OfSize() - powerOffset] = newRecord;
    return;
  }
  current = getLastDescriptorInTable(current);
  current->setNext(newRecord);
}

void* MemoryManager::getStartOfMemoryBlock() const {
  return startOfMemoryBlock;
}

MemoryManager::~MemoryManager() {
  free(startOfMemoryBlock);
  delete[] descriptorTable;
}

