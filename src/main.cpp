#include <mbed.h>
#include <rtos.h>
#include <mbed_events.h>
#include <string>
#include <list>

Serial device(USBTX, USBRX);

CAN can1(PD_0, PD_1);
CAN can2(PB_5, PB_6);

DigitalIn button(USER_BUTTON);
DigitalOut rx_led(LED2);
DigitalOut tx_led(LED3);

rtos::Thread send_thread;
rtos::Thread recv_thread;

void send_can_message(string message) {
    CANMessage msg(1, message.c_str(), message.length() + 1);
    tx_led = 1;
    if (can1.write(msg)) {
        device.printf("[CAN1] Sent CAN message '%s'\n\r", message.c_str());
    } else {
        device.printf("[CAN1] Failed to send CAN message '%s'\n\r", message.c_str());
    }
    tx_led = 0;
    
}

void recv_func() {
    CANMessage message;
    device.printf("[CAN2] Starting thread to receive CAN messages...\n\r");
    while (1) {
        rx_led = 1;
        while (can2.read(message, 0)) {
            device.printf("[CAN2] '%s'\n\r", message.data);
        }
        rx_led = 0;
    }
}

void read_from_serial() {
    string buffer = "";
    list<string> messages;
    char c;
    device.printf("[CAN1] Starting thread to listening for input...\n\r");
    while (1) {
        while (1) {
            c = device.getc();
            device.printf("%c", c);
            if (c == '\r') {
                messages.push_back(buffer);
                break;
            } else {
                buffer += c;
            }

            if (buffer.length() == 7) {
                messages.push_back(buffer);
                buffer = "";
            }
        }
        device.printf("\n\r");
        while (!messages.empty()) {
            send_can_message(messages.front());
            messages.pop_front();
        }
        while (device.readable()) {
            c = device.getc();
            device.printf("%c\n\r", c);
        }
    }

}

int main() {
    device.baud(38400);
    send_thread.start(read_from_serial);
    recv_thread.start(recv_func);
}

