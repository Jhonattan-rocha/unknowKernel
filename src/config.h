#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10


#define UNKNOWKERNEL_TOTAL_INTERRUPTS 512

// 100MB heap size
// voltar aqui para melhorar esse sistema, montar a table com base na ram instalada e não fixa.
#define UNKNOWKERNEL_HEAP_SIZE_BYTES 104857600
#define UNKNOWKERNEL_HEAP_BLOCK_SIZE 4096
#define UNKNOWKERNEL_HEAP_ADDRESS 0x01000000 //addr, termina a quqantidade de ram disponivel para utilizar
#define UNKNOWKERNEL_HEAP_TABLE_ADDRESS 0x00007E00 //esse endereço é para a heap table do kernel e determina quanto de ram pode usar
#define KEYBOARD_PORT 0x60
#define KEYBOARD_BUFFER_SIZE 256
#define UNKNOWKERNEL_SECTOR_SIZE 512
#define UNKNOWKERNEL_MAX_FILESYSTEMS 12
#define UNKNOWKERNEL_MAX_FILE_DESCRIPTORS 512
#define IDE_ATA   0x00
#define IDE_ATAPI 0x01
#define IDE_PRIMARY_BUS   0x1F0
#define IDE_SECONDARY_BUS 0x170
#define MAX_DISKS 512
#endif