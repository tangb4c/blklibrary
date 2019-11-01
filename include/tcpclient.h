/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * tcpclient.h
 * 
 * Description:
 * 
 * --------------------
 * 2010-12-28, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __TCPCLIENT_H_8780__
#define __TCPCLIENT_H_8780__
#include <stdlib.h>
#include <sys/time.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "comm_base.h"
#include "comm_macro.h"
#include "comm_network_interface.h"
#include "comm_errorcode.h"
namespace comm
{
    using namespace std;
    /******************************************************************************
     * CTcpClient - TCP类
     * DESCRIPTION: - 
     *    封装TCP操作，当遇到网络连接失败，超时，参数错误时，将抛出异常 
     * Input: 
     * Output: 
     * Returns: 
     * 
     * modification history
     * --------------------
     * 01a, 21dec2009,  written
     * --------------------
     ******************************************************************************/
	class CTcpClient:public INetInterface
	{
		public:
			CTcpClient(bool keeplive = true);
    		CTcpClient(const CTcpClient& client)throw(CCommException);
            CTcpClient& operator=(const CTcpClient&)throw(CCommException);
            ~CTcpClient();
		public:
			virtual unsigned GetIp();
			virtual unsigned GetPort();
			virtual const string& GetIpString();
            //若timeout_ms小于等于0,则为阻塞模式
			virtual void SetHost(const string& ip, unsigned port, int timeout_ms = 100)throw(CCommException);
			virtual int Init()throw(CCommException);
			virtual void Close();
			virtual const string& GetHostInfo()const;
            /******************************************************************************
                 * CTcpClient - 发送数据
                 * DESCRIPTION: - 
                 *    发送数据 
                 * Input: 
                 * Output: 
                 * Returns: 
                 * 
                 * modification history
                 * --------------------
                 * 01a, 21dec2009,  written
                 * --------------------
                 ******************************************************************************/
			virtual int Send(const void* ibuf, unsigned ibuflen, bool until_succ = false)throw(CCommException);
             /******************************************************************************
                 * CTcpClient - 接收数据，并返回长度
                 * DESCRIPTION: - 
                 *    接收数据，返回数据的长度 
                 * Input: obuf:接收数据存放;  obuflen:obuf的最大长度
                 * Output: 
                 * Returns: 返回实际接收buf的长度
                 * 
                 * modification history
                 * --------------------
                 * 01a, 21dec2009,  written
                 * --------------------
                 ******************************************************************************/
			virtual int Recv(void* obuf, unsigned obuflen)throw(CCommException);
			template<typename T>
			int Recv(void* obuf, unsigned obuflen, T check_packet)throw(CCommException);
			virtual int SendAndRecv(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
			/// <summary>
			/// 发送并接收Qzone协议头的数据
			/// </summary>
			/// <param name="ibuf">发送数据buffer</param>
			/// <param name="ibuflen">发送数据长度</param>
			/// <param name="obuf">接收数据buffer</param>
			/// <param name="obuflen">接收数据buffer最大长度</param>
			/// <returns>实际接收的长度</returns>
			virtual int SendAndRecvQzoneProtocol(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
			virtual int SendAndRecvQzaProtocol(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
		protected:
			//happend multi recv or send data, to decrease timeleft.
			bool UpdateIOLeftTime();
			//happend at the begin of one whole network request and response
			void BeginIOTime();
		protected:
			string m_ip, m_hostinfo;
			unsigned m_port, m_timeout_ms;
			struct timeval m_begintime, m_time, m_tv_timeout;
			int m_sock_fd;
			struct sockaddr_in  m_addr;
            bool m_keeplive;
	};
	template<typename T>
	int CTcpClient::Recv(void* obuf, unsigned obuflen, T check_packet)throw(CCommException)
	{
		int _recvlen = 0;
		do
		{
			int _ret = read(m_sock_fd, (char*)obuf + _recvlen, obuflen - _recvlen);
			if(_ret < 0)
			{
				if(errno == EINTR || errno == EAGAIN)
				{
					TRACE("EINTR or EAGAIN happend. continue recv data. ret:%d errno:%d msg:%s", _ret, errno, strerror(errno));
					continue;
				}
			}
			if(_ret < 0)
			{
				Close();
				THROW_EX("error happend.recv from ip:%s port:%d buf-len:%d recv-len:%d, ret:%d errno:%d msg:%s", 
						m_ip.c_str(), m_port, obuflen, _recvlen, _ret, errno, strerror(errno));
			}
			_recvlen += _ret;
			if(check_packet(obuf, _recvlen))
			{
				break;
			}
		}while(_recvlen < (int)obuflen && UpdateIOLeftTime());
		if(_recvlen <= 0)
		{
			Close();
			THROW_EX("Timeout at recving data. recved-len:%d need-recv-len:%d timout:%dms errno:%d msg:%s ip:%s:%d", _recvlen, obuflen, m_timeout_ms, errno, strerror(errno), m_ip.c_str(), m_port);
		}
		if(!m_keeplive)Close();
		return _recvlen;
	}
}
#endif
