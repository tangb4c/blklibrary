/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_utility.h
 * 
 * Description:
 * 
 * --------------------
 * 2011-9-27, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_ULTILITY_H__092711__
#define __COMM_ULTILITY_H__092711__
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <iterator>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <map>
#include <cstring>
#include "comm_macro.h"
#include "comm_base.h"
#include "comm_errorcode.h"
#include "comm_stl_algorithm.h"
namespace comm
{
namespace utility
{
struct ProcessStat
{
   pid_t pid;
   char name[24];
   char stat;
   pid_t ppid;
   pid_t pgrp;
   pid_t sessionid;
   int tty_nr;
   pid_t tpgid;
   unsigned flag;
   ulong minflt; //The number of minor faults the process has made which havenot required loading a memory page from disk.
   ulong cminflt;
   ulong majflt;
   ulong cmajflt;
   ulong utime; //Amount of time that this process has been scheduled in usermode
   ulong stime; //Amount of time that this process has been scheduled inkernel mode
   long cutime;
   long cstime;
   long priority;
   long nice;
   long num_threads;
   long itrealvalue;
   ullong starttime;
   ulong vsize;
   long rss;
   ulong rsslim;
   ulong startcode;
   ulong endcode;
   ulong startstack;
   ulong kstkesp;
   ulong signal;
   ulong blocked;
   ulong sigignore;
   ulong sigcatch;
   ulong wchan;
   ulong nswap;
   ulong cnswap;
   int exit_signal;
   pid_t processor; //cpu number last executed on
   unsigned rt_priority;
   unsigned policy;
   ullong delayacct_blkio_ticks;
};
/// <summary>
/// convert container(such as vector, set, list) to string
/// </summary>
/// <param name="ctr"></param>
/// <returns>string</returns>
template<typename T>
std::string ListToString(const T& ctr);

/// <summary>
/// 分割字符串，source中出现的在delim中任何字符，均作为分割符
/// </summary>
/// <param name="Container">容器，通常为stl中set或vector</param>
/// <param name="source">被分割的字符串</param>
/// <param name="delim">分割字符</param>
/// <param name="container">结果</param>
/// <returns></returns>
template<typename Container>
void Split(const std::string& source, const char *delim, Container& out_result);
/// <summary>
/// 分割字符串，source中出现的在delim中任何字符，均作为分割符
/// 并对每个分割后的字符串，进行Operator处理后，放入Container中
/// </summary>
/// <param name="Container">容器，通常为stl中set或vector</param>
/// <param name="source">被分割的字符串</param>
/// <param name="delim">分割字符</param>
/// <param name="container">结果</param>
/// <param name="Operator">对分割后的字符串进行转换</param>
/// <returns></returns>
template<typename Container, typename Operator>
void Split(const std::string& source, const char *delim, Container& out_result, Operator op);
/// <summary>
/// 取字符串一部分
/// </summary>
/// <param name="b">left seperator</param>
/// <param name="e">right seperator， 0 for str end</param>
/// <param name="str"></param>
/// <param name="ignore_prefix_count">ignore left seperator count</param>
/// <returns></returns>
template<std::string::value_type b, std::string::value_type e>
std::string Substr(const std::string& str, unsigned ignore_prefix_count = 0);
/// <summary>
/// trim string
/// </summary>
/// <param name="str"></param>
/// <param name="delimit"></param>
/// <returns></returns>
std::string TrimBegin(const std::string& str, char delimit = ' ');
std::string TrimEnd(const std::string& str, char delimit = ' ');
std::string Trim(const std::string& str, char delimit = ' ');

/// <summary>
/// 获取
/// </summary>
/// <returns></returns>
const char* GetTimeStr();
/// <summary>
///
/// </summary>
/// <param name="abs_time"></param>
/// <returns></returns>
const char* GetTimeStr(time_t abs_time);
/// <summary>
///
/// </summary>
/// <param name="abs_time"></param>
/// <returns></returns>
const char* GetFullTimeStr(time_t abs_time);
/// <summary>
/// 获取本机IP
/// </summary>
/// <param name="interface"></param>
/// <returns></returns>
unsigned GetLocalAddress(const char *interface = "eth1");
/// <summary>
///
/// </summary>
/// <param name="result">first: ip, second: interface-name</param>
/// <returns></returns>
void GetAllLocalAddress(std::map<unsigned, std::string>& result);

char* GetIpString(unsigned ip);
unsigned GetIp(const std::string& ipstr);
unsigned GetIp(const char *ipstr);
/// <summary>
/// get random
/// </summary>
/// <returns>RANDMAX:2147483647</returns>
unsigned GetRandom();
std::string Binary2String(const void *buf, int len);
///template <typename T>
///inline size_t SizeOf(const T& array);
///CountOf
///

/// <summary>
/// 获取当前程序名
/// </summary>
/// <returns></returns>
std::string GetAppFullPathName();
/// <summary>
/// check pid is running
/// </summary>
/// <param name="pid"></param>
/// <returns></returns>
bool IsRunningOfPid(pid_t pid);

std::string ReadLink(const char *link);
std::string GetAppPath();
std::string GetCurrentWorkPath();

/// <summary>
/// get physical memory size (byte as unit)
/// </summary>
/// <returns>bytes</returns>
unsigned long long GetPhysicalMemorySize();
void Mkdir(const std::string& path);
std::string GetAbsolutePath(const std::string& path);
time_t GetBootTime();
/// <summary>
///
/// </summary>
/// <param name="pid">0 for current process</param>
/// <param name="statinfo"></param>
/// <returns></returns>
void GetProcessTime(pid_t pid, ProcessStat& statinfo);
std::string GetProcessNameByPid(pid_t pid);

//bitwise
///find first 1 bit position, base from zero
///
inline int First1BitPosition(uint64_t value)
{
   const int lsb_64_table[64] =
   {
	  63, 30, 3, 32, 59, 14, 11, 33,
	  60, 24, 50, 9, 55, 19, 21, 34,
	  61, 29, 2, 53, 51, 23, 41, 18,
	  56, 28, 1, 43, 46, 27, 0, 35,
	  62, 31, 58, 4, 5, 49, 54, 6,
	  15, 52, 12, 40, 7, 42, 45, 16,
	  25, 57, 48, 13, 10, 39, 8, 44,
	  20, 47, 38, 22, 17, 37, 36, 26
   };
   value ^= value - 1;
   unsigned _m = (int)value ^ (value >> 32);
   return lsb_64_table[_m * 0x78291ACF >> 26];
}
}
}
#include "hpp/comm_utility.hpp"
#endif /* __COMM_ULTILITY_H__092711__ */
