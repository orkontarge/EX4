//
// Created by avigayil on 17/05/2022.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"


void traversingTree(uint64_t virtualAddress, uint64_t *addr);

word_t findFreeFrame(uint64_t virtualAddress, const word_t *parentOfNewFrame);

void restorePage(word_t frameIndex, uint64_t virtualAddress);


uint64_t min(uint64_t number1, uint64_t number2);

uint64_t absOfMinus(uint64_t num1, uint64_t num2) {
    if (num1 >= num2) {
        return num1 - num2;
    } else {
        return (num2 - num1);
    }
}
uint64_t findPSize(uint64_t depth){
    uint64_t pSize = OFFSET_WIDTH;
    if ((depth == TABLES_DEPTH - 1) && ((VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH) != 0)) {
        pSize = VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH;
    }
    return pSize;
}
uint64_t findP(uint64_t depth, uint64_t &address) {
    uint64_t pSize = findPSize(depth);
    uint64_t p = address >> (VIRTUAL_ADDRESS_WIDTH - pSize);
    address <<= pSize;
    uint64_t shifter = 1 << VIRTUAL_ADDRESS_WIDTH;
    address &= shifter - 1;
    return p;
}

void
treeDFS(word_t root, word_t depth, word_t frameToNotEvict, word_t *maxFrame, bool *foundEmptyFrame,
        uint64_t currAddress, uint64_t pageToSwapIn, word_t *FrameToEvict, word_t *parentOfPageToEvict,
        uint64_t *pageToEvict, word_t *frameOfZeros, uint64_t parentOfEmptyFrame);

void fillFrameWithValue(uint64_t address, word_t value) {
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        PMwrite(address + i, value);
    }
}

void restorePage(word_t frameIndex, uint64_t virtualAddress) {
    uint64_t pageIndex = virtualAddress >> (OFFSET_WIDTH);
    PMrestore(frameIndex, pageIndex);
}


void VMinitialize() {
    fillFrameWithValue(0, 0);
    /*
     * intialize with frame 0 as root, can initialize full tree based on constants sizes, initialize freeframelist range(NUM_FRAMES)
     * class of pagetable (node in tree) - hold the table, pages number: frame address, pointer to next table, validation
     * store each table in a frameindex.
    */
}


int VMread(uint64_t virtualAddress, word_t *value) {

    if (virtualAddress >= VIRTUAL_MEMORY_SIZE) {
        return 0;
    }
    uint64_t PMReadingAddress;
    traversingTree(virtualAddress, &PMReadingAddress);
    PMread(PMReadingAddress, value);
    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {

    if (virtualAddress >= VIRTUAL_MEMORY_SIZE) {
        return 0;
    }
    uint64_t PMWritingAddress;
    traversingTree(virtualAddress, &PMWritingAddress);
    PMwrite(PMWritingAddress, value);
    return 1;
}

word_t findFrameOfPage(uint64_t pageNumber) {

    uint64_t virtualAddress = pageNumber * PAGE_SIZE;
    uint64_t remainAddress = virtualAddress;
    word_t pointerFrame = 0;
    uint64_t currAddress;
    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {

        uint64_t p =  findP(i, remainAddress);

        uint64_t frameAddress = (pointerFrame * PAGE_SIZE);
        currAddress = frameAddress + p;
        PMread(currAddress, &pointerFrame);
        if (pointerFrame == 0) {
            return 0;
        }

    }
    return pointerFrame;
}

void traversingTree(uint64_t virtualAddress,
                    uint64_t *addr) {

    uint64_t remainAddress = virtualAddress;
    word_t futureFrame = 0;
    word_t parentFrame = 0;
    uint64_t currAddress;
    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {

        uint64_t p = findP(i, remainAddress);
        uint64_t frameAddress = (futureFrame * PAGE_SIZE);
        currAddress = frameAddress + p;
        parentFrame = futureFrame;
        PMread(currAddress, &futureFrame);
        if (futureFrame == 0) {
            word_t freeFrame = findFreeFrame(virtualAddress, &parentFrame);
            uint64_t freeFrameAddress = freeFrame * PAGE_SIZE;
            if (i < (TABLES_DEPTH - 1)) {
                fillFrameWithValue(freeFrameAddress,
                                   0);
            } else {
                restorePage(freeFrame, virtualAddress);
            }
            PMwrite(currAddress, freeFrame);
            futureFrame = freeFrame;
        }

    }
    uint64_t dOnes = (1LL << OFFSET_WIDTH) - 1;
    uint64_t d = virtualAddress & dOnes;
    *addr = futureFrame * PAGE_SIZE + d;

}

void
treeDFS(word_t root, word_t depth, word_t frameToNotEvict, word_t *maxFrame, bool *foundEmptyFrame,
        uint64_t currAddress, uint64_t pageToSwapIn, word_t *FrameToEvict, word_t *parentOfPageToEvict,
        uint64_t *pageToEvict, word_t *frameOfZeros, uint64_t parentOfEmptyFrame) {

    //doing DFS while at the same time check all parameters for frame

    bool isFrameEmpty = true;

    if (*foundEmptyFrame) {
        return;
    }
    if (depth == TABLES_DEPTH) {
        return;
    }

    for (word_t i = 0; i < PAGE_SIZE; i++) { // iterate over the root's sons
        word_t currentRow = 0;
        uint64_t frameAddress = root * PAGE_SIZE;
        PMread(frameAddress + i, &currentRow);
        if (currentRow > *maxFrame) {
            *maxFrame = currentRow;
        }
        if (currentRow != 0) {
            uint64_t pSize = findPSize(depth);

            if (depth == TABLES_DEPTH - 1) {
                // means it is a leaf. Calculate distance between pages for evicting later if needed
                if (*FrameToEvict == -1) { //means we didn't fill it
                    *FrameToEvict = currentRow;
                    *parentOfPageToEvict = root;
                    *pageToEvict = (currAddress << pSize) + i;
                } else {
                    uint64_t leafSon = (currAddress << pSize) + i;
                    uint64_t distance1 = min(absOfMinus(pageToSwapIn, leafSon),
                                             (NUM_PAGES - absOfMinus(pageToSwapIn, leafSon)));
                    uint64_t distance2 = min(absOfMinus(pageToSwapIn, *pageToEvict),
                                             (NUM_PAGES -
                                              absOfMinus(pageToSwapIn, *pageToEvict))); //TODO: check long long
                    if (distance1 > distance2) {
                        *pageToEvict = (currAddress << pSize) + i;
                        *FrameToEvict = findFrameOfPage(*pageToEvict);
                        *parentOfPageToEvict = root;
                    }
                }
            }
            isFrameEmpty = false;

            treeDFS(currentRow, depth + 1, frameToNotEvict, maxFrame, foundEmptyFrame,
                    (currAddress << pSize) + i, pageToSwapIn, FrameToEvict,
                    parentOfPageToEvict,
                    pageToEvict, frameOfZeros, root * PAGE_SIZE + i);
        }

    }
    if (isFrameEmpty) {
        if (root == 0) {
            return;
        }
        if (root != frameToNotEvict) {
            PMwrite(parentOfEmptyFrame, 0);
            *frameOfZeros = root;
            *foundEmptyFrame = true;
        }
    }

}


word_t findFreeFrame(uint64_t virtualAddress, const word_t *parentOfNewFrame) {


    uint64_t pageNum = virtualAddress >> OFFSET_WIDTH;
    word_t maxFrame = 0;
    word_t frameToEvict = -1;
    word_t numOfParentOfPageToEvict = -1;
    word_t freeFrameOfZeros = 0;
    uint64_t pageToEvict = 0;
    bool isFrameEmpty = false;

    treeDFS(0, 0, *parentOfNewFrame,
            &maxFrame, &isFrameEmpty, 0,
            pageNum, &frameToEvict,
            &numOfParentOfPageToEvict, &pageToEvict, &freeFrameOfZeros,
            0);


    if (isFrameEmpty) {
        return freeFrameOfZeros;
    }
    if (maxFrame < NUM_FRAMES - 1) {
        return maxFrame + 1;
    }
    //third case - need to evict page
    PMevict(frameToEvict, pageToEvict);

    word_t pointer; //delete the parent
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        PMread(i + numOfParentOfPageToEvict * PAGE_SIZE, &pointer);
        if (pointer == frameToEvict) {
            PMwrite(i + numOfParentOfPageToEvict * PAGE_SIZE, 0);
        }
    }
    return frameToEvict;

}


uint64_t min(uint64_t number1, uint64_t number2) {
    if (number1 >= number2) {
        return number2;
    }
    return number1;
}
