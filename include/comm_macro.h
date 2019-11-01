#ifndef __COMM_MACRO_H_8870__
#define __COMM_MACRO_H_8870__
#include <string>
#include <errno.h>
#include <stdio.h>
#ifdef TRACE
	#undef TRACE
#endif

#if !__GLIBC_PREREQ(2, 3)
#define __builtin_expect(x, expected_value) (x)
#endif
#ifndef likely
	#define likely(x)   __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
	#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

namespace comm
{
	//base macro
	#define SET_STRING(str, fmt, arg...) do{str.resize(512); int __size = snprintf((char*)str.data(), 512, fmt, ##arg); str.resize(__size); }while(0)
	#define APPEND_STRING(stl_str, fmt, arg...) do{int __size = stl_str.size(); if(__size < 0)__size = 0;int __newsize = __size > 256? 2 * __size: 512;stl_str.resize(__newsize); __size += snprintf((char*)stl_str.data()+__size, __newsize - __size, fmt, ##arg);stl_str.resize(__size);}while(0)
	
	#ifdef DEBUG
	//base log macro
	#define ERRMSG_STRING(stl_str, fmt, arg...) do{SET_STRING(stl_str, "[%s %s %s %d]\t\t" fmt "\n", comm::utility::GetTimeStr(), __FILE__, __FUNCTION__, __LINE__, ##arg);fprintf(stderr, "%s\n", stl_str.c_str());}while(0)
	#define ERRMSG_APDSTR(stl_str, fmt, arg...) do{APPEND_STRING(stl_str, fmt, ##arg);fprintf(stderr, "%s\n", stl_str.c_str());}while(0)
	
	#ifndef TRACE
		#define TRACE(fmt, arg...) do{fprintf(stderr, "[T %s %s %s %d]\t\t" fmt "\n", comm::utility::GetTimeStr(), __FILE__, __FUNCTION__, __LINE__, ##arg);}while(0)
	#endif

	#else 

	#define ERRMSG_STRING(stl_str, fmt, arg...) SET_STRING(stl_str, "[%s %s %s %d]\t\t" fmt "\n", comm::utility::GetTimeStr(), __FILE__, __FUNCTION__, __LINE__, ##arg)
	#define ERRMSG_APDSTR(stl_str, fmt, arg...) APPEND_STRING(stl_str, fmt, ##arg)

	#ifndef TRACE
		#define TRACE(fmt, arg...)
	#endif

	#endif
	
	//extend log macro
	#define ERRMSG(fmt, arg...) ERRMSG_STRING(m_errmsg, fmt, ##arg)
	#define ERRMSG_APPEND(fmt, arg...) APPEND_STRING(m_errmsg, fmt, ##arg)
	
	
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned long ulong;
	typedef long long llong;
	typedef unsigned long long ullong;
}
#endif

