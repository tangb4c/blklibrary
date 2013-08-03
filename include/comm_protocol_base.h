/******************************************************************************
 * comm_protocol_base.h - 协议公共库
 * 
 * Copyright 2009-2009 Tencent Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *     协议公共库
 * modification history
 * --------------------
 * 01a, 2009-dec-15, comm written
 * --------------------
 ******************************************************************************/

#ifndef __COMM_PROTOCOL_BASE_H_8004__
#define __COMM_PROTOCOL_BASE_H_8004__
#include <arpa/inet.h>
#include <byteswap.h>
#include <typeinfo>
#include "comm_base.h"
//for unsigned long long
#if BYTE_ORDER == LITTLE_ENDIAN
	#undef htonq
	#define htonq(x) bswap_64(x)
#elif BYTE_ORDER == BIG_ENDIAN
	#undef htonq
	#define htonq(x)  (x)
#endif
namespace comm
{
/// <summary>
/// 字节对齐, 将buf指针，移动到字节对齐的位置
/// </summary>
/// <param name="align">要对齐的字节2/4/8/16/...</param>
/// <param name="buf">要对齐的buf指针，值会被修改</param>
/// <param name="left_len">buf剩余空间, 值会被修改</param>
/// <returns></returns>
/*-------------------------------------------
inline void RoundUp(int align, char* &buf, int* left_len)
{
	char* _tmp = (buf + align - 1) & (char*)(~(align - 1));
	*left_len -= (_tmp - buf);
	if(left_len < 0)
		THROW_EX("align skip failed. not enough buf %d left.", *left_len);
	buf = _tmp;
}
-------------------------------------------*/
/// <summary>
/// 对齐当前指针
/// </summary>
/// <param name="align"></param>
/// <param name="buf_begin"></param>
/// <param name="buf_current"></param>
/// <param name="left_len"></param>
/// <returns></returns>
inline void RoundUp(int align, const char *buf_begin, char * &buf_current, int* left_len)
{
	return;
	int _al = (buf_current - buf_begin) % align;
	if(_al)
	{
		if(*left_len < _al)THROW_EX("align skip failed. not enough buf %d left.", *left_len);
		*left_len -= _al;
		buf_current += _al;
	}
}
inline void RoundUp(int align, const char *buf_begin, const char* &buf_current, int* left_len)
{
	return;
	int _al = (buf_current - buf_begin) % align;
	if(_al)
	{
		if(*left_len < _al)THROW_EX("align skip failed. not enough buf %d left.", *left_len);
		*left_len -= _al;
		buf_current += _al;
	}
}
#undef FETCH_CHAR
#undef FETCH_SHORT
#undef FETCH_INT
#undef FETCH_DATA
#undef FETCH_LONGLONG
#undef APPEND_CHAR
#undef APPEND_SHORT
#undef APPEND_INT
#undef APPEND_DATA
#undef APPEND_LONGLONG
/*打包*/
#define SKIP_BYTES(buf, buf_len, skip_bytes) do{if(int(buf_len) < int(skip_bytes) || (skip_bytes) < 0)THROW_EX("Not enough buff length %d for skip len:%d", (buf_len), (skip_bytes));buf_len -= (skip_bytes);buf += (skip_bytes);}while(0)
//从网络buffer中解包数据
#define N_FETCHCHAR(buff, val) do{val = *(unsigned char*)buff; buff += sizeof(char);/*fprintf(stderr, "fetch char. " #buff " " #val "is %u\n", val);*/}while(0)
#define N_FETCHSHORT(buff, val) do{val = ntohs(*(unsigned short*)(buff)); buff += sizeof(short);/*fprintf(stderr, "fetch short. " #buff " " #val "is %u\n", val);*/}while(0)
#define N_FETCHINT(buff, val) do{val = ntohl(*(unsigned*)(buff)); buff += sizeof(int);/*fprintf(stderr, "fetch int. " #buff " " #val "is %u\n", val);*/}while(0)
#define N_FETCHDATA(buff, pointer, len) do{memcpy(pointer, buff, len); buff += len;/*fprintf(stderr, "fetch data. " #buff " " #pointer "'s len is %u\n", len);*/}while(0)
#define N_FETCHLONGLONG(buf, val) do{val = htonq(*(unsigned long long*)(buff));buff+=sizeof(unsigned long long);}while(0)
#ifndef FETCH_CHAR
#define FETCH_CHAR(buff, buf_len, val) do{if(buf_len < (int)sizeof(char))THROW_EX("Fetch Char failed. buf_len:%d less than sizeof(char)\n", buf_len);val = *(unsigned char*)buff; buff += sizeof(char);buf_len -= sizeof(char);}while(0)
#endif
#ifndef FETCH_SHORT
#define FETCH_SHORT(buff, buf_len, val) do{if(buf_len < (int)sizeof(short))THROW_EX("Fetch Short failed. buf_len:%d less than sizeof(short)\n", buf_len);val = ntohs(*(unsigned short*)(buff)); buff += sizeof(short);buf_len -= sizeof(short);}while(0)
#endif
#ifndef FETCH_INT
#define FETCH_INT(buff, buf_len, val) do{if(buf_len < (int)sizeof(int))THROW_EX("Fetch Int failed. buf_len:%d less than sizeof(short)\n", buf_len);val = ntohl(*(unsigned*)(buff)); buff += sizeof(int);buf_len -= sizeof(int);}while(0)
#endif
#ifndef FETCH_DATA
#define FETCH_DATA(buff, buf_len, pointer, len) do{if(buf_len < len)THROW_EX("Fetch Data failed. buf_len:%d less than need fetch len:%d\n", buf_len, len);memcpy((pointer), buff, len); buff += len;buf_len -= len;}while(0)
#endif
#ifndef FETCH_LONGLONG
#define FETCH_LONGLONG(buff, buf_len, val) do{if(buf_len < (int)sizeof(unsigned long long))THROW_EX("Fetch unsigned long long failed. buf_len;%d less than sizeof(unsigned long long)\n", buf_len);val=htonq(*(unsigned long long*)buff);buff+=sizeof(unsigned long long);buf_len -= sizeof(unsigned long long);}while(0)
#endif

//向网络buffer中打包数据
#define N_APPEND_CHAR(buff, val) do{        \
    unsigned char c = (unsigned char)(val); \
    memcpy(buff, &c, sizeof(char)); buff += sizeof(char);}while(0)
    
#define N_APPEND_SHORT(buff, val) do{       \
    unsigned short s = htons(val);          \
    memcpy(buff, &s, sizeof(short));buff += sizeof(short);}while(0)
    
#define N_APPEND_INT(buff, val) do{         \
    unsigned m = htonl(val);                \
    memcpy(buff, &m, sizeof(unsigned)); buff += sizeof(unsigned);}while(0)
																												  
#define N_APPEND_LONGLONG(buff, val) do{																		  \
	unsigned long long m = htonq(val);  																		  \
	memcpy(buff, &m, sizeof(unsigned long long)); buff += sizeof(unsigned long long);}while(0)

#define N_APPEND_DATA(buff, pointer, len) do{memcpy(buff, pointer, len); buff += len;}while(0)

#ifndef APPEND_CHAR
//向网络buffer中打包数据，溢出检查
#define APPEND_CHAR(buff, buf_len, val) do{  																		 \
	if((buf_len) < (int)sizeof(char))THROW_EX("AppendChar failed. buf_len:%d is too small.", buf_len); 					 \
	N_APPEND_CHAR(buff, val);   																					 \
	buf_len -= sizeof(char);																						 \
}while(0)
#endif

#ifndef APPEND_SHORT
#define APPEND_SHORT(buff, buf_len, val) do{  																		 \
	if((buf_len) < (int)sizeof(short))THROW_EX("AppendShort failed. buf_len:%d is too small.", buf_len); 				 \
	N_APPEND_SHORT(buff, val);   																					 \
	buf_len -= sizeof(short);																						 \
}while(0)
#endif

#ifndef APPEND_INT
#define APPEND_INT(buff, buf_len, val) do{  																		 \
	if((buf_len) < (int)sizeof(int))THROW_EX("AppendInt failed. buf_len:%d is too small.", buf_len); 				     \
	N_APPEND_INT(buff, val);   																					     \
	buf_len -= sizeof(int);																						     \
}while(0)
#endif

#ifndef APPEND_DATA
#define APPEND_DATA(buff, buf_len, pointer, len) do{ 																  \
	if((buf_len) < (int)len)THROW_EX("AppendData failed. buf_len:%d is small than %d.", buf_len, len); 		    		  \
	N_APPEND_DATA(buff, pointer, len);   																    		  \
	buf_len -= len;																						    		  \
}while(0)   																										  
#endif

//template fetch and append implentment
template<typename T>
inline T FETCH_VAR(const char* &buf, int *leftlen)
{
	THROW_CODE_EX(EBadParams, "not implentment %s's FETCH_VAR", typeid(T).name());
}
template<typename T>
inline void APPEND_VAR(char* &buf, int *leftlen, T value)
{
	THROW_CODE_EX(EBadParams, "not implentment %s's APPEND_VAR", typeid(T).name());
}
//============FETCH_VAR implentment
template<>
inline char FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(char))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(char)\n", *leftlen);
	*leftlen -= sizeof(char);
	return *buf++;
}
template<>
inline unsigned char FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(char))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(char)\n", *leftlen);
	*leftlen -= sizeof(char);
	return *(unsigned char*)buf++;
}

template<>
inline short FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(short))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(short)\n", *leftlen);
	*leftlen -= sizeof(short);
	short _v = ntohs(*(short*)buf);
	buf += sizeof(short);
	return _v;
}
template<>
inline unsigned short FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(short))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(short)\n", *leftlen);
	*leftlen -= sizeof(short);
	unsigned short _v = ntohs(*(unsigned short*)buf);
	buf += sizeof(short);
	return _v;
}
template<>
inline int FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(int))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(int)\n", *leftlen);
	*leftlen -= sizeof(int);
	int _v = ntohl(*(int*)buf);
	buf += sizeof(int);
	return _v;
}
template<>
inline unsigned FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(int))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(int)\n", *leftlen);
	*leftlen -= sizeof(int);
	unsigned _v = ntohl(*(unsigned*)buf);
	buf += sizeof(int);
	return _v;
}
template<>
inline long long FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(long long))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(long long)\n", *leftlen);
	*leftlen -= sizeof(long long);
	long long _v = htonq(*(long long*)buf);
	buf += sizeof(long long);
	return _v;
}
template<>
inline unsigned long long FETCH_VAR(const char* &buf, int *leftlen)
{
	if(*leftlen < (int)sizeof(long long))THROW_EX("FETCH_VAR failed. leftlen:%d less than sizeof(long long)\n", *leftlen);
	*leftlen -= sizeof(long long);
	unsigned long long _v = htonq(*(unsigned long long*)buf);
	buf += sizeof(long long);
	return _v;
}
//====================APPEND_VAR implentment
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, char value)
{
	if(*leftlen < (int)sizeof(char))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(char)\n", *leftlen);
	*leftlen -= sizeof(char);
	*buf++ = value;
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, unsigned char value)
{
	if(*leftlen < (int)sizeof(char))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(char)\n", *leftlen);
	*leftlen -= sizeof(char);
	*buf++ = value;
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, short value)
{
	if(*leftlen < (int)sizeof(short))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(short)\n", *leftlen);
	*leftlen -= sizeof(short);
	*(short*)buf = htons(value);
	buf += sizeof(short);
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, unsigned short value)
{
	if(*leftlen < (int)sizeof(short))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(short)\n", *leftlen);
	*leftlen -= sizeof(short);
	*(unsigned short*)buf = htons(value);
	buf += sizeof(short);
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, int value)
{
	if(*leftlen < (int)sizeof(int))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(int)\n", *leftlen);
	*leftlen -= sizeof(int);
	*(int*)buf = htonl(value);
	buf += sizeof(int);
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, unsigned value)
{
	if(*leftlen < (int)sizeof(int))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(int)\n", *leftlen);
	*leftlen -= sizeof(int);
	*(unsigned*)buf = htonl(value);
	buf += sizeof(int);
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, long long value)
{
	if(*leftlen < (int)sizeof(long long))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(long long)\n", *leftlen);
	*leftlen -= sizeof(long long);
	*(long long*)buf = htonq(value);
	buf += sizeof(long long);
}
template<>
inline void APPEND_VAR(char* &buf, int *leftlen, unsigned long long value)
{
	if(*leftlen < (int)sizeof(long long))THROW_EX("APPEND_VAR failed. leftlen:%d less than sizeof(long long)\n", *leftlen);
	*leftlen -= sizeof(long long);
	*(unsigned long long*)buf = htonq(value);
	buf += sizeof(long long);
}

}
#endif

