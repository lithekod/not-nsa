#include "interface.hpp"

#include <Arduino.h>

#include <cstring>

const char* REGISTER_COMMAND = "reg";
const char* SET_EVENT_COMMAND = "set event";
const char* EVENT_END_COMMAND = ";done;";

const char* ENTER_LIU_ID_MSG = "Enter LiU ID\n\r> ";
const char* CARD_SCAN_MSG = "Scan card\n\r";
const char* INVALID_ID_MSG = "Liu ID must be 8 characters\n\r";
const char* READ_EVENT_OUTPUT = "Enter event details (liu id, notes\\n). End with ;done;\n\r>";
const char* EVENT_REGISTRATION_DONE_MSG = "Event entered, bye!\r\n>";

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
                else if(strcmp(line_buffer, SET_EVENT_COMMAND) == 0) {
                    state = InterfaceState::READ_EVENT;
                    strcpy(to_output, READ_EVENT_OUTPUT);
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
            else if(state == InterfaceState::READ_EVENT) {
                // Check for the done commant
                if(strcmp(line_buffer, EVENT_END_COMMAND) == 0) {
                    strcpy(to_output, EVENT_REGISTRATION_DONE_MSG);
                    state = InterfaceState::WAITING;
                }
                else {
                    // TODO: Replace 9 with constant
                    line_buffer[8] = '\0';
                    new_attendee = EventAttendee(line_buffer, line_buffer + 9);
                    has_new_attendee = true;
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

EventAttendee* Interface::get_new_attendee() {
    if(has_new_attendee) {
        has_new_attendee = false;
        return &new_attendee;
    }
    return nullptr;
}

