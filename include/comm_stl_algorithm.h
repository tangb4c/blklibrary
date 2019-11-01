/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_stl_algorithm.h
 * 
 * Description:
 * 
 * --------------------
 * 2011-8-25, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_STL_ALGORITHM_H__082511__
#define __COMM_STL_ALGORITHM_H__082511__
#include <functional>
#include <sstream>
#include <algorithm>
template<class T>
struct MapCompareSecond : public std::binary_function<typename T::value_type,
													  typename T::mapped_type,
													  bool>
{
public:
   bool operator()(typename T::value_type& item,
				   const typename T::mapped_type& value) const
   {
	  return item.second == value;
   }
};
//operator
struct IsUin
{
   bool operator()(unsigned uin) const
   {
	  return uin > 10000;
   }
};
struct IsLetter : public std::unary_function<std::string::value_type, bool>
{
   std::string::value_type c;
   mutable unsigned skipcount;
   IsLetter(std::string::value_type x, unsigned skip) : c(x), skipcount(skip)
   {
   }
   bool operator()(std::string::value_type v) const
   {
	  if(v != c) return false;
	  return skipcount-- == 0;
   }
};
template<class Arg>
struct DoNothing : public std::unary_function<Arg, Arg>
{
   Arg operator()(Arg v) const
   {
	  return v;
   }
};
template<class Result>
struct StringTo : public std::unary_function<std::string, Result>
{
   Result operator()(const std::string& v) const
   {
	  std::istringstream _oss(v);
	  Result r;
	  _oss >> r;
	  return r;
   }
};
//algorithm
template<class InputIterator, class OutputIterator, class Predicate>
OutputIterator copy_if(InputIterator first, InputIterator last,
					   OutputIterator result, Predicate pred)
{
   while (first != last)
   {
	  if(pred(*first))
		 *result++ = *first;
	  ++first;
   }
   return result;
}
template<class T>
inline void StlErase(T *stl_containter, const typename T::key_type one)
{
   stl_containter->erase(one);
}

//获取first
template<class T> struct select1st : public std::unary_function<const T, typename T::first_type>
{
   typename T::first_type operator()(const T& x)
   {
	  return x.first;
   }
};

//获取second
template<class T> struct select2nd : public std::unary_function<const T, typename T::second_type>
{
   typename T::second_type operator()(const T& x)
   {
	  return x.second;
   }
};
//get var member pointer type
template< typename T > struct ExtractMemberTypeHelper;
//--get member pointer type
template< typename _Result, typename _Class >
struct ExtractMemberTypeHelper< _Result(_Class::*) >
{
    typedef _Result result_type;
    typedef _Class class_type;
};
//--get normal function type
template< typename _Result, typename _Arg >
struct ExtractMemberTypeHelper<_Result& (*)(_Arg& c) >
{
    typedef _Result result_type;
    typedef _Arg class_type;
};
//
template<class map_iterator>
struct key_iterator : public map_iterator
{
   typedef typename map_iterator::value_type::first_type key_type;
   key_iterator(const map_iterator& other) : map_iterator(other)
   {
   }
   key_type& operator*()
   {
	  return map_iterator::operator*().first;
   }
};
template<class map_type>
inline key_iterator<typename map_type::iterator> key_begin(map_type& m)
{
   return key_iterator<typename map_type::iterator>(m.begin());
}
template<class map_type>
inline key_iterator<typename map_type::iterator> key_end(map_type& m)
{
   return key_iterator<typename map_type::iterator>(m.end());
}

//member iterator
template<typename container_iterator, typename ReturnType, typename Class>
struct member_iterator : public container_iterator
{
	typedef ReturnType Class::* t_memptr;
	t_memptr m_obj;
   member_iterator(const container_iterator& other, t_memptr ptr) : container_iterator(other), m_obj(ptr)
   {
   }
   ReturnType& operator*()
   {
	  return container_iterator::operator*().*m_obj;
   }
	ReturnType* operator->()
	{
		return &(container_iterator::operator*().*m_obj);
	}
};
template<class container_type, class _Memptr>
inline member_iterator<typename container_type::iterator,
	typename ExtractMemberTypeHelper<_Memptr>::result_type,
	typename ExtractMemberTypeHelper<_Memptr>::class_type>
memptr_begin(container_type& m, _Memptr ptr)
{
   return member_iterator<typename container_type::iterator,
		typename ExtractMemberTypeHelper<_Memptr>::result_type,
		typename ExtractMemberTypeHelper<_Memptr>::class_type>
		(m.begin(), ptr);
}

template<class container_type, class _Memptr>
inline member_iterator<typename container_type::iterator,
	typename ExtractMemberTypeHelper<_Memptr>::result_type,
	typename ExtractMemberTypeHelper<_Memptr>::class_type>
memptr_end(container_type& m, _Memptr ptr)
{
   return member_iterator<typename container_type::iterator,
		typename ExtractMemberTypeHelper<_Memptr>::result_type,
		typename ExtractMemberTypeHelper<_Memptr>::class_type>
		(m.end(), ptr);
}

template<class container_type>
inline member_iterator<typename container_type::iterator,
	typename container_type::mapped_type,
	typename container_type::value_type>
second_begin(container_type& m)
{
	typedef typename container_type::value_type t_paire;
   return member_iterator<typename container_type::iterator,
		typename container_type::mapped_type,
		typename container_type::value_type>
			(m.begin(), &t_paire::second);
}

template<class container_type>
inline member_iterator<typename container_type::iterator,
	typename container_type::mapped_type,
	typename container_type::value_type>
second_end(container_type& m)
{
   return member_iterator<typename container_type::iterator,
		typename container_type::mapped_type,
		typename container_type::value_type>
			(m.begin(), &container_type::value_type::second);
}

//iterator with function (fetch special member)
template<typename container_iterator, typename ReturnType, typename _Func>
struct func_iterator : public container_iterator
{
    _Func m_func;
   func_iterator(const container_iterator& other, _Func ptr) : container_iterator(other), m_func(ptr)
   {
   }
   ReturnType& operator*()
   {
      return m_func(container_iterator::operator*());
   }
    ReturnType* operator->()
    {
        return &m_func(container_iterator::operator*());
    }
};
//_Memptr should be like: int& func(Class& a)
template<class container_type, class _Memptr>
inline func_iterator<typename container_type::iterator,
    typename ExtractMemberTypeHelper<_Memptr>::result_type,
    _Memptr>
func_begin(container_type& m, _Memptr ptr)
{
return func_iterator<typename container_type::iterator,
    typename ExtractMemberTypeHelper<_Memptr>::result_type,
    _Memptr>(m.begin(), ptr);
}
template<class container_type, class _Memptr>
inline func_iterator<typename container_type::iterator,
    typename ExtractMemberTypeHelper<_Memptr>::result_type,
    _Memptr>
func_end(container_type& m, _Memptr ptr)
{
return func_iterator<typename container_type::iterator,
    typename ExtractMemberTypeHelper<_Memptr>::result_type,
    _Memptr>(m.end(), ptr);
}
//map inserter


//operator
template<typename T>
struct DeletePtr
{
   void operator()(T *ptr) const
   {
	  delete ptr;
   }
};
#endif /* __COMM_STL_ALGORITHM_H__082511__ */
