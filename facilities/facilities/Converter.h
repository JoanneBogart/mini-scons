// $Id$
// 
//  Original author: Sawyer Gillespie
//                   hgillesp@u.washington.edu
//

#ifndef _H_Converter_facilities_
#define _H_Converter_facilities_

// class Converter
//  Template class to define the converter behavior. This is really just 
//  specifying a pattern.
//
template <class Ty1, class Ty2>
class   Converter {
public:
    // type declarations
    typedef Ty1     source;
    typedef Ty2     destination;

    // declare the forward method
    inline destination*    operator (const source&) const;

protected:
    // do the actual conversion here
    virtual destination*    convert (const source&) const = 0;
};

// inline declarations

template <class Ty1, class Ty2>
inline Converter<Ty1,Ty2>::destination* 
Converter<Ty1,Ty2>::operator () (const Converter<Ty1,Ty2>::source& s) const
{
    return  convert (s);
}

#endif // _H_Converter_facilities_