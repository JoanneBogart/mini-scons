//## begin module%352D11EA00FB.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%352D11EA00FB.cm

//## Module: Observer%352D11EA00FB; Package specification
//	Implementation of the Observer class.
//## Subsystem: facilities%3550CC6801AC
//## Source file: D:\code\glastsim\facilities\Observer.h

#ifndef Observer_h
#define Observer_h 1

#include "facilities/Adapter.h"
//## begin module%352D11EA00FB.declarations preserve=yes
#ifdef _MSC_VER
# pragma warning(disable: 4786)
#endif

#include <vector>
//## end module%352D11EA00FB.declarations


//## Class: Observer%352D0D8F02E2
//	Abstract observer class. Encapsulates the update()
//	behavior.
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

class Observer 
{
  public:
    //## Constructors (specified)
      //## Operation: Observer%894312585
      //	constructor
      Observer ()
        //## begin Observer::Observer%894312585.initialization preserve=yes
        //## end Observer::Observer%894312585.initialization
      {
        //## begin Observer::Observer%894312585.body preserve=yes
        //## end Observer::Observer%894312585.body
      }


    //## Other Operations (specified)
      //## Operation: update%892143866
      //	Encapsulate the update behavior for the Subject-Observer
      //	pattern. Respond to a change in the state of the Subject
      //	to which this observer (when instatiated) is attached.
      virtual void update () = 0;

  protected:

};

//## Class: Subject%352D0AF10220; Abstract
//	Abstract subject pattern. Follows Observer behavioral
//	pattern described in 'Design Patterns'.
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

//## Uses: <unnamed>%352D101A02D4;Observer { -> }

class Subject 
{
  public:
    //## Constructors (specified)
      //## Operation: Subject%894312586
      //	constructor
      Subject ()
        //## begin Subject::Subject%894312586.initialization preserve=yes
        : m_observers()
        //## end Subject::Subject%894312586.initialization
      {
        //## begin Subject::Subject%894312586.body preserve=yes
        //## end Subject::Subject%894312586.body
      }


    //## Other Operations (specified)
      //## Operation: attach%892143867
      //	Attach an observer to this subject.
      void attach (Observer* anObserver)
      {
        //## begin Subject::attach%892143867.body preserve=yes
	  m_observers.push_back(anObserver);
        //## end Subject::attach%892143867.body
      }

      //## Operation: detach%892143868
      //	Detach an observer from this subject.
      void detach (Observer* )
      {
        //## begin Subject::detach%892143868.body preserve=yes
	  //std::vector<Observer*>::const_iterator	it = m_observers.find(anObserver);
	  //if (it != m_observers.end())	m_observers.erase(it);
        //## end Subject::detach%892143868.body
      }

      //## Operation: notify%892143869
      //	Notify all subjects of a change.
      void notify ()
      {
        //## begin Subject::notify%892143869.body preserve=yes
	  std::vector<Observer*>::iterator	it = m_observers.begin();
	  while (it != m_observers.end()) {
	    if (*it)	(*it)->update();
	    it++;
	  }
        //## end Subject::notify%892143869.body
      }

  protected:
  private:
  private:  //## implementation
    // Data Members for Class Attributes

      //## Attribute: m_observers%352D1996009B
      //	List of all of the observers of this subject.
      //## begin Subject::m_observers%352D1996009B.attr preserve=no  private: set<Observer*> {U} 
      std::vector<Observer*> m_observers;
      //## end Subject::m_observers%352D1996009B.attr

};

//## Class: ObserverAdapter%3552473D03A8; Parameterized Class
//	Subclass of Observer which incorporates a callback
//	Adapter to another object.
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

template <class T, class Y = int>
class ObserverAdapter : public Observer  //## Inherits: <unnamed>%3552488300D6
{
  public:
    //## Constructors (specified)
      //## Operation: ObserverAdapter%894312604
      //	constructor
      ObserverAdapter (Adapter<Y>* anAdapter = 0)
        //## begin ObserverAdapter::ObserverAdapter%894312604.initialization preserve=yes
	: itsAdapter(anAdapter)
        //## end ObserverAdapter::ObserverAdapter%894312604.initialization
      {
        //## begin ObserverAdapter::ObserverAdapter%894312604.body preserve=yes
        //## end ObserverAdapter::ObserverAdapter%894312604.body
      }

      virtual ~ObserverAdapter ()
      {
		delete itsAdapter;
		itsAdapter = 0;
      }


    //## Other Operations (specified)
      //## Operation: setAdapter%894312605
      //	sets the adapter to use
      void setAdapter (Adapter<Y>* anAdapter = 0)
      {
        //## begin ObserverAdapter::setAdapter%894312605.body preserve=yes
        delete itsAdapter;
        itsAdapter = anAdapter;
        //## end ObserverAdapter::setAdapter%894312605.body
      }

      //## Operation: getAdapter%894312606
      //	access to the adapter object
      Adapter<Y>* getAdapter ()
      {
        //## begin ObserverAdapter::getAdapter%894312606.body preserve=yes
        return itsAdapter;
        //## end ObserverAdapter::getAdapter%894312606.body
      }

      //## Operation: update%894312607
      //	update overload. calls itsAdapter to execute the adapted
      //	function
      void update ()
      {
        //## begin ObserverAdapter::update%894312607.body preserve=yes
        if (itsAdapter)	(*itsAdapter)();
        //## end ObserverAdapter::update%894312607.body
      }

  protected:
  private:
  private:  //## implementation
    // Data Members for Associations

      //## Association: facilities::<unnamed>%3550D15301CA
      //## Role: ObserverAdapter::itsAdapter%3550D1540059
      //	adapter to call upon notification of a change in a
      //	Subject
      //## begin ObserverAdapter::itsAdapter%3550D1540059.role preserve=no  private: ActionAdapter { -> 0..1RHN}
      Adapter<Y> *itsAdapter;
      //## end ObserverAdapter::itsAdapter%3550D1540059.role

};



#endif
