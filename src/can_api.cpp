#include "can_api.h"
#include <SimpleCAN.h>

void _initCAN(int tx, int rx) {
    // SimpleCAN initialization is handled in STM32_main.cpp
}

void _transmitCAN(uint32_t identifier, uint8_t *data, uint8_t length) {
    CanMsg msg(identifier, length, data);
    CAN.write(msg);
}

bool _receiveCAN(uint32_t *identifier, uint8_t *data, uint8_t *length) {
    if (CAN.available()) {
        CanMsg msg = CAN.read();
        *identifier = msg.id;
        *length = msg.data_length;
        memcpy(data, msg.data, msg.data_length);
        return true;
    }
    return false;
}

void _getUniqueID(uint8_t *id) {
    // Get STM32's unique ID
    uint32_t *uid = (uint32_t *)0x1FFF7A10;  // STM32G4 UID address
    
    // Convert to byte array
    for (int i = 0; i < 3; i++) {
        id[i*2] = (uid[i] >> 8) & 0xFF;
        id[i*2+1] = uid[i] & 0xFF;
    }
}