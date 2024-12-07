#include "idt.h"
#include "config.h"
#include "memory/memory.h"
#include "kernel.h"
#include "io/io.h"

struct idt_desc idt_descriptores[UNKNOWKERNEL_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;
char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
int buffer_pos = 0;

// Tabela simplificada de mapeamento de scan codes para caracteres
const char scancode_to_char[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z',
    'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};
extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();

void int21h_handler() {
    // Lê o scan code da porta 0x60
    unsigned char scan_code = insb(KEYBOARD_PORT);

    // Verifica se é um scan code válido (tecla pressionada)
    if (scan_code < sizeof(scancode_to_char)) {
        char key = scancode_to_char[scan_code];
        
        // Armazena no buffer se for um caractere imprimível
        if (key != 0 && buffer_pos < KEYBOARD_BUFFER_SIZE - 1) {
            keyboard_buffer[buffer_pos++] = key;
            keyboard_buffer[buffer_pos] = '\0'; // Termina a string
        }

        terminal_writechar(key, 15);
    }

    // Envia o End of Interrupt (EOI) para o PIC
    outb(0x20, 0x20);
}

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void idt_zero()
{
    print("DivisionByZeroError\n");
}

void idt_set(int interrupt_nu, void* address)
{
    struct idt_desc* desc = &idt_descriptores[interrupt_nu];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t) address >> 16;
}

void idt_init()
{
    size_t idt_len_descriptor = sizeof(idt_descriptores);
    memset(idt_descriptores, 0, idt_len_descriptor);
    idtr_descriptor.limit = idt_len_descriptor - 1;
    idtr_descriptor.base = (uint32_t) idt_descriptores;

    for (int i = 0; i < UNKNOWKERNEL_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }
    idt_set(0, idt_zero);
    idt_set(0x21, int21h);

    // load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}
