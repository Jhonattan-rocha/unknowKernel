#include "io/io.h"

void delay(int s) {
    for (volatile int i = 0; i < s * 1000; i++);
}
