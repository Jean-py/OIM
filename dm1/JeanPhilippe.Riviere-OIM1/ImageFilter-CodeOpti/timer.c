/**
 *	timer.c
 */

#include "timer.h"


Time timer_get_time(Timer* t){
	struct timeval tv;
	gettimeofday ( &tv, NULL );
	return ( Time ) ( ( double ) tv.tv_sec + 1.e-6 * ( double ) tv.tv_usec );
}



void timer_reset(Timer* t){
	t->last_=t->sum_=0.;
}

void timer_start(Timer* t){
	t->last_=timer_get_time(t);
}

void timer_stop(Timer* t){
	t->sum_+=timer_get_time(t)-t->last_;
}

Time timer_restart(Timer* t){
	timer_stop(t);
	Time v = timer_value(t);
	timer_reset(t);
	timer_start(t);
	return v;
}

Time timer_value(Timer* t){
	return t->sum_;
}


