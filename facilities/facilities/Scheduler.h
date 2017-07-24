// $Id$

#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef _MSC_VER
#pragma warning(disable:4786) 
#endif

#include <map>
#include <iostream>
class ScheduledEvent;


class GPStime {
    // Represent absolute time. Units seconds.
public:
    GPStime(double t=0):m_t(t){}
    /* */ operator double()const{return m_t;}
private:
    double m_t;

	friend GPStime& operator += (GPStime&, const double& );
};

inline GPStime& operator += (GPStime& t, const double& dt) 
{ 
	t.m_t += dt; 
	return t; 
}


class Scheduler : private std::multimap<double, ScheduledEvent*>
{
    //create a sequence of call backs to a list of ScheduledEvent
    // instances, in time order. Delete each after running 

public:
    Scheduler();
    // ctor

    ~Scheduler();
    //  dtor, removes all entries still in list

    void schedule(double deltaT, ScheduledEvent* event);
    // add a scheduled event, to occur deltaT after the present
    // the instance will be deleted after being run

    void start();
    // start the scheduler

    void stop();
    // stop the scheduler 

    bool running(){return m_running;}
    // status

    void clear();
    // purge pending events

    GPStime elapsed_time()const{return m_time;}
    // access to the current time

    void setLog(std::ostream& out);
    // define the log file, start logging to it

    void endLogging();
    // end logging.

    void printOn(std::ostream& out)const;
    // make table of pending entries

    static  Scheduler* instance();
private:
    static  Scheduler* s_instance;
    GPStime m_time;  // current time
    bool   m_running; 
    std::ostream* m_log;
};
#endif
