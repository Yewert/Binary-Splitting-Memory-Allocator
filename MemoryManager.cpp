//
// Created by Yewert on 27-Mar-18.
//

#include "MemoryManager.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>

using namespace std;

void* MemoryManager::allocate(size_t amountOfBytes) {
  MemoryPartDescriptor* targetDescriptor = getTargetDescriptor(amountOfBytes);
  if (targetDescriptor != nullptr) {
    targetDescriptor = getMinimizedDescriptor(amountOfBytes, targetDescriptor);
    targetDescriptor->setDataAddress(targetDescriptor + 1);
    return targetDescriptor->getDataAddress();
  }
  return nullptr;
}

MemoryPartDescriptor* MemoryManager::getMinimizedDescriptor(size_t amountOfBytes,
                                                            MemoryPartDescriptor* targetDescriptor) {
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
  while (current->getNext() != nullptr) {
    current = current->getNext();
  }
  return current;
}

MemoryPartDescriptor* MemoryManager::bisectDescriptor(MemoryPartDescriptor* targetDescriptor, size_t capacity) {
  MemoryPartDescriptor* newDescriptor = reinterpret_cast<MemoryPartDescriptor*>((void*) targetDescriptor + capacity);
  *newDescriptor = MemoryPartDescriptor((unsigned char) (targetDescriptor->getLog2OfSize() - 1));
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
        previous->setNext(nullptr);
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
  while (current->getNext() != nullptr) {
    previous = current;
    current = current->getNext();
  }
}

void MemoryManager::freeSpace(void* address) {
  auto correspondingDescriptor = reinterpret_cast<MemoryPartDescriptor*>(address - sizeof(MemoryPartDescriptor));
  correspondingDescriptor->setNext(nullptr);
  while (correspondingDescriptor->getLog2OfSize() != log2OfTotalMemory) {
    size_t relativeAddress = (size_t) correspondingDescriptor - (size_t) startOfMemoryBlock;
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
    removeDescriptorFromTheTable(brotherDescriptor);
    if (index % 2 != 0) {
      correspondingDescriptor = brotherDescriptor;
    }

    *correspondingDescriptor =
        MemoryPartDescriptor(static_cast<unsigned char>(correspondingDescriptor->getLog2OfSize() + 1));
  }
  correspondingDescriptor->setNext(nullptr);
  descriptorTable[correspondingDescriptor->getLog2OfSize() - powerOffset] = correspondingDescriptor;
}
void MemoryManager::removeDescriptorFromTheTable(MemoryPartDescriptor* outdatedRecord) {
  auto cur = descriptorTable[outdatedRecord->getLog2OfSize() - powerOffset];
  if (cur == outdatedRecord) {
    descriptorTable[outdatedRecord->getLog2OfSize() - powerOffset] = nullptr;
  } else {
    while (cur != nullptr) {
      if (cur->getNext() == outdatedRecord) {
        cur->setNext(outdatedRecord->getNext());
        break;
      }
      cur = cur->getNext();
    }
  }
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
void MemoryManager::printSummary() {
  void* pointer = startOfMemoryBlock;
  size_t size = (size_t) 1 << log2OfTotalMemory;
  cout << setw(10) << "address" << setw(10) << "rel_addr" << setw(10) << "size" << setw(10) << "state" << endl;
  void* boundary = pointer + size;
  while (pointer < boundary) {
    auto relative_address = (size_t) pointer - (size_t) startOfMemoryBlock;
    auto descriptor = reinterpret_cast<MemoryPartDescriptor*>(pointer);
    size_t descriptor_size = (size_t) 1 << descriptor->getLog2OfSize();
    cout << setw(10) << descriptor << setw(10) << relative_address << setw(10) << descriptor_size << setw(10)
         << (descriptor->IsFree() ? "free" : "occupied") << endl;
    pointer += descriptor_size;
  }
  cout << endl;
}

