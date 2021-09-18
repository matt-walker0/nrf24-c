#include <stdio.h>
#include "pico/stdlib.h"
#include <RF24.h>         // rf24 radio object
#include "nrf24.h"

// NRF24 C WRAPPER: Used to call nrf24 common operations from C
// Expects to be in a project with RF24 included as library. Defined in main CMakeLists.txt

RF24 radio; // instantiate an object for the nrf24L01 transceiver

// Return true if setup correctly
bool nrf24Init(uint8_t address[2][6], uint8_t SPI_BUS, uint8_t SCK_PIN, uint8_t TX_PIN, uint8_t RX_PIN, uint8_t CE_PIN, uint8_t CSN_PIN) {
    
    if(SPI_BUS == 0) {
        spi.begin(spi0, SCK_PIN, TX_PIN, RX_PIN);        // Setup SPI bus
    }
    else if(SPI_BUS == 1) {
        spi.begin(spi1, SCK_PIN, TX_PIN, RX_PIN);        // Setup SPI bus
    }
    else { return (false); } // Unknown SPI bus

    if(radio.begin(&spi, CE_PIN, CSN_PIN) == false) {     // Setup and configure rf radio
        return(false);
    }
    radio.setAutoAck(1);                    // Ensure ACK is enabled
    radio.setRetries(2,15);                 // delay, max no. of retries
    radio.setPayloadSize(5);                // Here we are sending 1-byte payloads to test the call-response speed
    radio.openWritingPipe(address[1]);      // Both radios listen on the same pipes by default, and switch when writing
    radio.openReadingPipe(1,address[0]);
    radio.startListening();                 // Start listening
    return(true);
}

void nrf24TestRadio() {
    radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

// Return true if ack_rec
bool nrf24SendData(uint8_t buffer[5]) {
    bool ack_rec = radio.write(buffer, 5);
    if (ack_rec == true) {
        return(true);
    }
    else { return (false); }
}

// Modifies buffer with RX data
void nrf24ReadData(uint8_t buffer[5]) {
    if(radio.available() == true) {  // if there is data in the RX FIFO
        radio.read(&buffer, 5); // this clears the RX FIFO      
    }                 
}

// Returns true if new RX data available
bool nrf24InterruptHandle() {
    bool tx_ok, tx_fail, rx_ready;           
    radio.whatHappened(tx_ok, tx_fail, rx_ready); // get values for IRQ masks 

    if(rx_ready == true) { // new rx data
        return(true);
    }
    else {
        return(false); // either tx was ok or tx failed
    }
}