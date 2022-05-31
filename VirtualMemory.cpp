//
// Created by avigayil on 17/05/2022.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream> //TODO: delete it

#define ROOT 0

int NUM = 0; //TODO: delete it
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

void traversingTree(uint64_t virtualAddress, word_t *addr);

word_t findFreeFrame(uint64_t virtualAddress, const word_t *frameToNotEvict); //TODO: need to change word_t

void restorePage(word_t frameIndex, uint64_t virtualAddress);

bool isFrameContainsOnlyZeros(uint64_t frame);

word_t min(word_t number1, word_t number2);


word_t
treeDFS(word_t root, word_t depth, word_t pageToSwapIn, word_t frameToNotEvict, word_t *maxFrame, word_t *FrameToEvict,
        uint64_t *pageToEvict, word_t *parentOfPageToEvict, word_t *parentOfEmptyTable, uint64_t currAddress);

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
    printTree(); //TODO: delete it
    return 1;
}

uint64_t findFrameOfPage(uint64_t pageNumber) {

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
                    word_t *addr) { //TODO: need to check if all tables suppose to be with the same size
//    uint64_t pSize = (VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / TABLES_DEPTH; //TODO: don't know if the size is accurate

    uint64_t pSize = OFFSET_WIDTH; //TODO: not sure yet
    uint64_t pOnes = (1LL << pSize) - 1;
    uint64_t mask = pOnes << (VIRTUAL_ADDRESS_WIDTH - pSize); //ones with the size of pSize and after that zeros
    word_t pointerFrame = 0;
    word_t parentFrame;
    uint64_t currAddress;
    for (uint64_t i = 0; i < TABLES_DEPTH; i++) {
        uint64_t zerosToAvoid = ((TABLES_DEPTH - i) * pSize);
        uint64_t p = (mask & virtualAddress) >> zerosToAvoid;
        uint64_t frameAddress = (pointerFrame * PAGE_SIZE);
        currAddress = frameAddress + p;
        parentFrame = pointerFrame;
        PMread(currAddress, &pointerFrame);
        if (pointerFrame == 0) {
            word_t freeFrame = findFreeFrame(virtualAddress, &parentFrame);
            uint64_t freeFrameAddress = freeFrame * PAGE_SIZE;
            if (i < TABLES_DEPTH - 1) {
                fillFrameWithValue(freeFrameAddress,
                                   0);
            } else {
                try {//TODO:delete it
                    restorePage(freeFrame, virtualAddress);
                }
                catch(std::exception &e){
                    std::cout<<"problem with PMRESTORE"<<std::endl;
                }
            }
            PMwrite(currAddress, freeFrame);
            pointerFrame = freeFrame;
        }

        mask = mask >> pSize;
    }
    uint64_t dOnes = (1LL << OFFSET_WIDTH) - 1;
    uint64_t d = virtualAddress & dOnes;
    *addr = pointerFrame * PAGE_SIZE + d;

}

word_t
treeDFS(word_t root, word_t depth, word_t pageToSwapIn, word_t frameToNotEvict, word_t *maxFrame, word_t *FrameToEvict,
        uint64_t *pageToEvict, word_t *parentOfPageToEvict, word_t *parentOfEmptyTable, uint64_t currAddress) {

    //doing DFS while at the same time check all parameters for frame

    if (isFrameContainsOnlyZeros(root)) {
        if (root != frameToNotEvict && (depth < TABLES_DEPTH))
            return root;
        return 0; //means there are not available frames with zeros in this sub-tree
    }
    if (depth < TABLES_DEPTH) {  //TODO: I'm not sure about this condition
        for (word_t i = 0; i < PAGE_SIZE; i++) { // iterate over the root's sons
            word_t rowValue = 0;
            uint64_t frameAddress = root * PAGE_SIZE;
            PMread(frameAddress + i, &rowValue);
            if (rowValue != 0) {
                if (rowValue > *maxFrame) {
                    *maxFrame = rowValue;
                }
                if (depth == TABLES_DEPTH - 1) { // means it is a leaf
                    if (*FrameToEvict == -1) { //means we didn't fill it
                        *FrameToEvict = rowValue;
                        *parentOfPageToEvict = root;
                        *pageToEvict = (currAddress <<1) + i;
                    } else {
                        uint64_t leafSon = (currAddress <<1) + i;
                        word_t distance1 = min((pageToSwapIn - leafSon),
                                               (NUM_PAGES - (pageToSwapIn - leafSon))); //TODO: check long long
                        word_t distance2 = min((pageToSwapIn - *pageToEvict),
                                               (NUM_PAGES -
                                                (pageToSwapIn - *pageToEvict))); //TODO: check long long
                        if (distance1 > distance2) {
                            *pageToEvict = (currAddress<<1) + i;
                            *FrameToEvict = findFrameOfPage(*pageToEvict);
                            *parentOfPageToEvict = root;
                        }
                    }
                }
                *parentOfEmptyTable = root;
                word_t result = treeDFS(rowValue, depth + 1, pageToSwapIn, frameToNotEvict, maxFrame, FrameToEvict,
                                        pageToEvict,
                                        parentOfPageToEvict, parentOfEmptyTable, (currAddress << 1) + i);
                if (result != 0) {
                    return result;
                }
            }
        }
        return 0;
    }
    return 0;
}


word_t findFreeFrame(uint64_t virtualAddress, const word_t *frameToNotEvict) {

    if (isFrameContainsOnlyZeros(ROOT)) {
        return 1;
    }
    word_t pageNum = virtualAddress >> OFFSET_WIDTH;
    word_t maxFrame = 0;
    word_t frameToEvict = -1;
    word_t numOfParentOfPageToEvict = -1;
    word_t numOfParentOfEmptyTable = -1;
    uint64_t pageToEvict = 0;
    //is settled.
    word_t frameOfZeros = treeDFS(0, 0, pageNum, *frameToNotEvict, &maxFrame, &frameToEvict, &pageToEvict,
                                  &numOfParentOfPageToEvict, &numOfParentOfEmptyTable, 0);


    if (frameOfZeros) {
        if (maxFrame == NUM_FRAMES - 1) { //which means every frame in used-
            word_t pointer; //delete the parent
            for (uint64_t i = 0; i < PAGE_SIZE; i++) {
                PMread(i + numOfParentOfEmptyTable * PAGE_SIZE, &pointer);
                if (pointer == frameOfZeros) {
                    PMwrite(i + numOfParentOfEmptyTable * PAGE_SIZE, 0);
                }
            }
        }
        return frameOfZeros;
    }
    if (maxFrame < NUM_FRAMES - 1) {
        return maxFrame + 1;
    }
    //third case - need to evict page
    try{ //TODO: delete it
    PMevict(frameToEvict, pageToEvict);}
    catch(std::exception &e){
        std::cout<<"problem with PMEVICT"<<std::endl;
    }
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

word_t min(word_t number1, word_t number2) {
    if (number1 >= number2) {
        return number2;
    }
    return number1;
}


//TODO: word_t vs unit_64???