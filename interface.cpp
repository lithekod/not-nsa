#include "interface.hpp"

#include <cstring>

void Interface::handle_input(char next_byte, char* to_output) {
    if(next_byte != -1) {
        if(next_byte == '\n') {
            
        }
        else {
            line_buffer[line_length] = next_byte;
            line_length += 1;
            to_output[0] = next_byte;
            to_output[1] = NULL;
        }
    }
}

