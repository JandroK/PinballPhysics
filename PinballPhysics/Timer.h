#ifndef TIMER_H
#define TIMER_H

#include "Globals.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	int Read() const;
	float ReadSec() const;
	bool check(int interval);

private:
	int startTime;
};
#endif // MODULE_TIMER_H