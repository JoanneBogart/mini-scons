// $Header$
// author: Sawyer Gillespie

#ifndef	_H_FACILITES_UTILITY
#define	_H_FACILITIES_UTILITY

#include <vector>
#include <algorithm>
#include <utility>

// utility functions for use with iterators - 

// NOTE: In STL form, these functions could be encapsulated into iterators 
//  themselves. However the gcc compiler does not necessarily support the 
//  iterator class (?). It is 
//  therefore more simple to create these functions 
//  (it also requires less code, but might not be as useful down the road).

template <class FwdIt, class UniPred, class BinPred>
inline std::pair<FwdIt, FwdIt>	adjacent_find_if ( const FwdIt& first, const FwdIt& last, UniPred if_pred, BinPred adj_pred )
{
    FwdIt   i = std::find_if( first, last, if_pred );
    FwdIt   j = i;
    if (i != last)  i = std::find_if( ++i, last, if_pred );
    while (i  != last) {
        if (adj_pred ( (*j), (*i) ))	return std::make_pair(j,i);
	j = i++;
	while (!(if_pred(*i)) && (i != last)) ++i;
    }
    return std::make_pair(last, last);
}

template <class FwdIt, class UniPred, class BinPred>
inline std::vector<FwdIt>  adjacent_find_multiple( const FwdIt& first, 
					    const FwdIt& last, 
					    UniPred if_pred, 
					    BinPred adj_pred, 
					    unsigned N = 2 )

// FwdIt class is a forward iterator class into a container (must support ++ operator)
// BinPred is a binary predicate function which returns bool (note that there are two separate
//  comparison functions: one for the individual element adjacency, and another for group
//  adjacency (that is there are two levels of adjacency here.)
{
    std::vector<FwdIt>	itvec;
    std::pair<FwdIt, FwdIt>	adjpr1 = adjacent_find_if( first, last, if_pred, adj_pred );
    if (adjpr1.second == last)	return itvec;

    itvec.push_back( adjpr1.first);
    itvec.push_back( adjpr1.second );
    std::pair<FwdIt, FwdIt> adjpr2;

    while ((itvec.size() < N) && (adjpr1.second != last)) {
	adjpr2 = adjacent_find_if( adjpr1.second, last, if_pred, adj_pred );
	if (adjpr2.second == last)  return  std::vector<FwdIt>();
	if (adj_pred((*adjpr1.second), (*adjpr2.second)))  itvec.push_back(adjpr2.second);
	else	{
	    itvec.clear();
	    itvec.push_back(adjpr2.first);
	    itvec.push_back(adjpr2.second);
	}
	adjpr1.second = adjpr2.second;
    }
    return itvec;
}
#endif

