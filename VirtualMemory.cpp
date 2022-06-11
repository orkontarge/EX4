//
// Created by avigayil on 17/05/2022.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream> //TODO: delete it


void printFrame(word_t frameIndex) {//TODO: delete it
    std::cout << "FRAME: " << frameIndex << std::endl;
    for (word_t d = 0; d < PAGE_SIZE; d++) {
        word_t value;
        PMread(frameIndex * PAGE_SIZE + d, &value);
        std::cout << value << " ";
    }
    std::cout << "\n";
}

void printTree() { //TODO:delete it
    for (uint64_t frame = 0; frame < NUM_FRAMES; frame++) {
        std::cout << "FRAME: " << frame << std::endl;
        for (word_t d = 0; d < PAGE_SIZE; d++) {
            word_t value;
            PMread(frame * PAGE_SIZE + d, &value);
            std::cout << value << " ";
        }
        std::cout << "\n\n\n";
    }
}

void traversingTree(uint64_t virtualAddress, uint64_t *addr);

word_t findFreeFrame(uint64_t virtualAddress, const word_t *parentOfNewFrame); //TODO: need to change word_t

void restorePage(word_t frameIndex, uint64_t virtualAddress);

bool isFrameContainsOnlyZeros(uint64_t frame);

uint64_t min(uint64_t number1, uint64_t number2);

uint64_t absOfMinus(uint64_t num1, uint64_t num2) {
    if (num1 >= num2) {
        return num1 - num2;
    } else {
        return (num2 - num1);
    }
}

uint64_t findP(uint64_t depth, uint64_t &address) {
    uint64_t pSize = OFFSET_WIDTH;
    if ((depth == TABLES_DEPTH - 1) && ((VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH) != 0)) {
        pSize = VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH;
    }
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
    //TODO: make sure that the page size == frame size == offset_size(log2)
    // (notice- page_size != table_size)
    //TODO: need to add "MAX_INDEX"
    //TODO: need to check if the constants can be zero- which means that there are no memory
}


int VMread(uint64_t virtualAddress, word_t *value) {

    if (virtualAddress >= VIRTUAL_MEMORY_SIZE) {
        return 0;
    }
    uint64_t PMReadingAddress;
    std::cout << "reading from virtualAddress " << virtualAddress << std::endl; //TODO: delete it

    traversingTree(virtualAddress, &PMReadingAddress);
    PMread(PMReadingAddress, value);
    printTree();
    return 1;
    //TODO: what happens if we don't have what to read
}

int VMwrite(uint64_t virtualAddress, word_t value) {

    if (virtualAddress >= VIRTUAL_MEMORY_SIZE) {
        return 0;
    }
    uint64_t PMWritingAddress;
    traversingTree(virtualAddress, &PMWritingAddress);
    PMwrite(PMWritingAddress, value);
    std::cout << "printing tree after writing " << value << std::endl; //TODO: delete it
    printTree(); //TODO: delete it
    return 1;
}

word_t findFrameOfPage(uint64_t pageNumber) {

    uint64_t virtualAddress = pageNumber * PAGE_SIZE;
    uint64_t pSize = OFFSET_WIDTH; //TODO: not sure yet
    uint64_t pOnes = (1LL << pSize) - 1;
    uint64_t mask = pOnes << (VIRTUAL_ADDRESS_WIDTH - pSize); //ones with the size of pSize and after that zeros
    word_t pointerFrame = 0;
    uint64_t currAddress;
    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {
        uint64_t zerosToAvoid = ((TABLES_DEPTH - i) * pSize);
        uint64_t p = (mask & virtualAddress) >> zerosToAvoid;
        uint64_t frameAddress = (pointerFrame * PAGE_SIZE);
        currAddress = frameAddress + p;
        PMread(currAddress, &pointerFrame);
        if (pointerFrame == 0) {
            return 0;
        }
        mask = mask >> pSize;

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

            if (depth == TABLES_DEPTH - 1) {
                // means it is a leaf. Calculate distance between pages for evicting later if needed
                if (*FrameToEvict == -1) { //means we didn't fill it
                    *FrameToEvict = currentRow;
                    *parentOfPageToEvict = root;
                    *pageToEvict = (currAddress << 1) + i;
                } else {
                    uint64_t leafSon = (currAddress << 1) + i;
                    uint64_t distance1 = min(absOfMinus(pageToSwapIn, leafSon),
                                             (NUM_PAGES - absOfMinus(pageToSwapIn, leafSon)));
                    uint64_t distance2 = min(absOfMinus(pageToSwapIn, *pageToEvict),
                                             (NUM_PAGES -
                                              absOfMinus(pageToSwapIn, *pageToEvict))); //TODO: check long long
                    if (distance1 > distance2) {
                        *pageToEvict = (currAddress << 1) + i;
                        *FrameToEvict = findFrameOfPage(*pageToEvict);
                        *parentOfPageToEvict = root;
                    }
                }
            }
            isFrameEmpty = false;

            treeDFS(currentRow, depth + 1, frameToNotEvict, maxFrame, foundEmptyFrame,
                    (currAddress << 1) + i, pageToSwapIn, FrameToEvict,
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
    word_t numOfParentOfEmptyTable = -1;
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



    //TODO: need to notice that we can't evict frames that necessary for the process - delt with in DFS???????????/
    //TODO: the function dfs returns number of frame of zeros or null

    //TODO: if null- check if maxFrame smaller then numberFrame constant
    //TODO: if not- return the number of frame with maximal distance


}

bool isFrameContainsOnlyZeros(uint64_t frame) {
    word_t value;
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        PMread(frame * PAGE_SIZE + i, &value);
        if (value != 0) {
            return false;
        }
    }
    return true;
}

uint64_t min(uint64_t number1, uint64_t number2) {
    if (number1 >= number2) {
        return number2;
    }
    return number1;
}


////TODO: word_t vs unit_64???

//#include "VirtualMemory.h"
//#include "PhysicalMemory.h"
//
//void evictAndReplace(word_t *frameIndex);
//
//void clearTable(uint64_t frameIndex) {
//    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
//        PMwrite(frameIndex * PAGE_SIZE + i, 0);
//    }
//}
//
//void VMinitialize() {
//    clearTable(0);
//}
//
//void DFS(int depth, word_t *freeFrameIndex, word_t *maxFrameIndex, word_t currentFrame, uint64_t currentFrameParentAddr,
//         word_t newFrameParent, bool *foundEmptyFrame) {
//    bool isFrameEmpty = true;
//    if (*foundEmptyFrame) {
//        return;
//    }
//    if (depth == TABLES_DEPTH){
//        return;
//    }
//    for (int i = 0; i < PAGE_SIZE; i++){
//        word_t currentRow;
//        PMread(((uint64_t )currentFrame)*PAGE_SIZE + i, &currentRow);
//        if (currentRow > *maxFrameIndex){
//            *maxFrameIndex = currentRow;
//        }
//        if (currentRow != 0){
//            DFS(depth + 1, freeFrameIndex, maxFrameIndex, currentRow, currentFrame * PAGE_SIZE + i, newFrameParent,
//                foundEmptyFrame);
//            isFrameEmpty = false;
//        }
//    }
//    if (isFrameEmpty){
//        if (currentFrame != 0) {
//            if (currentFrame != newFrameParent){
//                PMwrite (currentFrameParentAddr, 0); // remove pointer from parent
//                *freeFrameIndex = currentFrame;
//                *foundEmptyFrame = true;
//            }
//        }
//    }
//}
//
//
//void findAvailableFrame(word_t *freeFrame, word_t parentOfNewFrame) {
//    bool foundEmptyFrame = false;
//    word_t maxFrame = 0;
//    DFS(0, freeFrame, &maxFrame, 0, 0, parentOfNewFrame, &foundEmptyFrame);
//    if (!foundEmptyFrame){
//        *freeFrame = maxFrame + 1;
//    }
//    if (*freeFrame >= NUM_FRAMES) {
//        evictAndReplace(freeFrame);
//    }
//}
//
//
//void findMaxPath(word_t currentFrame, uint64_t currentPage, word_t currentParent, int currentPathWeight, int depth,
//                 word_t *frameToEvict, uint64_t *pageToEvict, word_t *parentMax, int *maxPathWeight) {
//    if (depth == TABLES_DEPTH) {
//        if (currentPathWeight > *maxPathWeight) {
//            *maxPathWeight = currentPathWeight;
//            *parentMax = currentParent;
//            *frameToEvict = currentFrame;
//            *pageToEvict = currentPage;
//        }
//        return;
//    }
//    // not a leaf, run on all sons
//    word_t currentRow = 0;
//    for (int i = 0; i < PAGE_SIZE; i++) {
//        PMread(((uint64_t )currentFrame)*PAGE_SIZE + i, &currentRow);
//        int weight = 1;
//        if ( currentRow % 2 == 0) {
//            weight = 1;
//        }
//        if (currentRow != 0) {
//            findMaxPath(currentRow, (currentPage << OFFSET_WIDTH) + i, currentFrame * PAGE_SIZE + i, currentPathWeight + weight, depth + 1, frameToEvict,
//                        pageToEvict, parentMax, maxPathWeight);
//        }
//    }
//}
//
//
//void evictAndReplace(word_t *frameIndex) {
//    word_t parent = 0;
//    int maxPathWeight = 0;
//    uint64_t pageToEvict = 0;
//    findMaxPath(0, 0, -1, 0, 0, frameIndex, &pageToEvict, &parent, &maxPathWeight);
//    PMevict(*frameIndex, pageToEvict);
//    PMwrite(parent, 0);
//}
//
//
//uint64_t takeOffset(uint64_t *address, bool firstTime) {
//    int offsetWidth = OFFSET_WIDTH;
//    if (firstTime && ((VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH) != 0)){
//        offsetWidth = VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH;
//    }
//    uint64_t offset = *address >> (VIRTUAL_ADDRESS_WIDTH - offsetWidth);
//    *address <<= offsetWidth;
//    uint64_t shifter = 1 << VIRTUAL_ADDRESS_WIDTH;
//    *address &= shifter - 1;
//    return offset;
//}
//
//
//void VMreadWrite(uint64_t virtualAddress, uint64_t *offset, word_t *addr_curr, word_t *addr_future){
//    uint64_t table_depth = TABLES_DEPTH;
//    uint64_t originalVirtualAddress = virtualAddress;
//    bool firstTime = false;
//    for (uint64_t i = 0; i < table_depth; i++) {
//        if (i == 0){
//            firstTime = true;
//        }
//        *offset = takeOffset(&virtualAddress, firstTime);
//        PMread(((uint64_t )(*addr_curr)) * PAGE_SIZE + *offset, addr_future);
//        if (*addr_future == 0){
//            findAvailableFrame(addr_future, *addr_curr); // this returns a frame index
//            clearTable((uint64_t)*addr_future);
//            if (i == table_depth - 1) {
//                PMrestore((uint64_t )(*addr_future), (originalVirtualAddress >> OFFSET_WIDTH));
//            }
//            PMwrite(((uint64_t )(*addr_curr)) * PAGE_SIZE + *offset, *addr_future);
//        }
//        *addr_curr = *addr_future;
//        firstTime = false;
//    }
//    *offset = takeOffset(&virtualAddress, false);
//}
//
//
//int VMread(uint64_t virtualAddress, word_t* value) {
//    if (virtualAddress >= VIRTUAL_MEMORY_SIZE){
//        return 0;
//    }
//    uint64_t offset = 0;
//    word_t addr_curr = 0;
//    word_t addr_future = 0;
//    VMreadWrite(virtualAddress, &offset, &addr_curr, &addr_future);
//    PMread(((uint64_t )addr_curr) * PAGE_SIZE + offset, value);
//    return 1;
//}
//
//
//int VMwrite(uint64_t virtualAddress, word_t value) {
//    if (virtualAddress >= VIRTUAL_MEMORY_SIZE){
//        return 0;
//    }
//    uint64_t offset = 0;
//    word_t addr_curr = 0;
//    word_t addr_future;
//    VMreadWrite(virtualAddress, &offset, &addr_curr, &addr_future);
//    PMwrite(addr_curr * PAGE_SIZE + offset, value);
//    return 1;
//}