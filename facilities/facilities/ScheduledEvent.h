// $Id$

#ifndef SCHEDULEDEVENT_H
#define SCHEDULEDEVENT_H

#include <string>

class Scheduler;

class ScheduledEvent 
{
    // abstract base class for an event that is scheduled by the ScheduledEvent class
public:
    virtual ~ScheduledEvent(){}

    virtual void execute()=0;
    // perform the task (must be implemented)

    virtual std::string name()const;
    // describe the event. Default is the class name from class info

protected:
    ScheduledEvent(){};

    static void schedule(double t, ScheduledEvent* next);
    // subclass can easily schedule a new event

private:
    friend class Scheduler;

};
#endif
