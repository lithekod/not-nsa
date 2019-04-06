#include "interface.hpp"

#include <cstring>

const char* REGISTER_COMMAND = "reg";

const char* ENTER_LIU_ID_MSG = "Enter LiU ID\n\r> ";
const char* CARD_SCAN_MSG = "Scan card\n\r";
const char* INVALID_ID_MSG = "Liu ID must be 8 characters\n\r";

void Interface::handle_input(char next_byte, char* to_output) {
    // If there is a byte. This is probably redundant
    if(next_byte != -1) {
        // If we get a new line, we should probably handle stuff
        if(next_byte == '\r') {
            line_buffer[line_length] = '\0';
            to_output[0] = '\n';
            to_output[1] = '\r';
            to_output += 2;

            // If we are idle, check if we got a command
            if(state == InterfaceState::WAITING) {
                // Check for register command
                if(strcmp(line_buffer, REGISTER_COMMAND) == 0) {
                    // Output message to enter LIU id and change state to READ_USER
                    strcpy(to_output, ENTER_LIU_ID_MSG);
                    state = InterfaceState::READ_USER;
                }
            }
            else if(state == InterfaceState::READ_USER) {
                // Do some basic validation
                if(strlen(line_buffer) != 8) {
                    // Tell the user if the ID was invalid
                    strcpy(to_output, INVALID_ID_MSG);
                }
                else {
                    // Store the current line as the LIU id to save when we get a scan
                    strcpy(liu_id_to_register, line_buffer);
                    // Tell the user to scan a card
                    strcpy(to_output, CARD_SCAN_MSG);
                    // Start waiting for a card
                    state = InterfaceState::WAIT_FOR_CARD;
                }
            }
            // We are done with this line, reset it
            line_length = 0;
            line_buffer[0] = '\0';
        }
        else {
            // If we didn't get any input, echo the received byte and store it
            // in the current line
            line_buffer[line_length] = next_byte;
            line_length += 1;
            to_output[0] = next_byte;
            to_output[1] = '\0';
        }
    }
}

