#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/memory/paging.h"

__attribute__((aligned(0x1000))) struct PageDirectory _paging_kernel_page_directory = {
    .table = {
        [0] = {
            .flag.present_bit       = 1,
            .flag.write_bit         = 1,
            .flag.use_pagesize_4_mb = 1,
            .lower_address          = 0,
        },
        [0x300] = {
            .flag.present_bit       = 1,
            .flag.write_bit         = 1,
            .flag.use_pagesize_4_mb = 1,
            .lower_address          = 0,
        },
    }
};

static struct PageManagerState page_manager_state = {
    .page_frame_map = {
        [0]                            = true,
        [1 ... PAGE_FRAME_MAX_COUNT-1] = false
    },
    .free_page_frame_count = PAGE_FRAME_MAX_COUNT-1,
    .next_free_frame = 1,
};

void update_page_directory_entry(
    struct PageDirectory *page_dir,
    void *physical_addr, 
    void *virtual_addr, 
    struct PageDirectoryEntryFlag flag
) {
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF;
    page_dir->table[page_index].flag          = flag;
    page_dir->table[page_index].lower_address = ((uint32_t) physical_addr >> 22) & 0x3FF;
    flush_single_tlb(virtual_addr);
}

void flush_single_tlb(void *virtual_addr) {
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr): "memory");
}



/* --- Memory Management --- */
// TODO: Implement
bool paging_allocate_check(uint32_t amount) {
    uint32_t req_page = amount / PAGE_FRAME_SIZE;
    if(amount%PAGE_FRAME_SIZE!=0){
        req_page++;
    }
    return req_page<=page_manager_state.free_page_frame_count;
}


bool paging_allocate_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr) {
    /**
     * TODO: Find free physical frame and map virtual frame into it
     * - Find free physical frame in page_manager_state.page_frame_map[] using any strategies
     * - Mark page_manager_state.page_frame_map[]
     * - Update page directory with user flags:
     *     > present bit    true
     *     > write bit      true
     *     > user bit       true
     *     > pagesize 4 mb  true
     */ 
    if(page_manager_state.free_page_frame_count==0){
        return false;
    }

    int phy_mem_free = page_manager_state.next_free_frame;
    uint32_t phy_addr_free = PAGE_FRAME_SIZE*(phy_mem_free+1);
    page_manager_state.free_page_frame_count--;
    page_manager_state.page_frame_map[phy_mem_free] = true;
    
    for(int i=phy_mem_free+1;i<PAGE_FRAME_MAX_COUNT;i++){
        if(page_manager_state.page_frame_map[i]==false){
            page_manager_state.next_free_frame = i;
            break;
        }
    }
    
    struct PageDirectoryEntryFlag flag = {
        .present_bit = 1,
        .write_bit = 1,
        .user_bit = 1,
        .use_pagesize_4_mb = 1,
    };
    update_page_directory_entry(
        page_dir,
        (void*)&phy_addr_free,
        (void*)&virtual_addr,
        flag
    );
    return true;
}

bool paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr) {
    /* 
     * TODO: Deallocate a physical frame from respective virtual address
     * - Use the page_dir.table values to check mapped physical frame
     * - Remove the entry by setting it into 0
     */
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF;
    uint16_t phy_mapped = page_dir->table[page_index].lower_address;
    if(!page_manager_state.page_frame_map[phy_mapped]){
        return false;
    }

    page_dir->table[page_index].lower_address = 0;
    page_manager_state.free_page_frame_count++;
    page_manager_state.page_frame_map[phy_mapped] = false;
    if(page_manager_state.next_free_frame>phy_mapped){
        page_manager_state.next_free_frame = phy_mapped;
    }

    return true;
}