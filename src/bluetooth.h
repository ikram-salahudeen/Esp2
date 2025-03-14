#pragma once
#include "mbed.h"
#include <vector>
#include <stdlib.h>
#include <map>
#include <string>

struct Bluetooth {
    char buffer[256];
    int buffer_length;

    bool expecting_AT_output;
    
    std::map<std::string, float> params;
        

    Serial serial;
    Bluetooth(PinName tx, PinName rx, int baud): serial(tx, rx, baud), buffer_length(0), expecting_AT_output(false) {
        serial.attach(callback(this, &Bluetooth::recieved_data_isr));
        serial.printf("Hello\n");
    };

    // Proccess a command in the form "lhs=float"
    void recieved_command(std::string command) {
        // Is it an AT command?
        if (command.length() >= 2 && command[0] == 'A' && command[1] == 'T') {
            // AT command detected, repeat what user said onto the TX pin
            serial.printf("%s", command.c_str());
            expecting_AT_output = true;
            return;
        }

        // Parse the expersion var_name=float

        std::string lhs;
        std::string rhs;

        bool success = false;

        for (int i = 0; i < command.length(); i++) {
            if (command[i] == '=') {
                lhs = command.substr(0, i);
                rhs = command.substr(i + 1);

                success = true;
                break;
            }
        }

        if (success == false) {
            serial.printf("\n>Invalid command, listing params:\n");
            for (std::map<std::string, float>::iterator it = params.begin(); it != params.end(); it++) {
                serial.printf("%s = %f,", it->first.c_str(), it->second);
            }
            serial.printf("\n");
            return;
        }


        // Now we have lhs and rhs, set the parameter accordingly
        
        std::map<std::string, float>::iterator it = params.find(lhs);
        // Check to see if the paramater actually exists in the map
        if (it == params.end())  {
            serial.printf("\n>Invalid variable %s\n", lhs.c_str());
            return;
        }
            
        // Convert to float
        char *str_end = rhs.end();
        float val = strtof(rhs.begin(), &str_end);
        if (str_end == rhs.begin()) {
            serial.printf("\n>Invalid float\n");
            return;
        }
        it->second = val;
        serial.printf("\n>Successfully changed %s to %f\n", it->first.c_str(), it->second);
        for (std::map<std::string, float>::iterator it = params.begin(); it != params.end(); it++) {
            serial.printf("%s = %f,", it->first.c_str(), it->second);
        }
        serial.printf("\n");
        
    }

    void recieved_byte(char new_byte) {

        // When expecting_AT_output==true it is not the user typing, it's the response of the module
        if (expecting_AT_output) {
            // Put whatevers on RX onto TX
            serial.putc(new_byte);
            if (new_byte == '\n') expecting_AT_output = false;
            return;
        }

        
        buffer[buffer_length] = new_byte;
        buffer_length++;
        
        if (buffer_length >= sizeof(buffer)) {
            buffer_length = 0;
        }

        if (new_byte == '\n' || new_byte == ';') {
            recieved_command(std::string(buffer, buffer_length - 1));
            buffer_length = 0;
        }
    }

    // Called everytime there is a new byte available to read on the UART pins
    void recieved_data_isr() {
        while (serial.readable()) {
            recieved_byte(serial.getc());
        }
    }
};

extern Bluetooth bt;