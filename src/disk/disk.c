#include "io/io.h"
#include "disk.h"
#include "config.h"
#include "status.h"
#include "memory/memory.h"

struct disk disks[MAX_DISKS];
int num_disks = 0;

int disk_read_sector(int lba, int total, void* buf)
{
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);
    unsigned short* ptr = (unsigned short*) buf;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        {
            c = insb(0x1F7);
        }
        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

// Função de espera mais robusta
static void ide_wait_for_ready(int bus) {
    unsigned char status;
    do {
        status = insb(bus + 7);
    } while (status & 0x80); // Espera BSY ficar limpo
}

static int ide_wait_for_drq(int bus) {
    int timeout = 100000; // Timeout maior
    unsigned char status;
    while (timeout--) {
        status = insb(bus + 7);
        if (status & 0x08) return 1; // DRQ setado
        if (status & 0x01) return 0; // Erro
    }
    return 0; // Timeout
}

int ide_detect_drive(int bus, int drive) {
    outb(bus + 6, (drive ? 0xB0 : 0xA0));
    ide_wait_for_ready(bus);

    outb(bus + 7, 0xEC); // Comando IDENTIFY
    ide_wait_for_ready(bus);

    if (insb(bus + 7) == 0x00) return 0; // Drive não existe

    if (!ide_wait_for_drq(bus)) return 0; // Espera DRQ com timeout

    return 1;
}

unsigned short disk_search_and_init() {
    int bus, drive;
    unsigned short buffer[256];

    num_disks = 0;
    for (bus = 0; bus < 2; bus++) {
        for (drive = 0; drive < 2; drive++) {
            if (ide_detect_drive(bus ? IDE_SECONDARY_BUS : IDE_PRIMARY_BUS, drive)) {
                // Leitura dos dados de IDENTIFY
                for (int i = 0; i < 256; i++) {
                    buffer[i] = insw((bus ? IDE_SECONDARY_BUS : IDE_PRIMARY_BUS) + 0x00);
                }

                memset(&disks[num_disks], 0, sizeof(struct disk));
                disks[num_disks].type = UNKNOWKERNEL_DISK_TYPE_REAL;
                disks[num_disks].sector_size = UNKNOWKERNEL_SECTOR_SIZE;
                for (int i = 0; i < 20; i++) {
                    disks[num_disks].model[i * 2] = buffer[27 + i] >> 8;
                    disks[num_disks].model[i * 2 + 1] = buffer[27 + i] & 0xFF;
                }
                disks[num_disks].model[40] = '\0';
                disks[num_disks].filesystem = fs_resolve(&disks[num_disks]);
                num_disks++;
            }
        }
    }
    return *buffer; // Retorna o primeiro valor do buffer (para depuração)
}

struct disk* disk_get(int index) {
    if (index < 0 || index >= num_disks)
        return 0;

    return &disks[index];
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf)
{
    int found = 0;
    for (int i = 0; i < num_disks; i++) {
        if (idisk == &disks[i]) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return -EIO;
    }
    return disk_read_sector(lba, total, buf);
}
