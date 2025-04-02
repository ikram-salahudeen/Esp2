#pragma once
#include "mbed.h"
#include <cstring>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <map>
#include <string>

extern Timer timer;

struct Bluetooth {
    char buffer[1024];
    int buffer_length;
    std::vector<std::string> commands;

    bool expecting_AT_output;
    
    std::map<std::string, float> params;
    std::map<std::string, float> outputs;
        
    RawSerial serial;
    

    Bluetooth(PinName tx, PinName rx, int baud): serial(tx, rx, baud), buffer_length(0), expecting_AT_output(false) {
        serial.attach(callback(this, &Bluetooth::recieved_data_isr));
        
    };

    void report() {
        // Report data in the outputs map in Arduino serial plotter format e.g "Var1:12.0,Var2,1.0\n"
        for (std::map<std::string, float>::iterator it = outputs.begin(); it != outputs.end(); it++) {
            serial.printf("%s:%f,", it->first.c_str(), it->second);
        }
        outputs.clear();
        serial.printf("\n");
    }



    void process() {
        int start = 0;
        for (int i = 0; i < buffer_length; i++) {
            if (buffer[i] == ';' || buffer[i] == '\n') {
                parse_command(std::string(buffer + start, i-start));
                start = i + 1;
            }
        }

        buffer_length -= start;
        std::memcpy(buffer, buffer + start, buffer_length);
    }

    // Proccess a command which should be in the form "lhs=float"
    void parse_command(std::string command) {
        serial.printf("Received command '%s'.\n", command.c_str());


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

    

    // Called everytime there is a new byte available to read on the UART pins
    void recieved_data_isr() {
        while (serial.readable()) {
            char new_byte = serial.getc();

            if (new_byte == '!') {
                params["s"] = 0;
            }

            if (new_byte == '@') {
                params["s"] = 1;
            }

            if (new_byte == '#') {
                params["s"] = 2;
            }


            if (buffer_length < sizeof(buffer)) {
                buffer[buffer_length] = new_byte;
                buffer_length++;
            }
        }
        
    }
};

extern Bluetooth bt;