#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    long hours = seconds / 3600;
    hours = hours % 100; // Making sure our hours number fits in 2 digits
    string hoursString = hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);
    long minutes = (seconds % 3600)/60;
    string minutesString = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
    long remainingSeconds = seconds % 60;
    string secondString = remainingSeconds < 10 ? "0" + std::to_string(remainingSeconds) : std::to_string(remainingSeconds);
    return hoursString + ":" + minutesString + ":" + secondString;
}