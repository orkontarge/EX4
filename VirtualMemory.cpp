//
// Created by avigayil on 17/05/2022.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"


void traversingTree(uint64_t virtualAddress, word_t *addr);

word_t findFreeFrame(uint64_t virtualAddress);

void restorePage(uint64_t physicalAddress, uint64_t virtualAddress);

void fillFrameWithValue(uint64_t address, word_t value) {
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        PMwrite(address + i, value);
    }
}

void restorePage(uint64_t physicalAddress, uint64_t virtualAddress) {
    //TODO: need to check if there can be a problem with the input
    uint64_t vmAddress = virtualAddress >> (OFFSET_WIDTH);
    PMrestore(physicalAddress, vmAddress);
}


void VMinitialize() {
    fillFrameWithValue(0, 0);
    /*
     * intialize with frame 0 as root, can initialize full tree based on constants sizes, initialize freeframelist range(NUM_FRAMES)
     * class of pagetable (node in tree) - hold the table, pages number: frame address, pointer to next table, validation
     * store each table in a frameindex.
    */
    //TODO: make sure that the page size == frame size == offset_size(log2)
    // (notice- page_size != table_size)
    //TODO: need to add "MAX_INDEX"
    //TODO: need to check if the constants can be zero- which means that there are no memory
}


int VMread(uint64_t virtualAddress, word_t *value) { //TODO: NEED TO CHECK IF THE SIZE INPUT IS OK
    //TODO: need to add scenarios when it return false
    word_t PMReadingAddress;
    traversingTree(virtualAddress, &PMReadingAddress);
    PMread(PMReadingAddress, value);
    return 1;
    //TODO: what happens if we don't have what to read
}

int VMwrite(uint64_t virtualAddress, word_t value) { //TODO: NEED TO CHECK IF THE SIZE INPUT IS OK
    //TODO: need to add scenarios when it return false
    word_t PMWritingAddress;
    traversingTree(virtualAddress, &PMWritingAddress);
    PMwrite(PMWritingAddress, value);
    return 1;
}


void traversingTree(uint64_t virtualAddress,
                    word_t *addr) { //TODO: need to check if all tables suppose to be with the same size
    int pSize = (VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / TABLES_DEPTH; //TODO: don't know if the size is accurate
    uint64_t onlyOnes = (1LL << pSize) - 1;
    uint64_t mask = onlyOnes << (VIRTUAL_ADDRESS_WIDTH - pSize); //ones with the size of pSize and after that zeros
    int currAddress = 0; //TODO: need to check when its suppose to be int and when uint64_t

    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {
        uint64_t p = mask & virtualAddress;
        uint64_t frameNum = (currAddress * PAGE_SIZE);
        PMread(frameNum + p, &currAddress);
        //TODO: need to check if need to check if frameNum is too much big
        if (currAddress == 0) {
            word_t freeFrameAddress = findFreeFrame(virtualAddress); //TODO: need to implement
            //TODO: need to check if a tree can be not full
            if (i < TABLES_DEPTH - 1) { //means that it's not a leaf
                fillFrameWithValue(freeFrameAddress, 0);
            } else {
                restorePage(virtualAddress, freeFrameAddress);
            }
            PMwrite(currAddress, freeFrameAddress);
            currAddress = freeFrameAddress;
        }

        mask >> pSize;
    }
    uint64_t d = (1LL << OFFSET_WIDTH);
    PMread(currAddress + d, addr);
}
//TESTING -
// Find an unused frame or evict a page from some frame. return page or index
//TODO: need to check in total what to do with word_t