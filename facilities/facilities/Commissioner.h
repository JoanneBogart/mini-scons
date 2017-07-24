//## begin module.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module.cm

//## Module: Commissioner; Package specification
//	Implementation of the Commissioner class
//## Subsystem: utilities
//## Source file: d:\code\glastsim\utilities\Commissioner.h

#ifndef Commissioner_h
#define Commissioner_h 1

#include "Adapter.h"
//## begin module.declarations preserve=no
//## end module.declarations


//## Class: Commissioner; Parameterized Class
//	Class used to select other classes based on some
//	specific attribute within those classes. This is
//	implemented by setting some parameter within this class
//	of type _Ty and creating an Action object, of return
//	type _Ty and encapsulated within the Subject class which
//	gives access to that object.
//## Category: utilities
//## Subsystem: utilities
//## Persistence: Transient
//## Cardinality/Multiplicity: n

template <class Commissionee, class Criteria>
class Commissioner 
{
  //## begin Commissioner.initialDeclarations preserve=yes
  //## end Commissioner.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: Commissioner%894312587
      //	constructor
      Commissioner (Action<Commissionee, Criteria> aSelector, Criteria aCriteria)
        //## begin Commissioner::Commissioner%894312587.hasinit preserve=no
        //## end Commissioner::Commissioner%894312587.hasinit
        //## begin Commissioner::Commissioner%894312587.initialization preserve=yes
	: itsSelector(aSelector), itsCriteria(aCriteria)
        //## end Commissioner::Commissioner%894312587.initialization
      {
        //## begin Commissioner::Commissioner%894312587.body preserve=yes
        //## end Commissioner::Commissioner%894312587.body
      }


    //## Other Operations (specified)
      //## Operation: select%894312588
      //	executes its selection criteria on the object passed to
      //	it.
      bool select (Commissionee* aCommissionee)
      {
        //## begin Commissioner::select%894312588.body preserve=yes
	  return (itsCriteria == itsSelector.execute(aCommissionee));
        //## end Commissioner::select%894312588.body
      }

      //## Operation: select%894312589
      //	performs its selection using the criteria passed in to
      //	the funciton
      bool select (Commissionee* aCommissionee, Criteria aCriteria) const
      {
        //## begin Commissioner::select%894312589.body preserve=yes
	  return (aCriteria == itsSelector.execute(aCommissionee));
        //## end Commissioner::select%894312589.body
      }

    // Additional Public Declarations
      //## begin Commissioner.public preserve=yes
      //## end Commissioner.public

  protected:
    // Additional Protected Declarations
      //## begin Commissioner.protected preserve=yes
      //## end Commissioner.protected

  private:
    // Additional Private Declarations
      //## begin Commissioner.private preserve=yes
      //## end Commissioner.private

  private:  //## implementation
    // Data Members for Associations

      //## Association: utilities::<unnamed>%3550EE700380
      //## Role: Commissioner::itsSelector
      //	action function used to select Subjects
      //## begin Commissioner::itsSelector.role preserve=no  private: Action { -> VHN}
      Action<Commissionee, Criteria> itsSelector;
      //## end Commissioner::itsSelector.role

      //## Association: utilities::<unnamed>%3550F03B007D
      //## Role: Commissioner::<the_Command>
      //## begin Commissioner::<the_Command>.role preserve=no  public: Command {0..1 -> RHG}
      //## end Commissioner::<the_Command>.role

    // Additional Implementation Declarations
      //## begin Commissioner.implementation preserve=yes
      Criteria	itsCriteria;
      //## end Commissioner.implementation

};

//## begin Commissioner.postscript preserve=yes
//## end Commissioner.postscript



#endif
