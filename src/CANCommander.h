#ifndef CANCOMMANDER_H
#define CANCOMMANDER_H

#include "communication/Commander.h"

// Only include CANDriver.h if it hasn't been included yet
#ifndef CANDRIVER_H
#include "CANDriver.h"
#endif

class CANCommander : public Commander
{
public:
    CANCommander(CANDriver &can, char eol = '\n', bool echo = false) 
      : Commander(eol, echo) {
        can_driver = &can;
        can_driver->command = *this;
        com_port = &can.stream;
    }
    
    void runWithCAN() {
        can_driver->receive();
        run();
        can_driver->transmit();
    }
    
    CANDriver* can_driver = nullptr;  

protected:
    // Since Commander's print functions are private, we need our own implementations
    void _print(const float number) {
        if(!can_driver) return;
        can_driver->stream.dataType = can_driver->stream.double_val;
        can_driver->stream.double_value = number;
    }
    
    void _print(const int number) {
        if(!can_driver) return;
        can_driver->stream.dataType = can_driver->stream.int_val;
        can_driver->stream.int_value = number;
    }
    
    void _print(const char* message) {
        if(!can_driver) return;
        // For now, we don't handle string messages
    }
    
    void _print(const __FlashStringHelper *message) {
        if(!can_driver) return;
        // For now, we don't handle string messages
    }
    
    void _print(const char message) {
        if(!can_driver) return;
        can_driver->stream.dataType = can_driver->stream.char_val;
        can_driver->stream.char_value = message;
    }
};

#endif