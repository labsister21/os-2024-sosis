#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/driver/fat32.h"

struct FAT32DriverState fat32_driver_state;

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

uint32_t cluster_to_lba(uint32_t cluster){
    return cluster*CLUSTER_BLOCK_COUNT;
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    read_blocks(ptr, cluster_number * CLUSTER_BLOCK_COUNT, cluster_count * CLUSTER_BLOCK_COUNT);
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_number * CLUSTER_BLOCK_COUNT, cluster_count * CLUSTER_BLOCK_COUNT);
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    for (int i = 0; i < 8; i++){
        dir_table->table[0].name[i] = name[i];
    }
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[1].user_attribute = UATTR_NOT_EMPTY;

    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[1].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);

    dir_table->table[0].filesize = 0;
    dir_table->table[1].filesize = 0;
    for(int i=2;i<CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry);i++){
        dir_table->table[i].user_attribute = FAT32_FAT_EMPTY_ENTRY;
    }
}

void create_fat32(void)
{
    // Write fs signature to boot sector
    write_blocks(fs_signature, 0, 1);

    // Initialize cluster 0,1,2 in FAT
    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fat32_driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    for(int i=3;i<CLUSTER_MAP_SIZE;i++){
        // Inisialisasi Kosong
        fat32_driver_state.fat_table.cluster_map[i] = 0;
    }

    struct FAT32DirectoryTable root_dir = {0};
    init_directory_table(&root_dir, "Root\0\0\0\0", ROOT_CLUSTER_NUMBER);
    root_dir.table[0].cluster_high = 0;
    root_dir.table[0].cluster_low = ROOT_CLUSTER_NUMBER;

    write_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    write_clusters(&root_dir, ROOT_CLUSTER_NUMBER, 1);
}

bool is_empty_storage(void){
    struct BlockBuffer boot_sector;
    read_blocks(boot_sector.buf, BOOT_SECTOR, 1);
    return memcmp(boot_sector.buf, fs_signature, BLOCK_SIZE);
}

void initialize_filesystem_fat32(void)
{
    if (is_empty_storage()){
        create_fat32();
    }else{
        read_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    }
}

int findEntry(struct FAT32DirectoryTable dir_table, char name[8],char ext[3]){
    int i = 2;
    while(i<CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)){
        if (memcmp(dir_table.table[i].name, name, 8) == 0 && memcmp(dir_table.table[i].ext, ext, 3) == 0){
            return i;
        }
        i++;
    }
    return -9999;
}

int8_t read(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
    int idx = findEntry(fat32_driver_state.dir_table_buf,request.name,request.ext);
    if (idx == -9999){
        return 3;
    }
    // Not enough buffer size
    if (request.buffer_size < fat32_driver_state.dir_table_buf.table[idx].filesize){
        return 2;
    }
    // Is a Folder, not a file
    if (fat32_driver_state.dir_table_buf.table[idx].attribute == ATTR_SUBDIRECTORY){
        return 1;
    }
    
    uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[idx].cluster_low;
    int j=0;
    while(cluster_number!=FAT32_FAT_END_OF_FILE){
        read_clusters(request.buf+(j*CLUSTER_SIZE),cluster_number,1);
        cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
        j++;
    }
    return 0;
}

int8_t read_directory(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int idx = findEntry(fat32_driver_state.dir_table_buf,request.name,request.ext);
    // Not Found
    if (idx==-9999){
        return 2;
    }
    // Is a file, not a folder
    if (fat32_driver_state.dir_table_buf.table[idx].attribute != ATTR_SUBDIRECTORY){
        return 1;
    }
    uint32_t entry = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16)|(fat32_driver_state.dir_table_buf.table[idx].cluster_low);
    read_clusters(request.buf,entry, 1);
    return 0;
}

int ceil(double x){
    int intPart = (int)x;
    return (x > intPart) ? (intPart + 1) : intPart;
}

int findEmptySpace(struct FAT32FileAllocationTable *fat){
    for(int i=0;i<CLUSTER_MAP_SIZE;i++){
        if(fat->cluster_map[i]==0){
            return i;
        }
    }
    return -9999;
}

int findIdxEmptyEntry(struct FAT32DirectoryTable dir){
    for(int i=2;i<CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry);i++){
        if(dir.table->user_attribute!=UATTR_NOT_EMPTY){
            return i;
        }
    }
    return -9999;
}

void addEntry(struct FAT32DirectoryTable parent_dir,struct FAT32DriverRequest request, int idxEmpty){
    memcpy(parent_dir.table[idxEmpty].name,request.name,8);
    memcpy(parent_dir.table[idxEmpty].ext,request.ext,3);
    parent_dir.table[idxEmpty].user_attribute = UATTR_NOT_EMPTY;
    if(request.buffer_size==0){
        parent_dir.table[idxEmpty].attribute = ATTR_SUBDIRECTORY;
    }
    parent_dir.table[idxEmpty].cluster_high = (uint16_t)(idxEmpty>>16);
    parent_dir.table[idxEmpty].cluster_high = (uint16_t)(idxEmpty & 0xFFFF);
    parent_dir.table[idxEmpty].filesize = request.buffer_size;
}

int8_t write(struct FAT32DriverRequest request){
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number,1);
    if(fat32_driver_state.dir_table_buf.table[0].attribute != ATTR_SUBDIRECTORY){
        return 2;
    }
    int idxFound = findEntry(fat32_driver_state.dir_table_buf,request.name,request.ext);
    if(idxFound!=-9999){
        return 1;
    }
    int idxEntryParent = findIdxEmptyEntry(fat32_driver_state.dir_table_buf);
    // Jika directory table parent udah penuh
    if(idxEntryParent==-9999){
        return -1;
    }
    int idx = findEmptySpace(&fat32_driver_state.fat_table);
    addEntry(fat32_driver_state.dir_table_buf,request,idx);
    write_clusters(&fat32_driver_state.dir_table_buf.table,request.parent_cluster_number,1);

    if(request.buffer_size==0){
        struct FAT32DirectoryTable new_dir;
        init_directory_table(&new_dir,request.name,request.parent_cluster_number);
        write_clusters(&new_dir,idx,1);

        fat32_driver_state.fat_table.cluster_map[idx] = FAT32_FAT_END_OF_FILE;
        write_clusters(&fat32_driver_state.fat_table.cluster_map,FAT_CLUSTER_NUMBER,1);
    }else{
        int total_cluster = ceil(request.buffer_size/CLUSTER_SIZE);
        int prev = -9999;
        for(int i=1;i<total_cluster;i++){
            prev = idx;
            idx = findEmptySpace(&fat32_driver_state);
            if(idx==-9999){
                fat32_driver_state.fat_table.cluster_map[prev] = FAT32_FAT_END_OF_FILE;
                return -1;
            }else{
                write_clusters(request.buf+((i-1)*CLUSTER_SIZE),idx,1);
                if(prev!=-9999){
                    fat32_driver_state.fat_table.cluster_map[prev] = idx;
                }
            }
        }
        fat32_driver_state.fat_table.cluster_map[idx] = FAT32_FAT_END_OF_FILE;
        write_clusters(&fat32_driver_state.fat_table.cluster_map,FAT_CLUSTER_NUMBER,1);
    }
    return 0;
}


int8_t delete(struct FAT32DriverRequest request){
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int idx = findEntry(fat32_driver_state.dir_table_buf,request.name,request.ext);
    if (idx == -9999){
        return 1;
    }
    if (fat32_driver_state.dir_table_buf.table[idx].attribute == ATTR_SUBDIRECTORY){
        int i = 2;
        int count = 0;
        
        uint32_t foundfolder = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | (fat32_driver_state.dir_table_buf.table[idx].cluster_low);
        read_cluster(&fat32_driver_state.dir_table_buf,foundfolder,1);

        while(i < 64){
            if (fat32_driver_state.dir_table_buf.table[i].user_attribute == UATTR_NOT_EMPTY){
                return 2;
            }
            i++;
        }
        
        read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
        fat32_driver_state.dir_table_buf.table[i].user_attribute = FAT32_FAT_EMPTY_ENTRY;
        fat32_driver_state.fat_table.cluster_map[foundfolder] = 0;
        write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
        return 0;
    }
    if (fat32_driver_state.dir_table_buf.table[idx].attribute != ATTR_SUBDIRECTORY){

        uint32_t foundfolder = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | (fat32_driver_state.dir_table_buf.table[idx].cluster_low);
        read_cluster(&fat32_driver_state.dir_table_buf,foundfolder,1);
        fat32_driver_state.dir_table_buf.table[idx].user_attribute = FAT32_FAT_EMPTY_ENTRY;

        int j=0;
        while(foundfolder!=FAT32_FAT_END_OF_FILE){
            fat32_driver_state.fat_table.cluster_map[foundfolder] = 0;
            foundfolder = fat32_driver_state.fat_table.cluster_map[foundfolder];
        }
        fat32_driver_state.fat_table.cluster_map[foundfolder] = 0;
        return 0;
    }
    return -1;

}
// int8_t deletes(struct FAT32DriverRequest request)
// {
//     if (request.parent_cluster_number == 0){
//         return 2;
//     }
//     else{
//         read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
//         uint16_t hi, lo, idxDir;
//         uint32_t found = 0;
//         for(uint32_t i = 0; i < (CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)); i++){
//             if( memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 ){
//                 hi = fat32_driver_state.dir_table_buf.table[i].cluster_high;
//                 lo = fat32_driver_state.dir_table_buf.table[i].cluster_low;
//                 idxDir = i;
//                 found = 1;
//                 break;
//             }
//         }

//         if(found == 0){
//             return 1;
//         }

//         struct FAT32DriverState check;
//         read_clusters(&check.dir_table_buf, idxDir, 1);

//         int counter = 0;
//         for(int i = 0 ; i < 64; i++){
//             if(check.dir_table_buf.table[i].user_attribute == UATTR_NOT_EMPTY){
//                 counter++;
//             }
//         }

//         if(counter <= 1){
//             uint32_t idx = hi << 16 | lo;

//             if(request.buffer_size == 0){
//                 fat32_driver_state.fat_table.cluster_map[idx] = 0;
//                 write_clusters(&fat32_driver_state.fat_table, 1, 1);
//             }

//             else{
//                 while(fat32_driver_state.fat_table.cluster_map[idx] != FAT32_FAT_END_OF_FILE){
//                     uint32_t temp = idx;
//                     idx = fat32_driver_state.fat_table.cluster_map[idx];
//                     fat32_driver_state.fat_table.cluster_map[temp] = 0;
//                 }
//                 fat32_driver_state.fat_table.cluster_map[idx] = 0;
//                 write_clusters(&fat32_driver_state.fat_table, 1, 1);
//             }

//             struct FAT32DirectoryTable new;
//             for (uint32_t i = 0; i < (CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)); i++){
//                 if(i != idxDir){
//                     memcpy(&new.table[i],&fat32_driver_state.dir_table_buf.table[i], sizeof(fat32_driver_state.dir_table_buf.table[i]));
//                 }
//             }

//             memcpy(&fat32_driver_state.dir_table_buf, &new, sizeof(new));
//             write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
//             return 0;
//         }
//         else{
//             return 2;
//         }
//     }
//     return -1;
// }