//## begin module%3550CCFF0159.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3550CCFF0159.cm

//## Module: Adapter%3550CCFF0159; Package specification
//	Implementation of the Adapter design pattern.
//## Subsystem: facilities%3550CC6801AC
//## Source file: D:\code\glastsim\facilities\Adapter.h

#ifndef Adapter_h
#define Adapter_h 1

//## begin module%3550CCFF0159.declarations preserve=yes
#ifdef _MSC_VER
# pragma warning(disable:4503) //'insert' : decorated name length exceeded, name was truncated
# pragma warning(disable: 4786)
#endif
//## end module%3550CCFF0159.declarations


//## Class: Adapter%356C83B90323
//	abstract adapter class, just implements the () operator
//	and sets up a structure for the execute() operation.
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

template <class _Ty>
class Adapter 
{
  public:
    //## Constructors (specified)
      //## Operation: Adapter%896216810
      //	constructor
      Adapter ()
        //## begin Adapter::Adapter%896216810.initialization preserve=yes
        //## end Adapter::Adapter%896216810.initialization
      {
        //## begin Adapter::Adapter%896216810.body preserve=yes
        //## end Adapter::Adapter%896216810.body
      }

      virtual ~Adapter() {}


    //## Other Operations (specified)
      //## Operation: operator()%896216811
      //	performs the adaptation
      virtual _Ty operator () (void) = 0;

  protected:
  private:
  private:  //## implementation
};

template <class _Ty, class _Arg>
class ArgAdapter
{
public:
    typedef	_Arg ArgType;
    typedef _Ty	RetType;
    
    ArgAdapter() {}
    virtual _Ty operator () (ArgType&) = 0;
};


//## Class: Action%3550D9BD0145; Parameterized Class
//	Action encapsulates a function call to a member function
//	of a specific class. The function call is carried out on
//	a specific instance of that class in the execute method.
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

template <class Actor, class _Ty = int>
class Action 
{
  public:
    //## Class: ActionFunction%3550DA45030E
    //## Category: <Top Level>
    //## Subsystem: facilities%3550CC6801AC
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n

    typedef _Ty  (Actor::* ActionFunction) ();

  public:
    //## Constructors (specified)
      Action (ActionFunction anAction)
	: itsFunction(anAction)
      {
      }


    //## Other Operations (specified)
      //	executes the action function using anInstance, returning
      //	the value returned by the function.
      _Ty execute (Actor* anActor)
      {
        //## begin Action::execute%894312582.body preserve=yes
	  return (anActor->*itsFunction)();
        //## end Action::execute%894312582.body
      }

  protected:
  private:
  private:  //## implementation
    // Data Members for Associations

      //## Association: facilities::<unnamed>%3550DD290286
      //## Role: Action::itsFunction%3550DD2A00E2
      //	function to execute when called.
      //## begin Action::itsFunction%3550DD2A00E2.role preserve=no  public: Action< Actor,_Ty >::ActionFunction { -> VHN}
      ActionFunction itsFunction;
      //## end Action::itsFunction%3550DD2A00E2.role

};

//## Class: ActionAdapter%3550CDFE0051; Parameterized Class
//	Adapter template class.
//
//	Bridges the interface of one class into another while
//	presenting a generic interface to clients. Adapter
//	allows classes to work together without explicit
//	relationships between those classes.
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

template <class Adaptee, class _Ty = int>
class ActionAdapter : public Adapter<_Ty>  //## Inherits: <unnamed>%356C83DF01AB
{
  public:
    //## Constructors (specified)
      //## Operation: ActionAdapter%894312583
      //	constructor
      ActionAdapter (Adaptee* anAdaptee, Action<Adaptee,_Ty> anAction)
        //## begin ActionAdapter::ActionAdapter%894312583.initialization preserve=yes
	: itsAdaptee(anAdaptee), itsAction(anAction)
        //## end ActionAdapter::ActionAdapter%894312583.initialization
      {
        //## begin ActionAdapter::ActionAdapter%894312583.body preserve=yes
        //## end ActionAdapter::ActionAdapter%894312583.body
      }


    //## Other Operations (specified)
      //## Operation: operator()%894312584
      //	performs the adaptation
      virtual _Ty operator () ()
      {
        //## begin ActionAdapter::operator()%894312584.body preserve=yes
	return itsAction.execute(itsAdaptee);
        //## end ActionAdapter::operator()%894312584.body
      }

  protected:
  private:
  private:  //## implementation
    // Data Members for Class Attributes

      //## Attribute: itsAdaptee%3550CEC300AF
      //	object for which this adapter provides an interface
      //## begin ActionAdapter::itsAdaptee%3550CEC300AF.attr preserve=no  private: Adaptee* {U} 
      Adaptee* itsAdaptee;
      //## end ActionAdapter::itsAdaptee%3550CEC300AF.attr

    // Data Members for Associations

      //## Association: facilities::<unnamed>%3550DB4401A2
      //## Role: ActionAdapter::itsAction%3550DB45006D
      //	action to be performed when the Adapter executes
      //## begin ActionAdapter::itsAction%3550DB45006D.role preserve=no  private: Action { -> VHN}
      Action<Adaptee, _Ty> itsAction;
      //## end ActionAdapter::itsAction%3550DB45006D.role

};

#endif
