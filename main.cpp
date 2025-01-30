#include "mbed.h"
#include "sx1276.h"
#include "wiringPi.h"
#include <iostream>
#include <chrono>
#include <thread>

// Pin configuration for Raspberry Pi 4 (adjusting for mbed compatibility)
#define PIN_MOSI    PB_5  // SPI MOSI
#define PIN_MISO    PB_4  // SPI MISO
#define PIN_SCLK    PB_3  // SPI CLK
#define PIN_CS      PA_4  // SPI Chip Select
#define PIN_RESET   PA_0  // Reset
#define PIN_DIO0    PA_1  // Interrupt pins
#define PIN_DIO1    PA_2
#define PIN_DIO2    PA_3
#define PIN_DIO3    PA_5
#define PIN_DIO4    PA_6
#define PIN_DIO5    PA_7

// Frequency and modulation settings
#define RF_FREQUENCY 144390000  // 144.39 MHz (APRS)
#define FSK_BITRATE  4800       // Bitrate: 4800 bps
#define FSK_FDEV     5000       // Frequency deviation: 5 kHz
#define PAYLOAD_LENGTH 64       // Fixed packet size (64 bytes)

// SX1276 instance
RadioEvents_t RadioEvents;
SX1276 sx1276(&RadioEvents, PIN_MOSI, PIN_MISO, PIN_SCLK, PIN_CS, PIN_RESET, PIN_DIO0, PIN_DIO1, PIN_DIO2, PIN_DIO3, PIN_DIO4, PIN_DIO5);

// Configure the SX1276 for FSK mode
void configure_sx1276_fsk() {
    // Set modem to FSK
    sx1276.SetModem(MODEM_FSK);

    // Set operating frequency
    sx1276.SetChannel(RF_FREQUENCY);
    printf("SX1276 configured in FSK mode at 144.39 MHz\n");

    // Set FSK modulation parameters
    sx1276.SetTxConfig(MODEM_FSK, (int8_t)FSK_BITRATE, FSK_FDEV, 0, 0, 0, 5, false, true, 0, 0, false, 3000);
}

// Send a packet of data
void send_fsk_packet(const uint8_t *data, size_t size) {
    if (size > PAYLOAD_LENGTH) {
        printf("Error: Packet size exceeds payload length (%d bytes)\n", PAYLOAD_LENGTH);
        return;
    }

    // Transmit the data
    sx1276.Send(data, size);
    printf("Packet sent!\n");
}

int main() {
    if (wiringPiSetup() == -1) {
        std::cerr << "Failed to initialize WiringPi!" << std::endl;
        return 1;
    }

    pinMode(PIN_CS, OUTPUT);
    pinMode(PIN_MISO, INPUT);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_SCLK, OUTPUT);
    pinMode(PIN_RESET, OUTPUT);
    pinMode(PIN_DIO0, INPUT);
    pinMode(PIN_DIO1, INPUT);
    pinMode(PIN_DIO2, INPUT);
    pinMode(PIN_DIO3, INPUT);
    pinMode(PIN_DIO4, INPUT);
    pinMode(PIN_DIO5, INPUT);

    std::cout << "Pin modes set" << std::endl;

    printf("Initializing SX1276 module...\n");

    // Initialize the SX1276 in FSK mode
    configure_sx1276_fsk();

    // Prepare a test packet to send
    uint8_t packet[PAYLOAD_LENGTH] = { 'H', 'e', 'l', 'l', 'o', ' ', 'A', 'P', 'R', 'S', '!' };

    // Main loop: send packets every 5 seconds
    while (1) {
        send_fsk_packet(packet, sizeof(packet));
        std::this_thread::sleep_for(std::chrono::seconds(5));  // Delay before sending the next packet
    }

    return 0;
}

