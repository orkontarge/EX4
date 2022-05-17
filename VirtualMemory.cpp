//
// Created by avigayil on 17/05/2022.
//



void VMinitialize(){
    /*
     * intialize with frame 0 as root, can initialize full tree based on constants sizes, initialize freeframelist range(NUM_FRAMES)
     * class of pagetable (node in tree) - hold the table, pages number: frame address, pointer to next table, validation
     * store each table in a frameindex.
    */

}



int VMread(uint64_t virtualAddress, word_t* value){
    /* frameIndex == 0 == PAGEFAULT -> swap in ->update tree
     * if virtualAddress in PhysicalAdrees -> if validation bit is True, if frameindex==0:
     * divide VirtaulAddress into secations, based on table size/width
     * PMread(0+p1,addr1)
     * (loop) for each p:
     *      PMread(add1*PAGE_SIZE+p, add1) check at each point if frameindex is zero.
     * PMread(...,value)
     * else:
     *
     */
}


// Find an unused frame or evict a page from some frame. return page or index