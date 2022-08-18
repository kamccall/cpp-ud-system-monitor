#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{ 
  string uptime;
  int hour, minute, second;
  
  hour   = seconds / 3600;
  minute = seconds % 3600 / 60; 
  second = seconds % 60;
  
  // KM note: investigate auto-formatting. this is total caveman programming <sigh>
  if (hour < 10) uptime = "0" + std::to_string(hour) + ":";
  	else uptime = std::to_string(hour) + ":";
  
  if (minute < 10) uptime += "0" + std::to_string(minute) + ":";
  	else uptime += std::to_string(minute) + ":";
  
  if (second < 10) uptime += "0" + std::to_string(second);
    else uptime += std::to_string(second);
  
  return uptime;
}