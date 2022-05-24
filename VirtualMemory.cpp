//
// Created by avigayil on 17/05/2022.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define ROOT 0

void traversingTree(uint64_t virtualAddress, word_t *addr);

uint64_t findFreeFrame(const uint64_t virtualAddress, const uint64_t *parentsPointer); //TODO: need to change word_t

void restorePage(uint64_t physicalAddress, uint64_t virtualAddress);

bool isFrameContainsOnlyZeros(uint64_t frame);

word_t min(word_t number1, word_t number2);

/**
 * the function does DFS on the tree and find a frame with zeros.
 * Also, update what is the max used frame and what is the biggest distance from a frame that will define by the
 * given address
 * @param root the root of the tree
 * @param depth the depth of the root
 * @param pageAddress the address to looking for
 * @param frameToNotEvict frame that can't be evict
 * @param maxFrame pointer to maxFrame
 * @param pageToEvict pointer to pageToEvict
 * @return  zero's frame. 0 if there aren't any frame like this.
 */
word_t treeDFS(word_t root,
               word_t depth,
               uint64_t pageAddress, //TODO: check if needed
               word_t frameToNotEvict,
               word_t *maxFrame,
               word_t *pageToEvict);

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
    uint64_t pSize = (VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / TABLES_DEPTH; //TODO: don't know if the size is accurate
    uint64_t pOnes = (1LL << pSize) - 1;
    uint64_t mask = pOnes << (VIRTUAL_ADDRESS_WIDTH - pSize); //ones with the size of pSize and after that zeros
    word_t currAddress = 0; //TODO: need to check when its suppose to be int and when uint64_t
    uint64_t *preAddress = nullptr;
    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {
        uint64_t p = mask & virtualAddress;
        uint64_t frameNum = (currAddress * PAGE_SIZE);
        PMread(frameNum + p, &currAddress);
        //TODO: need to check if frameNum is too much big
        if (currAddress == 0) {
            word_t freeFrameAddress = findFreeFrame(virtualAddress, preAddress); //TODO: need to implement
            //TODO: need to check if a tree can be not full
            if (i < TABLES_DEPTH) {
                fillFrameWithValue(freeFrameAddress, 0);
            } else {
                restorePage(virtualAddress, freeFrameAddress);
            }
            *preAddress = currAddress;
            PMwrite(*preAddress, freeFrameAddress);
            currAddress = freeFrameAddress;
        }

        mask >> pSize;
    }
    uint64_t dOnes = (1LL << OFFSET_WIDTH) - 1;
    uint64_t d = virtualAddress & dOnes;
    PMread(currAddress + d, addr);
}

word_t treeDFS(word_t root, word_t depth, uint64_t pageAddress,
               word_t frameToNotEvict, word_t *maxFrame,
               uint64_t *pageToEvict) { //TODO: didn't update yet the maxFrame and pageToEvict
    //doing DFS while at the same time check all parameters for frame
    if (isFrameContainsOnlyZeros(root)) {
        if (root != frameToNotEvict)
            return root;
        return 0; //means there are not available frames with zeros
    }
    if (depth < TABLES_DEPTH) {  //TODO: I'm not sure about this condition
        for (word_t i = 0; i < PAGE_SIZE; i++) { // iterate over the root's sons
            word_t rowValue = 0;
            word_t frameAddress = root * PAGE_SIZE; //TODO: need to do something with the long long
            PMread(frameAddress + i, &rowValue);
            if (rowValue != 0) {
                if (rowValue > *maxFrame) {
                    *maxFrame = rowValue;
                }
                if (depth == TABLES_DEPTH - 1) { // means it is a leaf
                    word_t distance1 = min((pageAddress - rowValue),
                                           NUM_PAGES - (pageAddress - rowValue)); //TODO: check long long
                    word_t distance2 = min((pageAddress - *pageToEvict),
                                           NUM_PAGES - (pageAddress - *pageToEvict)); //TODO: check long long
                    if (distance1 > distance2) {
                        *pageToEvict = rowValue;
                        *(pageToEvict+1) = rowValue;
                    }
                }
                word_t result = treeDFS(root, depth + 1, pageAddress, frameToNotEvict, maxFrame, pageToEvict);
                if (result != 0) {
                    return result;
                }
            }
        }
        return 0;
    }
}

/***
 * checks if there are any unused frames
 * @return address of unused frame, null pointer if all frames are in use.
 */
//word_t unusedFrame(){ //TODO: maybe delete it
//    word_t value = 0;
//    word_t numFrameUsed;
//    for (uint64_t i = 0; i < NUM_FRAMES; ++i) { // TODO why in simple test it has a times 2 for NUM_FRAME
//        PMread(i, &numFrameUsed);
//        if (numFrameUsed > value) { // updating number of frames in use
//            value = numFrameUsed;
//        }
//        if (value < NUM_FRAMES) { // didn't use all the frames yet //TODO word_t vs long long??
//            return value + 1; // next unused frame address
//        }
//    }
//    return 0; // frame zero is never free
//        //TODO: what if all were used, but the some were evicted, so high frame number not assigned -> looks like unused
//        // TODO but really it is empty. can it mess with the tree structure?
//}




uint64_t findFreeFrame(const uint64_t virtualAddress, const uint64_t *parentsPointer) {

    // find an empty frame
    // if no empty frame -> find an unused frame -> return that address
    // if no an unused frame -> evict table, find frame to evict with cyclic algorithm
    if (isFrameContainsOnlyZeros(ROOT)) {
        return 1;
    }
    uint64_t pageAddress = virtualAddress >> OFFSET_WIDTH;
    word_t maxFrame = 0;
    uint64_t pageToEvict[2] = {0,0}; // first index the page. second index the frame where it settled.
    word_t frameOfZeros = treeDFS(0, 0, pageAddress, *parentsPointer, &maxFrame, pageToEvict);
    if (frameOfZeros) {
        return frameOfZeros;
    }
    if (maxFrame < NUM_PAGES - 1) {
        return maxFrame;
    } else {
        PMevict(*(pageToEvict+1),*pageToEvict);
        return *pageToEvict;
    }


    //TODO: need to notice that we can't evict frames that necessary for the process - delt with in DFS???????????/
    //TODO: the function dfs returns number of frame of zeros or null

    //TODO: if null- check if maxFrame smaller then numberFrame constant
    //TODO: if not- return the number of frame with maximal distance




}

bool isFrameContainsOnlyZeros(uint64_t frame) {
    word_t value;
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        PMread(frame + i, &value);
        if (value != 0) {
            return false;
        }
    }
    return true;
}

word_t min(word_t number1, word_t number2) {
    if (number1 >= number2) {
        return number1;
    }
    return number2;
}


//TODO: word_t vs unit_64???