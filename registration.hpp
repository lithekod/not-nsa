#ifndef H_REGISTRATION
#define H_REGISTRATION

#include <cinttypes>

#include "attendee.hpp"

#include <tuple>

enum class RegistrationState {
    SUCCESS,
    ALREADY_REGISTERED,
    NOT_SIGNED_UP,
    NOT_RECOGNISED
};

std::tuple<RegistrationState, const char*> check_registration(
    char* liu_id,
    EventAttendee* attendees,
    std::size_t attendee_count
) {
    bool found = false;
    for(int i = 0; i < attendee_count; i++) {
        if(strcmp(liu_id, attendees[i].liu_id) == 0) {
              if(!attendees[i].logged){
                  attendees[i].logged = true;
                  return std::make_tuple(RegistrationState::SUCCESS, attendees[i].note.c_str());
              } else {
                  return std::make_tuple(RegistrationState::ALREADY_REGISTERED, "");
              }
        }
    }
    return std::make_tuple(RegistrationState::ALREADY_REGISTERED, "");
    // return {RegistrationState::NOT_SIGNED_UP, ""};
}

#endif
