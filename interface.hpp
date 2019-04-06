#ifndef H_INTERFACE
#define H_INTERFACE

enum class InterfaceState {
    WAITING,
    READ_USER,
    WAIT_FOR_CARD,
};

class Interface {
    public:
        // Handles a single line of input.
        void handle_input(char next_byte, char* to_output);

        InterfaceState state;
        char liu_id_to_register[9];
    private:
        char line_buffer[128];
        char line_length = 0;
};


#endif
