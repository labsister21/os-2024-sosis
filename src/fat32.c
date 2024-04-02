#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/driver/fat32.h"

static struct FAT32DriverState fat32_driver_state;

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

void create_fat32(void)
{
    uint8_t boot_sector[BLOCK_SIZE];
    memset(boot_sector, 0, BLOCK_SIZE);
    memcpy(boot_sector, fs_signature, BLOCK_SIZE);
    write_blocks(boot_sector, 0, 1);

    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fat32_driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);

    struct FAT32DirectoryTable root_dir_table = {0};
    init_directory_table(&root_dir_table, "ROOT\0\0\0", 2);
    write_clusters(&root_dir_table, 2, 1);
}

bool is_empty_storage(void)
{
    uint8_t boot_sector[BLOCK_SIZE];
    read_blocks(boot_sector, 0, 1);
    return memcmp(boot_sector, fs_signature, BLOCK_SIZE);
}

void initialize_filesystem_fat32(void)
{
    if (is_empty_storage())
    {
        create_fat32();
    }
    else
    {
        read_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
    }
}

int8_t read(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int idx = -1;
    for (uint32_t i = 0; i < 64; i++){
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0){
            idx = i;
            break;
        }
    }

    if (idx == -1){
        return 3;
    }

    if (request.buffer_size < fat32_driver_state.dir_table_buf.table[idx].filesize){
        return 2;
    }
    else if (fat32_driver_state.dir_table_buf.table[idx].attribute == 1 || request.buffer_size == 0){
        return 1;
    }
    else{
        uint32_t cluster_count = fat32_driver_state.dir_table_buf.table[idx].filesize / CLUSTER_SIZE;
        if (cluster_count * CLUSTER_SIZE < fat32_driver_state.dir_table_buf.table[idx].filesize){
            cluster_count++;
        }
        uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[idx].cluster_low;
        for (uint32_t j = 0 ; j < cluster_count; j++){
            if (j == 0){
                read_clusters(request.buf, cluster_number, 1);
                cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
            }
            else{
                read_clusters(request.buf + (j * CLUSTER_SIZE), cluster_number, 1);
                cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
            }
        }
        return 0;
    }
    return -1;
}

int8_t read_directory(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int idx = -1;
    for (uint32_t i = 0; i < 64; i++){
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0){
            idx = i;
            break;
        }
    }

    if (idx == -1){
        return 3;
    }

    if (request.buffer_size < fat32_driver_state.dir_table_buf.table[idx].filesize){
        return 2;
    }
    else if (fat32_driver_state.dir_table_buf.table[idx].attribute == 1){
        return 1;
    }
    else{
        uint32_t entry = fat32_driver_state.dir_table_buf.table[idx].cluster_low | fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16;
        read_clusters(request.buf,entry, 1);
        return 0;
    }
    return -1;
}

int8_t write(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int index = 0;
    while (fat32_driver_state.fat_table.cluster_map[index] != 0){
        index++;
    }
    if (is_dir_table_valid()){
        for (uint32_t i = 0; i < 64; i++){
            if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0){
                return 1;
            }
        }

        if (request.buffer_size == 0){
            struct FAT32DirectoryEntry new_entry = {
                .attribute = ATTR_SUBDIRECTORY,
                .user_attribute = UATTR_NOT_EMPTY,
                .filesize = 0,
                .cluster_high = (index >> 16) & 0xFFFF,
                .cluster_low = index & 0xFFFF,
            };

            for (int i = 0; i < 8; i++){
                new_entry.name[i] = request.name[i];
            }
            int dirindex = 0;
            while (fat32_driver_state.dir_table_buf.table[dirindex].user_attribute == UATTR_NOT_EMPTY){
                dirindex++;
            }
            fat32_driver_state.dir_table_buf.table[dirindex] = new_entry;
            struct FAT32DirectoryTable temp = {
                .table = {
                            {
                                .attribute = ATTR_SUBDIRECTORY,
                                .user_attribute = UATTR_NOT_EMPTY,
                                .filesize = request.buffer_size,
                                .cluster_high = request.parent_cluster_number >> 16 ,
                                .cluster_low = request.parent_cluster_number & 0xFFFF,
                            }
                        }
            };
            for (int i = 0; i < 8; i++){
                temp.table[0].name[i] = request.name[i];
            }
            fat32_driver_state.fat_table.cluster_map[index] = FAT32_FAT_END_OF_FILE;
            write_clusters(&temp, index, 1);
            write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
            write_clusters(&fat32_driver_state.fat_table, 1, 1);
            return 0;
        }
        else{
            int dirindex = 0;
            while (fat32_driver_state.dir_table_buf.table[dirindex].user_attribute == UATTR_NOT_EMPTY)
            {
                dirindex++;
            }
            int tempindex = index;
            int nextIndex = index;
            for (int i = 0; i < ceil(request.buffer_size / CLUSTER_SIZE); i++)
            {
                while (fat32_driver_state.fat_table.cluster_map[tempindex] != 0){
                    tempindex++;
                }
                if (i == 0){
                    struct FAT32DirectoryEntry new_entry = {
                    .user_attribute = UATTR_NOT_EMPTY,
                    .filesize = request.buffer_size,
                    .cluster_high = (tempindex >> 16) & 0xFFFF,
                    .cluster_low = tempindex & 0xFFFF,
                    };

                    for (int i = 0; i < 8; i++){
                        new_entry.name[i] = request.name[i];
                    }
                    for (int i = 0 ; i < 3 ; i ++){
                        new_entry.ext[i] = request.ext[i];
                    }
                    fat32_driver_state.dir_table_buf.table[dirindex] = new_entry;
                    write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
                }
                nextIndex = tempindex+1;
                while (fat32_driver_state.fat_table.cluster_map[nextIndex] != 0){
                    nextIndex++;
                }
                if (i == ceil(request.buffer_size / CLUSTER_SIZE - 1)){
                    fat32_driver_state.fat_table.cluster_map[tempindex] = FAT32_FAT_END_OF_FILE;
                }
                else{
                    fat32_driver_state.fat_table.cluster_map[tempindex] = nextIndex;
                }
                write_clusters(request.buf + i * CLUSTER_SIZE, tempindex, 1);
                write_clusters(&fat32_driver_state.fat_table, 1, 1);
            }
            return 0;
        }
    }
    return -1;
}

int8_t delete(struct FAT32DriverRequest request)
{
    if (request.parent_cluster_number == 0){
        return 2;
    }
    else{
        read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
        uint16_t hi, lo, idxDir;
        uint32_t found = 0;
        for(uint32_t i = 0; i < (CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)); i++){
            if( memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 ){
                hi = fat32_driver_state.dir_table_buf.table[i].cluster_high;
                lo = fat32_driver_state.dir_table_buf.table[i].cluster_low;
                idxDir = i;
                found = 1;
                break;
            }
        }

        if(found == 0){
            return 1;
        }

        struct FAT32DriverState check;
        read_clusters(&check.dir_table_buf, idxDir, 1);

        int counter = 0;
        for(int i = 0 ; i < 64; i++){
            if(check.dir_table_buf.table[i].user_attribute == UATTR_NOT_EMPTY){
                counter++;
            }
        }

        if(counter <= 1){
            uint32_t idx = hi << 16 | lo;

            if(request.buffer_size == 0){
                fat32_driver_state.fat_table.cluster_map[idx] = 0;
                write_clusters(&fat32_driver_state.fat_table, 1, 1);
            }

            else{
                while(fat32_driver_state.fat_table.cluster_map[idx] != FAT32_FAT_END_OF_FILE){
                    uint32_t temp = idx;
                    idx = fat32_driver_state.fat_table.cluster_map[idx];
                    fat32_driver_state.fat_table.cluster_map[temp] = 0;
                }
                fat32_driver_state.fat_table.cluster_map[idx] = 0;
                write_clusters(&fat32_driver_state.fat_table, 1, 1);
            }

            struct FAT32DirectoryTable new;
            for (uint32_t i = 0; i < (CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)); i++){
                if(i != idxDir){
                    memcpy(&new.table[i],&fat32_driver_state.dir_table_buf.table[i], sizeof(fat32_driver_state.dir_table_buf.table[i]));
                }
            }

            memcpy(&fat32_driver_state.dir_table_buf, &new, sizeof(new));
            write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
            return 0;
        }
        else{
            return 2;
        }
    }
    return -1;
}