// $Heading: Scheduler.cxx$

#include "facilities/Scheduler.h"
#include "facilities/ScheduledEvent.h"

#include <cassert>

Scheduler* Scheduler::s_instance = 0;
Scheduler* Scheduler::instance(){return (s_instance) ? s_instance : new Scheduler();}

Scheduler::Scheduler()
: m_time(GPStime(0))
, m_running(false)
, m_log(0)
{
    assert( s_instance==0);  // require only one
    s_instance = this;
}

Scheduler::~Scheduler()
{
    clear();
}
void Scheduler::clear()
{
    while( ! empty() ) {
	iterator f = begin();
	delete (*f).second;
	erase(f);
    }
    m_time = 0;
}


void Scheduler::schedule(double deltaT, ScheduledEvent* event)
{
    insert(std::make_pair(m_time+deltaT, event));
}

void Scheduler::start()
{
    m_running = true;

    while( !empty() && m_running ) {

	// get the entry at the head of the queue
	std::pair<double, ScheduledEvent*> entry = *begin();

	// set current time, remove the entry
	m_time = entry.first;
	erase(begin());

	// run, then delete it
	if( m_log ) {
	    (*m_log) << "\t" << entry.first << '\t' << entry.second->name() << std::endl ;
	}
	entry.second->execute();
	delete entry.second;
    }
}

void Scheduler::stop()
{
    m_running = false;
}
void Scheduler::printOn(std::ostream& out)const
{
    out << "\nScheduler stack: current time = " << elapsed_time() << std::endl;
    out << "\ttime\tclass name\n";
    for( const_iterator it= begin(); it !=end(); ++it){
	std::pair<double, ScheduledEvent*> entry = *it;
	out << "\t" << entry.first << '\t' << entry.second->name() << std::endl ;
    }
    out << std::endl;

}

void Scheduler::setLog(std::ostream& out)
{
    m_log = &out;
    out << "\nSchedule event: current time = " << elapsed_time() << std::endl;
    out << "\ttime\tEvent\n";

}

void Scheduler::endLogging()
{
    m_log = 0;
}
