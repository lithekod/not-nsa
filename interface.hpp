#ifndef H_INTERFACE
#define H_INTERFACE

#include "attendee.hpp"

enum class InterfaceState {
    WAITING,
    READ_USER,
    WAIT_FOR_CARD,
    READ_EVENT,
};

class Interface {
    public:
        // Handles a single char of input. Content to write to the serial port
        // is put in to_output. It must be long enough to contain all messages
        // that can outputed.
        void handle_input(char next_byte, char* to_output);

        InterfaceState state;
        // If InterfaceState is WAIT_FOR_CARD, this contains the LIU id to
        // register when a new card is scanned
        char liu_id_to_register[9];

        EventAttendee* get_new_attendee();
    private:
        char line_buffer[128];
        char line_length = 0;

        EventAttendee new_attendee;

        // True if there is a new attendee to add
        bool has_new_attendee;
};


#endif
