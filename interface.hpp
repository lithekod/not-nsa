#ifndef H_INTERFACE
#define H_INTERFACE

enum class InterfaceState {
    WAITING,
    READ_USER,
    WAIT_FOR_ID,
};

class Interface {
    public:
        // Handles a single line of input.
        char* handle_line(char* line, char* to_output);
    private:
        InterfaceState state;

        char line_buffer[128];
        char line_length = 0;
};


#endif
