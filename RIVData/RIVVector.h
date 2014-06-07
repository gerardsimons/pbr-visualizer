//
//  RIVVector.h
//  Afstuderen
//
//  Created by Gerard Simons on 08/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

/*
 *      A vector class able to contain an arbitrary diversity of templated classes
 *      In our case it will be used by RIVDataSet to contain RIVDataSets of different template specialisation
 *      It also contains some helper structs to enforce constrains and enable iteration
 */

#ifndef _RIV_VECTOR_H
#define _RIV_VECTOR_H

#include <vector>
#include <tuple>
#include <utility>
#include <type_traits>

template<typename T, typename U, typename... V>
struct index_of : std::integral_constant<size_t, 1 + index_of<T, V...>::value>
{ };

template<typename T, typename... V>
struct index_of<T, T, V...> : std::integral_constant<size_t, 0>
{ };

template<typename T, typename... V>
struct is_different_than_all : std::integral_constant<bool, true>
{ };

template<typename T, typename U, typename... V>
struct is_different_than_all<T, U, V...>
: std::integral_constant<bool, !std::is_same<T, U>::value && is_different_than_all<T, V...>::value>
{ };

template<typename... V>
struct are_all_different : std::integral_constant<bool, true>
{ };

template<typename T, typename... V>
struct are_all_different<T, V...>
: std::integral_constant<bool, is_different_than_all<T, V...>::value && are_all_different<V...>::value>
{ };

template <typename... V>
class RIVVector
{
    static_assert(are_all_different<V...>::value, "All types must be different!");
    
    std::tuple<std::vector<V>...> _data;
    
public:
    template<typename T>
    std::vector<T>& data()
    { return std::get<index_of<T, V...>::value>(_data); }
    
    template<typename T>
    std::vector<T> const& data() const
    { return std::get<index_of<T, V...>::value>(_data); }
    
    template<typename T>
    void push_back(T&& arg)
    { data<typename std::remove_reference<T>::type>().push_back(std::forward<T>(arg)); }
    
    int size() { std::tuple_size<std::vector<V>...>(_data); };
    
};


#endif