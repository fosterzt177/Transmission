#include "mbed.h"
#include "sx1276.h"

// Pin configuration for Raspberry Pi 4
#define PIN_MOSI    PA_7  // GPIO10 / SPI MOSI
#define PIN_MISO    PA_6  // GPIO9  / SPI MISO
#define PIN_SCLK    PA_5  // GPIO11 / SPI CLK
#define PIN_CS      PA_4  // GPIO8  / SPI Chip Select
#define PIN_RESET   PB_0  // GPIO18 / Reset
#define PIN_DIO0    PA_3  // Interrupt pins
#define PIN_DIO1    PA_2
#define PIN_DIO2    PA_1
#define PIN_DIO3    PA_0
#define PIN_DIO4    PB_1
#define PIN_DIO5    PB_2

// Frequency and modulation settings
#define RF_FREQUENCY 144390000  // 144.39 MHz (APRS)
#define FSK_BITRATE 4800        // Bitrate: 4800 bps
#define FSK_FDEV 5000           // Frequency deviation: 5 kHz
#define PAYLOAD_LENGTH 64       // Fixed packet size (64 bytes)

// SX1276 instance
SX1276 sx1276(PIN_MOSI, PIN_MISO, PIN_SCLK, PIN_CS, PIN_DIO0, PIN_DIO1, PIN_DIO2, PIN_DIO3, PIN_DIO4, PIN_DIO5, PIN_RESET);

// Configure the SX1276 for FSK mode
void configure_sx1276_fsk() {
    // Put the module into sleep mode for configuration
    sx1276.SetOpMode(RF_OPMODE_SLEEP);
    printf("SX1276 in sleep mode...\n");

    // Set modem to FSK
    sx1276.SetModem(MODEM_FSK);

    // Set operating frequency
    sx1276.SetFrequency(RF_FREQUENCY);

    // Set FSK modulation parameters
    sx1276.SetBitrate(FSK_BITRATE);
    sx1276.SetFdev(FSK_FDEV);

    // Configure fixed packet size and disable CRC
    sx1276.SetPacketMode(false);           // Fixed-length packet
    sx1276.SetPayloadLength(PAYLOAD_LENGTH); // Payload length

    // Put module into standby mode
    sx1276.SetOpMode(RF_OPMODE_STANDBY);
    printf("SX1276 configured in FSK mode at 144.39 MHz\n");
}

// Send a packet of data
void send_fsk_packet(const uint8_t *data, size_t size) {
    if (size > PAYLOAD_LENGTH) {
        printf("Error: Packet size exceeds payload length (%d bytes)\n", PAYLOAD_LENGTH);
        return;
    }

    // Put module into transmit mode
    sx1276.SetOpMode(RF_OPMODE_TRANSMITTER);
    printf("Transmitting packet...\n");

    // Transmit the data
    sx1276.Send(data, size);

    // Wait for the packet to be sent
    while (sx1276.IsChannelFree() == false) {
        wait_ms(10);
    }

    printf("Packet sent!\n");

    // Return to standby mode
    sx1276.SetOpMode(RF_OPMODE_STANDBY);
}

int main() {
    printf("Initializing SX1276 module...\n");

    // Initialize the SX1276 in FSK mode
    configure_sx1276_fsk();

    // Prepare a test packet to send
    uint8_t packet[PAYLOAD_LENGTH] = { 'H', 'e', 'l', 'l', 'o', ' ', 'A', 'P', 'R', 'S', '!' };

    // Main loop: send packets every 5 seconds
    while (1) {
        send_fsk_packet(packet, sizeof(packet));
        wait(5.0);  // Delay before sending the next packet
    }
}
