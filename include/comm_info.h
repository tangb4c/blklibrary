#ifndef  __COMM_INFO_H_8009__
#define __COMM_INFO_H_8009__
#include <string>
#include <sys/types.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <errno.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include "comm_base.h"
namespace comm
{
    class CInfo: public noncopyable
    {
		public:
			CInfo();
			~CInfo();
        public:
             /******************************************************************************
                 * CInfo - 获取本地IP
                 * DESCRIPTION: - 
                 *    
                 * Input: 对应于网卡的interface,比如eth1, eth0
                 * Output: 
                 * Returns: 返回为空字符串，则表示没有获取到
                 * 
                 * modification history
                 * --------------------
                 * 01a, 21dec2009,  written
                 * --------------------
                 ******************************************************************************/
            const char* GetLocalIP(const char* interface)throw(CCommException);
			/// <summary>
			/// 获取本地IP
			/// </summary>
			/// <param name="interface">such as eth1</param>
			/// <returns>返回数字</returns>
			unsigned GetLocalAddr(const char* interface) throw(CCommException);
			/// <summary>
			/// 获取CPU利用率
			/// </summary>
			/// <param name="stat_interval_msec">统计时间间隔</param>
			/// <returns>0-100</returns>
			unsigned GetUtilization(int stat_interval_msec = 500)throw(CCommException);
		protected:
			unsigned long long GetCpuTime(unsigned long long& idle)throw(CCommException);
        protected:
            std::string m_ip;
			int m_procfd;
    };
}
#endif  /* __COMM_INFO_H_8009__ */


