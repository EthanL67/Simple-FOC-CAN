#include "Arduino.h" // Must be here for hardware defines to be linked first
#include "../can_api.h"

#if defined(STM32_H) || defined(STM32F1xx) || defined(STM32F4xx) || defined(STM32L4xx)

#include <string.h>
#include <STM32_CAN.h>

// STM32 CAN instance
STM32_CAN Can(CAN1, DEF);  //Use PA11/12 pins for CAN1.

void _initCAN(int tx, int rx) {
    // Initialize CAN with specified pins and 1Mbps baudrate
    // Note: STM32 CAN works with predefined pins that vary by variant
    // This implementation assumes tx/rx are valid STM32 CAN pins

    Can.begin();
    Can.setBaudRate(1000000);  // 1 MBPS
    
    // Serial.println("CAN Driver started");
}

void _transmitCAN(uint32_t identifier, uint8_t *data, uint8_t length) {
    // Configure CAN message frame
    CAN_message_t message;
    message.id = identifier;
    message.extended = true; // Extended 29-bit ID
    message.remote = false;  // Not a remote frame
    message.len = length;
    
    // Copy data to message
    for (int i = 0; i < length; i++) {
        message.data[i] = data[i];
    }
    
    // Send the message
    if (Can.write(message)) {
        // Successfully queued for transmission
    } else {
        // Failed to queue message
    }
}

bool _receiveCAN(uint32_t *identifier, uint8_t *data, uint8_t *length) {
    CAN_message_t message;
    
    // Check if message available (non-blocking)
    if (!Can.available()) {
        return false; // No message
    }
    
    // Read the message
    if (Can.read(message)) {
        // Only process extended frames
        if (!message.extended) {
            return false; // Skip standard 11-bit IDs
        }
        
        // Skip remote frames
        if (message.remote) {
            return false;
        }
        
        // Copy data to output parameters
        *identifier = message.id;
        memcpy(data, message.data, message.len);
        *length = message.len;
        return true;
    }
    
    return false;
}

void _getUniqueID(uint8_t * id) {
    // STM32 has a factory-programmed unique ID in the system memory area
    // The unique ID is 96 bits (12 bytes) starting at address 0x1FFF7A10
    // (address may vary depending on the specific STM32 family)
    
    // For STM32F1xx series
    #if defined(STM32F1xx)
    uint32_t *unique_id_ptr = (uint32_t *)0x1FFFF7E8;
    #elif defined(STM32F4xx)
    uint32_t *unique_id_ptr = (uint32_t *)0x1FFF7A10;
    #elif defined(STM32L4xx)
    uint32_t *unique_id_ptr = (uint32_t *)0x1FFF7590;
    #else
    // Default address for other series
    uint32_t *unique_id_ptr = (uint32_t *)0x1FFF7A10;
    #endif
    
    // Copy first 6 bytes of UID (to match ESP32 MAC length)
    memcpy(id, (uint8_t *)unique_id_ptr, 6);
}

#endif