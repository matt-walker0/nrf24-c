#include <stdio.h>
#include "pico/stdlib.h"
#include <RF24.h>         // rf24 radio object
#include "nrf24.h"
#include "hardware/gpio.h"

// NRF24 C WRAPPER: Used to call nrf24 common operations from C
// Expects to be in a project with RF24 included as library. Defined in main CMakeLists.txt

// Local functions
void nrf24InterruptHandle(uint gpio, uint32_t events);

RF24 radio; // instantiate an object for the nrf24L01 transceiver

// Return true if setup correctly
bool nrf24Init(uint8_t address[2][6], uint8_t spi_bus, uint8_t sck_pin, uint8_t tx_pin, uint8_t rx_pin, uint8_t ce_pin, uint8_t csn_pin) {
    
    if(spi_bus == 0) {
        spi.begin(spi0, sck_pin, tx_pin, rx_pin);        // Setup SPI bus
    }
    else if(spi_bus == 1) {
        spi.begin(spi1, sck_pin, tx_pin, rx_pin);        // Setup SPI bus
    }
    else { return (false); } // Unknown SPI bus

    if(radio.begin(&spi, ce_pin, csn_pin) == false) {     // Setup and configure rf radio
        return(false);
    }
    radio.setAutoAck(1);                    // Ensure ACK is enabled
    radio.setRetries(2,15);                 // delay, max no. of retries
    radio.setPayloadSize(5);                // Here we are sending 1-byte payloads to test the call-response speed
    radio.openWritingPipe(address[1]);      // Both radios listen on the same pipes by default, and switch when writing
    radio.openReadingPipe(1,address[0]);
    radio.startListening();    
    return(true);
}

// Calls nrf24InterruptHandle on IRQ pin falling
void nrf24SetupIRQ(uint8_t irq_pin, void (*irq_handler) (uint gpio, uint32_t event)) {
    gpio_set_irq_enabled_with_callback(irq_pin, GPIO_IRQ_EDGE_FALL, true, irq_handler);

}

void nrf24TestRadio() {
    radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

bool nrf24HasNewData() {
     if(radio.available() == true) {  // if there is data in the RX FIFO
        return(true);
     }
    return(false);
}

// Assumes, stop listen prior to run.
// Return true if ack_rec
bool nrf24SendData(uint8_t buffer[5]) {
    bool ack_rec = radio.write(buffer, 5);

    if (ack_rec == true) {
        return(true);
    }
    else { 
        return(false); 
    }
}

// Modifies buffer with RX data, ignore_ground stops 0 ID messages from pickup.
// True returned if read data, false if ground or something
void nrf24ReadData(uint8_t buffer[5]) {
    if(radio.available() == true) {  // if there is data in the RX FIFO
        radio.read(&buffer, 5); // this clears the RX FIFO      
    } 
}


// Returns true if new RX data available
void nrf24InterruptHandle(uint gpio, uint32_t events) {
    bool tx_ok, tx_fail, rx_ready;           
    radio.whatHappened(tx_ok, tx_fail, rx_ready); // get values for IRQ masks 

    if(rx_ready == true) { // new rx data
        //return(true);
    }
    else {
        //return(false); // either tx was ok or tx failed
    }
}

void nrf24StartListening() {
    radio.startListening();
}

void nrf24StopListening() {
    radio.stopListening();
}
