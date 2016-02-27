/**
 *	timer.h
 *  The abstract type Timer defined in this compilation unit reproduces the way a real chronograph works.
 *  Time is measured in second.
 */

#ifndef __SIMPLETIMER__
#define __SIMPLETIMER__

#include <stdio.h>
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif
#ifdef OS_WIN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64
#else
#include <sys/time.h>
#endif

#include <unistd.h>
#include <stdlib.h>

/**
 * In order to have gooe precision in time measurement, all time values must be of type double.
 */
typedef double Time;

/**
 *	Definition of the abstract type Timer.
 *  Just declare a variable of this type to use a new timer.
 */
typedef struct {
	Time sum_;
	Time last_;
} Timer;


/**
 *	Reset the timer.
 *	Call this function to initialize the timer so that it could mesure time from the first "start" call.
 */
void timer_reset(Timer* t);

/**
 *	Start the timer.
 *	Call this function to start the timer. Note that this do not reset the timer.
 */
void timer_start(Timer* t);

/**
 *	Stop the timer.
 *	Call this function to stop the timer.
 */
void timer_stop(Timer* t);


/**
 *	Restart the timer.
 *	This function is equivament to the sequence stop - reset - start.
 *	Returns the timer value after the stop action
 */
Time timer_restart(Timer* t);

/**
 *	Gets the value of the timer.
 *  The value of the timer is the time, IN SECOND, elapsed between the FIRST "start"
 *  called after reseting the timer and the LAST stop of the timer.
 */
Time timer_value(Timer* t);


#endif /* defined(__SIMPLETIMER__) */
