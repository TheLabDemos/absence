#ifndef _TIMING_H_
#define _TIMING_H_

#include <windows.h>

class Timer {
private:
	LARGE_INTEGER start, freq;
	LARGE_INTEGER PauseTime, LastStop;
	bool paused;

public:

	Timer();
	
	void Start();
	void Stop();
	void Resume();
	unsigned long GetTicks() const;
	unsigned long GetMilliSec() const;
	unsigned long GetSec() const;
};

#endif	//_TIMING_H_