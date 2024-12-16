#ifndef DISK_H
#define DISK_H
#include "fs/file.h"

typedef unsigned int UNKNOWKERNEL_DISK_TYPE;
// Represents a real physical hard disk
#define UNKNOWKERNEL_DISK_TYPE_REAL 0
struct disk
{
    UNKNOWKERNEL_DISK_TYPE type;
    int sector_size;
    struct filesystem* filesystem;
    char model[41];
};
unsigned short disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif
