//
#ifndef SIMPLEEVENT_H
#define SIMPLEEVENT_H
//## begin module.includes preserve=yes
#include "ScheduledEvent.h"

//## end module.includes preserve=yes
template <class Receiver>
class SimpleEvent : public ScheduledEvent {
	// A template class for simple events. The template parameter is the
	// class which is the receiver, i.e the class which actually performs
	// the operations.
    // usage: Scheduler::instance()->schedule(time, new SimpleEvent<MyClass>(my_instance, &MyClass::method));
public:
//## begin SimpleEvent.initialDeclarations preserve=yes
public:
   typedef void (Receiver::* Action)();

//## end SimpleEvent.initialDeclarations
   SimpleEvent(Receiver* r, Action a)
//## begin SimpleEvent::SimpleEvent%249491085.initialization preserve=yes
:m_receiver(r),m_action(a)
//## end SimpleEvent::SimpleEvent%249491085.initialization
{

//## begin SimpleEvent::SimpleEvent%249491085.body preserve=yes
//## end SimpleEvent::SimpleEvent%249491085.body
}
;
   // Constructor needs the receiver class and the method to invoke in that class

SimpleEvent(Receiver*r, Action a, std::string n)
: m_receiver(r), m_action(a), m_name(n){};

   virtual void execute();
   // All a command can do - tells the receiver to perform the action.

   virtual std::string name()const { 
       return !m_name.empty()? m_name : ScheduledEvent::name();}

private:
   Receiver* m_receiver;
   Action m_action;
   std::string m_name;

};

template <class Receiver> inline
void SimpleEvent<Receiver>::execute(){
//## begin SimpleEvent::execute%478434479.body preserve=yes
	if (m_receiver) (m_receiver->*m_action)();
//## end SimpleEvent::execute%478434479.body
}


#endif
