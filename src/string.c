#include "header/stdlib/string.h"

void* memset(void *s, int c, size_t n) {
    uint8_t *buf = (uint8_t*) s;
    for (size_t i = 0; i < n; i++)
        buf[i] = (uint8_t) c;
    return s;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *buf1 = (const uint8_t*) s1;
    const uint8_t *buf2 = (const uint8_t*) s2;
    for (size_t i = 0; i < n; i++) {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    if (dstbuf < srcbuf) {
        for (size_t i = 0; i < n; i++)
            dstbuf[i]   = srcbuf[i];
    } else {
        for (size_t i = n; i != 0; i--)
            dstbuf[i-1] = srcbuf[i-1];
    }

    return dest;
}

int strlen(const char *str) {
    int len = 0;
    while (str[len])
        len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    size_t i;
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

int strcmp(const char *str1, const char *str2) {
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

int strncmp(const char *str1, const char *str2, uint16_t n) {
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

char *strtok(char *str, const char *delim) {
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

void addTrailingNull(char *str, uint16_t start, uint16_t end) {
    for (uint16_t i = start; i < end; i++)
        str[i] = '\0';
}

int parseFileName(char *filename, char *name, char *ext) {
    int i = 0;
    while (i < 8 && filename[i] != '.' && filename[i] != '\0'){
        name[i] = filename[i];
        i++;
    }
    addTrailingNull(name, i, 8);

    if (filename[i] == '\0') {
        addTrailingNull(ext, 0, 3);
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
    addTrailingNull(ext, i - fnlen - 1, 3);

    if (filename[i] != '\0')
        return 1;

    return 0;
}

int countWords(const char* str) {
    int ctr = 0;
    for (int i = 0; i<strlen(str)-1; i++) {
        if (i == 0 && str[i] != ' ') {
            ctr++;
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
        }
    }
    return ctr;
}

int wordLen(const char* str, uint16_t idx) {
    int ctr = 0;
    int word_start = -1;
    int word_end = -1;

    for (int i = 0; i<strlen(str)-1; i++) {
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
        word_end = strlen(str);

    uint16_t n = word_end - word_start;
    return n;
}

void getWord(const char* str, uint16_t idx, char* buf) {
    int ctr = 0;
    int word_start = -1;

    for (int i = 0; i<strlen(str)-1; i++) {
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
    
    uint16_t n = wordLen(str, idx);
    for (int i = 0; i < n; i++) {
        buf[i] = str[word_start + i];
    }
    buf[n] = '\0';
}

char* custom_strtok(char* str, const char* delim) {
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

int string_to_int(const char *str) {
    int result = 0;
    int sign = 1; // Sign of the number, initially positive

    // Handle negative sign if present
    if (*str == '-') {
        sign = -1;
        str++; // Move to the next character
    }

    // Iterate through each character of the string
    while (*str != '\0') {
        // Check if the character is a digit
        if (*str >= '0' && *str <= '9') {
            // Convert the character to its integer value and add it to the result
            result = result * 10 + (*str - '0');
        } else {
            // If a non-digit character is encountered, stop conversion
            break;
        }
        str++; // Move to the next character
    }

    return sign * result;
}