#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
	long hours = 0, minutes = 0;
  	hours = seconds / 3600;
  	minutes = seconds % 3600;
  	minutes /= 60;
  	seconds = minutes % 60;
  
  	string sHours,sMinutes,sSeconds;
  	sHours = std::to_string(hours);
  	sMinutes = std::to_string(minutes);
  	if(hours < 10)
      sHours = "0" + sHours;
  	if(minutes < 10)
      sMinutes = "0" + sMinutes;
  	sSeconds = std::to_string(seconds);
  	if(seconds < 10)
      sSeconds = "0" + sSeconds;
      
  	string retVal = sHours + ":" + sMinutes + ":" + sSeconds;
  	return retVal;
}