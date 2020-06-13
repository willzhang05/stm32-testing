#include "mbed.h"

Serial device(USBTX, USBRX);
DigitalOut led(LED1);

int main() {
    device.baud(38400);
    device.printf("Hello world!\n");
    uint16_t* msg_buf = new uint16_t[128];
    char c;
    while (1) {
        device.read(msg_buf, 128);
        led = 0;
        c = device.getc();
        device.putc(c + 1);
        led = 1;
    }
}
