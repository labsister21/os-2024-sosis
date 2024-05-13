#include "header/process/process.h"
#include "header/memory/paging.h"
#include "header/stdlib/string.h"
#include "header/cpu/gdt.h"

struct ProcessControlBlock _process_list[PROCESS_COUNT_MAX];

static struct{
    int active_process_count;
    struct ProcessControlBlock* current_running;
} process_manager_state = {
    .active_process_count = 0,
    .current_running = NULL,
};

bool release_memory(struct ProcessControlBlock *pcb) {
    for (uint32_t i = 0; i < pcb->memory.page_frame_used_count; i++) {
        void *addr = pcb->memory.virtual_addr_used[i];
        free(addr);
        pcb->memory.virtual_addr_used[i] = NULL;
    }

    pcb->memory.page_frame_used_count = 0;

    return true;
}

int32_t process_create_user_process(struct FAT32DriverRequest request) {
    int32_t retcode = PROCESS_CREATE_SUCCESS; 
    if (process_manager_state.active_process_count >= PROCESS_COUNT_MAX) {
        retcode = PROCESS_CREATE_FAIL_MAX_PROCESS_EXCEEDED;
        goto exit_cleanup;
    }

    // Ensure entrypoint is not located at kernel's section at higher half
    if ((uint32_t) request.buf >= KERNEL_VIRTUAL_ADDRESS_BASE) {
        retcode = PROCESS_CREATE_FAIL_INVALID_ENTRYPOINT;
        goto exit_cleanup;
    }

    // Check whether memory is enough for the executable and additional frame for user stack
    uint32_t page_frame_count_needed = ceil_div(request.buffer_size + PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
    if (!paging_allocate_check(page_frame_count_needed) || page_frame_count_needed > PROCESS_PAGE_FRAME_COUNT_MAX) {
        retcode = PROCESS_CREATE_FAIL_NOT_ENOUGH_MEMORY;
        goto exit_cleanup;
    }

    // Process PCB 
    int32_t p_index = process_list_get_inactive_index();
    struct ProcessControlBlock *new_pcb = &(_process_list[p_index]);

    struct PageDirectory* cur_active = paging_get_current_page_directory_addr();
    new_pcb->context.page_directory_virtual_addr = paging_create_new_page_directory();
    paging_use_page_directory(new_pcb->context.page_directory_virtual_addr);

    for(uint32_t i=0;i<page_frame_count_needed;i++){
        new_pcb->memory.virtual_addr_used[i] = request.buf+i*PAGE_FRAME_SIZE;
    }
    new_pcb->memory.page_frame_used_count = page_frame_count_needed;

    paging_allocate_user_page_frame(new_pcb->context.page_directory_virtual_addr,request.buf);
    read(request);
    paging_use_page_directory(cur_active);

    new_pcb->context.eflags |= CPU_EFLAGS_BASE_FLAG | CPU_EFLAGS_FLAG_INTERRUPT_ENABLE;
    new_pcb->context.cpu.segment.ds = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.es = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.fs = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.gs = GDT_USER_DATA_SEGMENT_SELECTOR;

    new_pcb->metadata.pid = process_generate_new_pid();
    new_pcb->metadata.active = true;
    new_pcb->metadata.cur_state = READY;
exit_cleanup:
    return retcode;
}

struct ProcessControlBlock* process_get_current_running_pcb_pointer(void){
    return process_manager_state.current_running;
}

/**
 * Destroy process then release page directory and process control block
 * 
 * @param pid Process ID to delete
 * @return    True if process destruction success
 */
bool process_destroy(uint32_t pid) {
    for (int i = 0; i < PROCESS_COUNT_MAX; i++)
    {
        if (_process_list[i].metadata.pid == pid)
        {
            // release resource;
            if (!release_memory(&_process_list[i])) {
                printf("Fail release memory id: %d\n", pid);
                return false;
            }

            // release pcb;
            _process_list[i].metadata.pid = 0;
            _process_list[i].metadata.active = false;

            return true;
        }
    }
    
    return false;
}