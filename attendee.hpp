#ifndef H_ATTENDEE
#define H_ATTENDEE

#include <Arduino.h>

#include <cstring>

struct EventAttendee {
    EventAttendee() {}
    EventAttendee(char* liu_id, char* note) {
        strcpy(this->liu_id, liu_id);
        this->note = String(String("") + note);
    }
    char liu_id[9];
    String note;
    bool logged = false;
};

#endif
