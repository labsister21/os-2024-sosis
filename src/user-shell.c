#include <stdint.h>
#include "header/filesystem/fat32.h"

int strlen2(const char *str)
{
    int len = 0;
    while (*str != '\0')
    {
        len++;
        str++;
    }
    return len;
}

int strcmp2(const char *str1, const char *str2) {
    uint16_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i])
            return 0;
        i++;
    }
    if (str1[i] != str2[i])
        return 0;
    return 1;
}

int strncmp2(const char *str1, const char *str2, uint16_t n) {
    uint16_t i = 0;
    while (i < n && str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i])
            return 0;
        i++;
    }
    if (i < n && str1[i] != str2[i])
        return 0;
    return 1;
}

char *strtok2(char *str, const char *delim) {
    static char *last_token = NULL;
    char *tmp;
    const char *tmp_delim;

    if (str != NULL) {
        last_token = str;
    }

    if (last_token == NULL) {
        return NULL;
    }

    char* ret;
    tmp_delim = delim;
    while (*tmp_delim != '\0') {
        tmp = last_token;
        while (*tmp != '\0') {
            if (*tmp == *tmp_delim) {
                *tmp = '\0';
                ret = last_token;
                last_token = tmp + 1;
                return ret;
            }
            tmp++;
        }
        tmp_delim++;
    }

    tmp = last_token;
    last_token = NULL;
    return tmp;
}

void addTrailingNull2(char *str, uint16_t start, uint16_t end) {
    for (uint16_t i = start; i < end; i++)
        str[i] = '\0';
}

int parseFileName2(char *filename, char *name, char *ext) {
    int i = 0;
    while (i < 8 && filename[i] != '.' && filename[i] != '\0'){
        name[i] = filename[i];
        i++;
    }
    addTrailingNull2(name, i, 8);

    if (filename[i] == '\0') {
        addTrailingNull2(ext, 0, 3);
        return 0;
    }

    if (filename[i] != '.')
        return 1;
    
    int fnlen = i;
    i++;
    while (i <= fnlen + 3 && filename[i] != '\0'){
        ext[i-fnlen-1] = filename[i];
        i++;
    }
    addTrailingNull2(ext, i - fnlen - 1, 3);

    if (filename[i] != '\0')
        return 1;

    return 0;
}

int countWords2(const char* str) {
    int ctr = 0;
    for (int i = 0; i<strlen2(str)-1; i++) {
        if (i == 0 && str[i] != ' ') {
            ctr++;
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
        }
    }
    return ctr;
}

int wordLen2(const char* str, uint16_t idx) {
    int ctr = 0;
    int word_start = -1;
    int word_end = -1;

    for (int i = 0; i<strlen2(str)-1; i++) {
        if (word_start != -1 && word_end == -1 && str[i] == ' ') {
            word_end = i;
        }
        if (i == 0 && str[i] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i;
            }
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i + 1;
            }
        }
    }

    if (word_end == -1)
        word_end = strlen2(str);

    uint16_t n = word_end - word_start;
    return n;
}

void getWord2(const char* str, uint16_t idx, char* buf) {
    int ctr = 0;
    int word_start = -1;

    for (int i = 0; i<strlen2(str)-1; i++) {
        if (i == 0 && str[i] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i;
            }
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i + 1;
            }
        }
    }
    
    uint16_t n = wordLen2(str, idx);
    for (int i = 0; i < n; i++) {
        buf[i] = str[word_start + i];
    }
    buf[n] = '\0';
}

char* custom_strtok2(char* str, const char* delim) {
    static char* saved_str = NULL;
    char* token;

    if (str != NULL) {
        saved_str = str;
    }

    token = saved_str;
    while (*saved_str != '\0') {
        const char* d = delim;
        while (*d != '\0') {
            if (*saved_str == *d) {
                *saved_str = '\0';
                saved_str++;
                if (*token != '\0') {
                    return token;
                } else {
                    token = saved_str;
                    break;
                }
            }
            d++;
        }
        saved_str++;
    }

    if (*token == '\0') {
        return NULL;
    } else {
        saved_str = NULL;
        return token;
    }
}

void* memcpy2(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;  
}

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void puts(char* val, uint32_t color)
{
    syscall(6, (uint32_t) val, strlen2(val), color);
}

// MAIN COMMANDS
void remove(char* name, char* ext, uint32_t parent_number)
{
    int8_t ret;

    struct ClusterBuffer data_buf;
    struct FAT32DriverRequest request = {
        .buf = &data_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .buffer_size = 0,
        .parent_cluster_number = parent_number
    };

    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);
    syscall(3, (uint32_t)&request, (uint32_t)&ret, 0);

    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);
    request.parent_cluster_number = parent_number;
    syscall(3, (uint32_t)&request, (uint32_t)&ret, 0);
}

void rm(char* command) {
    uint16_t n_words = countWords2(command);
    int16_t recursive = -1;
    int8_t retcode = 0;

    struct FAT32DirectoryTable table_buf = {0};
    struct FAT32DriverRequest request = {
        .buf = &table_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .parent_cluster_number = 2, // could be a variable
        .buffer_size = sizeof(struct FAT32DirectoryTable)
    };
    
    for (uint16_t i = 1; i < n_words; i++) {
        uint16_t n = wordLen2(command, i);
        char word[n + 1];
        getWord2(command, i, word);
        if (strcmp2(word, "-r"))
            recursive = i;
    }

    if ((recursive == -1 && n_words < 2) || (recursive != -1 && n_words < 3)) {
        puts(": missing file operands\n", 0x07);
        return;
    }

    // check if all files exists
    for (uint16_t i = 1; i < n_words; i++) {
        if (recursive == i) continue;
        char filename[12];
        getWord2(command, i, filename);

        char name[9];
        char ext[4];

        // if filename is too long
        if (parseFileName2(filename, name, ext)) {
            puts(filename, 0x07);
            puts(": filename invalid, name or extension may be too long\n", 0x07);
            return;
        }

        memcpy2(request.name, name, 8);
        memcpy2(request.ext, ext, 3);
        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);
        if (retcode == 2) {
            puts(filename, 0x07);
            puts(": file not found\n", 0x07);
            return;
        }
        if (retcode == 0 && recursive == -1) {
            puts(filename, 0x07);
            puts(": is a directory;  -r not specified\n", 0x07);
            return;
        }
    }

    for (uint16_t i = 1; i < n_words; i++) {
        if (recursive == i) continue;
        char filename[12];
        getWord2(command, i, filename);

        char name[9];
        char ext[4];

        // if filename is too long
        if (parseFileName2(filename, name, ext)) {
            puts(filename, 0x07);
            puts(": filename invalid, name or extension may be too long\n", 0x07);
            return;
        }

        remove(name, ext, 2); // 2 could be a variable
    }
}


// MAIN
int main(void) {
    char dir[100] = "Root/";
    char name[100] = "\ns0sis@OS-IF2230:";
    char command[100] = "";
    int idx = 0;
    char *buf = '\0';

    while(true){
        // char *buf = NULL;
        syscall(7,0,0,0);
        puts(name, 0xA);
        puts(dir, 0x9);
        // puts("hi", 0xF);
        puts("$ ", 0xF);
        // syscall(6,(uint32_t)dir,10,0xF);
        // commandLI(dir);
        syscall(4, (uint32_t) command, 256, 0);

        while(*buf!='\n'){
            syscall(4,(uint32_t)buf,0,0);
            if(*buf!='\0' && *buf!='\n'){
                command[idx]=*buf;
                idx++;
                syscall(5,(uint32_t)buf,0xF,0);
            }
            // if(*buf!='\n'){
            //     command[idx]=*buf;
            //     idx++;
            //     syscall(5,(uint32_t)buf,0xF,0);
            // }
        }

        // syscall(6,(uint32_t)command,idx,0xF);
        // uint32_t n = wordLen2(command, 0);
        // char cmdtyped[n + 1];
        char *cmdtyped;
        cmdtyped = '\0';
        getWord2(command, 0, cmdtyped);
        puts("\n", 2);

        // puts(command, 0x07);

        // puts(cmdtyped, 0x07);


        // DO STUFF
        if (strcmp2(cmdtyped, "rm")) {
            rm(command);
        }
        else {
            puts(cmdtyped, 0x07);
            puts(": command not found\n", 0x07);
        }   

        idx = 0;
        *buf='\0';
    }

    syscall(5,(uint32_t)command[0],0xF,0);


    return 0;
}