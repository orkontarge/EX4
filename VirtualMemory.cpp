//
// Created by avigayil on 17/05/2022.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"


void traversingTree(uint64_t virtualAddress, word_t *addr);

word_t *findFreeFrame(const word_t *parentsPointer); //TODO: need to change word_t

void restorePage(uint64_t physicalAddress, uint64_t virtualAddress);

bool isFrameContainsOnlyZeros(uint64_t frame);

/**
 * the function does DFS on the tree and find a frame with zeros.
 * Also, update what is the max used frame and what is the biggest distance from a frame that will define by the
 * given address
 * @param root the root of the tree
 * @param depth the depth of the root
 * @param pageAddress the address to looking for
 * @param frameToNotEvict frame that can't be evict
 * @param maxFrame pointer to maxFrame
 * @param maxDistance pointer to maxDistance
 * @return pointer to zero's frame. Null if there aren't any frame like this.
 */
word_t *treeDFS(word_t *root,
                word_t depth,
                uint64_t pageAddress,
                const word_t *frameToNotEvict,
                const word_t *maxFrame,
                const word_t *maxDistance);

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
    uint64_t onlyOnes = (1LL << pSize) - 1;
    uint64_t mask = onlyOnes << (VIRTUAL_ADDRESS_WIDTH - pSize); //ones with the size of pSize and after that zeros
    word_t currAddress = 0; //TODO: need to check when its suppose to be int and when uint64_t
    word_t *preAddress = nullptr;
    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {
        uint64_t p = mask & virtualAddress;
        uint64_t frameNum = (currAddress * PAGE_SIZE);
        PMread(frameNum + p, &currAddress);
        //TODO: need to check if frameNum is too much big
        if (currAddress == 0) {
            word_t *freeFrameAddress = findFreeFrame(preAddress); //TODO: need to implement
            //TODO: need to check if a tree can be not full
            if (i < TABLES_DEPTH - 1) { //means that it's not a leaf
                fillFrameWithValue(freeFrameAddress, 0);
            } else {
                restorePage(virtualAddress, freeFrameAddress);
            }
            PMwrite(currAddress, freeFrameAddress);
            *preAddress = currAddress;
            currAddress = freeFrameAddress;
        }

        mask >> pSize;
    }
    uint64_t d = (1LL << OFFSET_WIDTH);
    PMread(currAddress + d, addr);
}

word_t *treeDFS(word_t *root, word_t depth, uint64_t pageAddress,
                const word_t *frameToNotEvict, const word_t *maxFrame,
                const word_t *maxDistance) { //TODO: didn't update yet the maxFrame and maxDistance
    //doing DFS while at the same time check all parameters for frame
    if (isFrameContainsOnlyZeros(*root)) {
        if (*root != *frameToNotEvict)
            return root;
        return nullptr;
    }
    if (depth < TABLES_DEPTH) { //means we didn't come to a leaf  //TODO: I'm not sure about this condition
        for (word_t i = 0; i < PAGE_SIZE; i++) { // iterate over the root's sons
            word_t sonValue = 0;
            word_t numberOfFrame = *root * PAGE_SIZE; //TODO: need to do something with the long long
            PMread(numberOfFrame + i, &sonValue);
            if (sonValue != 0) {
                word_t *result = treeDFS(root, depth + 1, pageAddress, frameToNotEvict, maxFrame, maxDistance);
                if (result!= nullptr){
                    return result;
                }
            }
        }
        return nullptr;
    }
}

/***
 * checks if there are any unused frames
 * @return address of unused frame, null pointer if all frames are in use.
 */
word_t unusedFrame(){
    word_t value = 0;
    word_t numFrameUsed;
    for (uint64_t i = 0; i < NUM_FRAMES; ++i) { // TODO why in simple test it has a times 2 for NUM_FRAME
        PMread(i, &numFrameUsed);
        if (numFrameUsed > value) { // updating number of frames in use
            value = numFrameUsed;
        }
        if (value < NUM_FRAMES) { // didn't use all the frames yet //TODO word_t vs long long??
            return value + 1; // next unused frame address
        }
    }
    return 0; // frame zero is never free
        //TODO: what if all were used, but the some were evicted, so high frame number not assigned -> looks like unused
        // TODO but really it is empty. can it mess with the tree structure?
}




word_t findFreeFrame(
        const word_t *parentsPointer) {

        // if we have an unused frame -> return that address
        word_t freeAddress = unusedFrame();
        if (freeAddress != 0){
            return freeAddress;
        }
        else{// if all frames in use -> find an empty frame
            &freeAddress = treeDFS(word_t *root, word_t depth, uint64_t pageAddress, //TODO: fill properly
            const word_t *frameToNotEvict, const word_t *maxFrame,
            const word_t *maxDistance);

            if (&freeAddress != nullptr){
                return freeAddress
            }
            else{
                // if no empty table -> evict table, find frame to evict with cyclic algorithm

            }
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



//TODO: word_t vs unit_64???