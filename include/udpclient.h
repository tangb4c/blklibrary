#ifndef __UDPCLIENT_H_8780__
#define __UDPCLIENT_H_8780__
#include <string>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "comm_base.h"
#include "comm_macro.h"
#include "comm_network_interface.h"
namespace comm
{
    using namespace std;
    /******************************************************************************
     * CUdpClient - UDP类
     * DESCRIPTION: - 
     *    封装UDP操作，当遇到网络连接失败，超时，参数错误时，将抛出异常 
     * Input: 
     * Output: 
     * Returns: 
     * 
     * modification history
     * --------------------
     * 01a, 21dec2009,  written
     * --------------------
     ******************************************************************************/
	class CUdpClient:public INetInterface
	{
		public:
			CUdpClient(bool needconnect = true);
    		CUdpClient(const CUdpClient& client)throw(CCommException);
            CUdpClient& operator=(const CUdpClient&)throw(CCommException);
            ~CUdpClient();
		public:
			virtual unsigned GetIp();
			virtual unsigned GetPort();
			virtual const string& GetIpString();
			/// <summary>
			/// 
			/// </summary>
			/// <param name="ip"></param>
			/// <param name="port"></param>
			/// <param name="timeout_ms">若timeout_ms小于等于0,则为阻塞模式</param>
			/// <returns></returns>
			virtual void SetHost(const string& ip, unsigned port, int timeout_ms = 100)throw(CCommException);
			virtual int Init()throw(CCommException);
			virtual void Close();
			virtual const string& GetHostInfo()const;
            /******************************************************************************
                 * CUdpClient - 发送数据
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
                 * CUdpClient - 接收数据，并返回长度
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
			virtual int SendAndRecv(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
			virtual int SendAndRecvQzoneProtocol(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
			virtual int SendAndRecvQzaProtocol(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
			template<typename protocol>
			int SendAndRecvProtocol(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException);
		protected:
			//happend multi recv or send data, to decrease timeleft.
			bool UpdateIOLeftTime();
			//happend at the begin of one whole network request and response
			//void BeginIOTime();
		protected:
			string m_ip, m_hostinfo;
			unsigned m_port, m_timeout_ms;
            bool m_connect;
			struct timeval m_begintime, m_now, m_tv_timeout;
			int m_sock_fd;
			unsigned m_confstatus;
			struct sockaddr_in  m_addr;
	};

	template<typename protocol>
	int CUdpClient::SendAndRecvProtocol(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException)
	{
		protocol _protocol;
		unsigned _send_flow = _protocol.GetSerialNo(ibuf);
		int _len = 0;
		Send(ibuf, ibuflen);
		for(int i = 0; i < 5; ++i)
		{
			_len = Recv(obuf, obuflen);
			int _ret = _protocol.CheckPacket(obuf, _len, _send_flow);
			if(likely(_ret == RTN_SUCCESS))
			{
				return _len;
			}

			if(_ret == RTN_ERROR)
			{
				Close();
				if((int)obuflen == _len)
					THROW_CODE_EX(EProtocolChk, "sendlen:%d recvlen:%d maxbuflen:%u (notice:recvlen == maxbuflen, maybe no enough buf for recv) %s", ibuflen, _len, obuflen, _protocol.GetLastErrmsg());
				else
					THROW_CODE_EX(EProtocolChk, "sendlen:%d recvlen:%d maxbuflen:%u %s", ibuflen, _len, obuflen, _protocol.GetLastErrmsg());
			}
		}
		Close();
		THROW_CODE_EX(ENetwork, "too many diff flow packet recved. sendlen:%u send-flow:%u last-recv:%u", ibuflen, _send_flow, _len);
	}
}
#endif
