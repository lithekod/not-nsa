#include "interface.hpp"

#include <cstring>

const char* REGISTER_COMMAND = "reg";

const char* ENTER_LIU_ID_MSG = "Enter LiU ID\n\r> ";
const char* CARD_SCAN_MSG = "Scan card\n\r";

void Interface::handle_input(char next_byte, char* to_output) {
    if(next_byte != -1) {
        if(next_byte == '\r') {
            line_buffer[line_length] = '\0';
            to_output[0] = '\n';
            to_output[1] = '\r';
            to_output += 2;

            if(state == InterfaceState::WAITING) {
                if(strcmp(line_buffer, REGISTER_COMMAND) == 0) {
                    strcpy(to_output, ENTER_LIU_ID_MSG);
                    state = InterfaceState::READ_USER;
                }
            }
            else if(state == InterfaceState::READ_USER) {
                if(strlen(line_buffer) != 8) {
                    strcpy(to_output, "Liu ID must be 8 characters");
                }
                else {
                    strcpy(liu_id_to_register, line_buffer);
                    strcpy(to_output, CARD_SCAN_MSG);
                    state = InterfaceState::WAIT_FOR_CARD;
                }
            }
            line_length = 0;
            line_buffer[0] = '\0';
        }
        else {
            line_buffer[line_length] = next_byte;
            line_length += 1;
            to_output[0] = next_byte;
            to_output[1] = '\0';
        }
    }
}

