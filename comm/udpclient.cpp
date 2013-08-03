#include "udpclient.h"
#include "qzone_protocol.h"
#include "qza_protocol.h"
#include "qzone_protocol_parser.h"
#include "qza_protocol_parser.h"
#define CLOSE_THROW(code, fmt, msg...) do{Close();THROW_CODE_EX(code, fmt, ##msg);}while(0)
namespace comm
{
const unsigned CONF_TIMECHANG = 0x1,    //m_timeout_ms changed
CONF_IPCHANG = 0x2,                 //ip or port changed
RESET_RCVTIME_WHEN_INIT = 0x4,      //socket SO_RCVTIMEO changed
TIMEMODE = 0x8,                     //need to update m_begintime's value before recv
CONF_ALLCHANGE = 0xFFFF;
CUdpClient::CUdpClient(bool needconnect) : m_port(0), m_timeout_ms(0), m_connect(needconnect), m_sock_fd(0), m_confstatus(CONF_ALLCHANGE)
{
	TRACE("Udpclient constructor. sock fd:%d", m_sock_fd);
}
CUdpClient::CUdpClient(const CUdpClient& client) throw(CCommException) : INetInterface()
{
	this->operator = (client);
	TRACE("Udpclient Copy. Current sock fd:%d, client sock fd:%d", m_sock_fd, client.m_sock_fd);
}
CUdpClient& CUdpClient::operator = (const CUdpClient& client) throw(CCommException)
{
	Close();
	m_connect = client.m_connect;
	SetHost(client.m_ip, client.m_port, client.m_timeout_ms);
	return *this;
}
void CUdpClient::Close()
{
	if(m_sock_fd > 0)
	{
		TRACE("close socket:%d %s", m_sock_fd, m_hostinfo.c_str());
		close(m_sock_fd);
		m_sock_fd = 0;
	}
}
CUdpClient::~CUdpClient()
{
	if(m_sock_fd > 0)
		close(m_sock_fd);
	TRACE("~Udpclient sock fd:%d", m_sock_fd);
}
unsigned CUdpClient::GetIp()
{
	return m_addr.sin_addr.s_addr;
}
unsigned CUdpClient::GetPort()
{
	return m_port;
}
const string& CUdpClient::GetIpString()
{
	return m_ip;
}
void CUdpClient::SetHost(const string& ip, unsigned port, int timeout_ms/* = 100*/) throw(CCommException)
{
	if(ip.empty() || port == 0 || port > 65535 || timeout_ms <= 0)
	{
		THROW_CODE_EX(EBadParams, "param invalid. ip:%s, port:%u, timeout_msec:%u", ip.c_str(), port, timeout_ms);
	}
	if(m_ip != ip || m_port != port)
	{
		m_ip = ip;
		m_port = port;
		m_confstatus |= CONF_IPCHANG;
		m_addr.sin_port = htons(m_port);
		m_addr.sin_family = AF_INET;
		if(inet_aton(m_ip.c_str(), &m_addr.sin_addr) < 0)
			THROW_CODE_EX(EInitFailed, "init server address[%s] failed.errmsg:%s", m_ip.c_str(), strerror(errno));

		char _buf[32];
		snprintf(_buf, sizeof(_buf), "%s:%u/UDP", ip.c_str(), port);
		m_hostinfo.assign(_buf);
	}
	if((int)m_timeout_ms != timeout_ms)
	{
		m_confstatus |= CONF_TIMECHANG;
		m_timeout_ms = timeout_ms;
		m_tv_timeout.tv_sec = m_timeout_ms / 1000;
		m_tv_timeout.tv_usec = m_timeout_ms % 1000 * 1000;
	}
}
bool CUdpClient::UpdateIOLeftTime()
{
	gettimeofday(&m_now, NULL);
	int _use_time = (m_now.tv_sec - m_begintime.tv_sec) * 1000 + (m_now.tv_usec - m_begintime.tv_usec) / 1000;
	TRACE("socketid:%d ip:%s timeout_settings:%u used_time:%d", m_sock_fd, m_hostinfo.c_str(), m_timeout_ms, _use_time);
	if(unlikely(_use_time < 0))
		return false;
	if((_use_time + 5) > (int)m_timeout_ms)
		return false;
	int _lefttime = m_timeout_ms - _use_time; //left time
	if(_lefttime < 10) //less than 10ms, need more time
	{
		m_now.tv_sec = 0;
		m_now.tv_usec = 10 * 1000;
	}
	else
	{
		m_now.tv_sec = _lefttime / 1000;
		m_now.tv_usec = _lefttime % 1000 * 1000;
	}
	int _ret;
	if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_now, sizeof(m_now))) < 0)
	{
		THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
	}
	m_confstatus |= RESET_RCVTIME_WHEN_INIT;
	return true;
}
#if 0
void CUdpClient::BeginIOTime()
{
	if(m_now.tv_sec < SOCKET_TIME_INITED) //setsockopt at UpdateIOLeftTime have executed. 1000s is a magic value, not any special mean.
	{
		int _ret;
		if((_ret = setsockopt(m_sock_fd, SOL_SOCKET,SO_RCVTIMEO,&m_tv_timeout,sizeof(m_tv_timeout))) < 0)
		{
			THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
		}
		if((_ret = setsockopt(m_sock_fd, SOL_SOCKET,SO_SNDTIMEO,&m_tv_timeout,sizeof(m_tv_timeout))) < 0)
		{
			THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
		}
		m_now.tv_sec = SOCKET_TIME_INITED;
	}
	m_istimeout = false;
	gettimeofday(&m_begintime, NULL);
}
#endif
int CUdpClient::Init() throw(CCommException)
{
	m_errmsg.clear();
	if(unlikely(m_sock_fd < 1))
	{
		if((m_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			THROW_CODE_EX(EInitFailed, "create socket failed.");
		m_confstatus = CONF_ALLCHANGE;
	}
	if(unlikely(m_confstatus & CONF_TIMECHANG))
	{
		/*
		 int _ret;
		 if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
		 {
			  THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
		 }
		 if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
		 {
			  THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
		 }*/
		setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_tv_timeout, sizeof(m_tv_timeout));
		setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout));
		m_confstatus &= ~(CONF_TIMECHANG | RESET_RCVTIME_WHEN_INIT);
	}
	else if(unlikely(m_confstatus & RESET_RCVTIME_WHEN_INIT))
	{
		/*
		int _ret;
		if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
		{
			THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
		}*/
		setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout));
		m_confstatus &= ~RESET_RCVTIME_WHEN_INIT;
	}

	if(unlikely(m_confstatus & CONF_IPCHANG))
	{
		if(m_connect)
		{
			int _ret = connect(m_sock_fd, (struct sockaddr *)&m_addr, sizeof(m_addr));
			if(_ret < 0)
				THROW_CODE_EX(ENetwork, "socket connect ip:%s::%d  ret:%d err:%s\n", m_ip.c_str(), m_port, _ret, strerror(errno));
		}
		m_confstatus &= ~CONF_IPCHANG;
	}

	return RTN_SUCCESS;
}
const string& CUdpClient::GetHostInfo() const
{
	return m_hostinfo;
}
int CUdpClient::Send(const void *ibuf, unsigned ibuflen, bool until_succ __attribute__((unused))) throw(CCommException)
{
	//TRACE("pre-connection:%s, Send length:%d", m_connect?"true":"false",  ibuflen);
	Init();
	int _ret;
	for (;;)
	{
		if(likely(m_connect))
			_ret = write(m_sock_fd, ibuf, ibuflen);
		else
			_ret = sendto(m_sock_fd, ibuf, ibuflen, 0, (struct sockaddr *)&m_addr, sizeof(m_addr));
		if(unlikely(_ret == -1))
		{
			if(unlikely(errno == EINTR))
			{
				TRACE("EINTR happend. continue send data. ret:%d errno:%d %m", _ret, errno);
				continue;
			}
			CLOSE_THROW(ENetwork, "timeout at send packet. ret:%d need-send len:%u timeout:%dms, ip:%s %m",
							_ret, ibuflen, m_timeout_ms, m_hostinfo.c_str());
		}
		if(unlikely(_ret != (int)ibuflen))
		{
			CLOSE_THROW(ENetwork, "socket sendto ip:%s buf-len:%d ret:%d %m",
							m_hostinfo.c_str(), ibuflen, _ret);
		}
		break;
	}
	return _ret;
}
int CUdpClient::Recv(void *obuf, unsigned obuflen) throw(CCommException)
{
	if(unlikely(m_sock_fd < 1))
	{
		THROW_CODE_EX(EInitFailed, "not create connect.Check your code for making Send Packet first.ip:%s::%d timeout:%d", m_ip.c_str(), m_port, m_timeout_ms);
	}
	int _len = 0;
	struct sockaddr_in _remote_addr;
	for (;;)
	{
		socklen_t _addr_len = sizeof(_remote_addr);
		_len = recvfrom(m_sock_fd, obuf, obuflen, 0, (sockaddr *)&_remote_addr, &_addr_len);
		if(unlikely(_len < 0))
		{
			if(unlikely(errno == EINTR))
			{
				TRACE("EINTR happend. continue recv data. ret:%d errno:%d %m", _len, errno);
				continue;
			}
			CLOSE_THROW(ENetwork, "socket recv ip:%s recv-len:%d timeout:%dms msg:%s %m",
							m_hostinfo.c_str(), _len, m_timeout_ms, m_errmsg.c_str());
		}
		if(unlikely(_len == 0))
		{
			CLOSE_THROW(ENetwork, "peer closed. socket recv ip:%s recv-len:%d timeout:%d msg:%s %m",
							m_hostinfo.c_str(), _len, m_timeout_ms, m_errmsg.c_str());
		}
		if(likely(m_connect))
		{
			if(unlikely(_remote_addr.sin_port != m_addr.sin_port || _remote_addr.sin_addr.s_addr != m_addr.sin_addr.s_addr))
			{
				ERRMSG("unbelievable! recv-ip:%s:%u send-ip:%s recvlen:%d",
						 inet_ntoa(_remote_addr.sin_addr), ntohs(_remote_addr.sin_port), m_hostinfo.c_str(), _len);
				continue;
				//CLOSE_THROW(ESystemFailed, "unbelievable! receive from diff ip:%s port:%u send-ip:%s recvlen:%d %m",
				//inet_ntoa(_remote_addr.sin_addr), _remote_addr.sin_port, m_hostinfo.c_str(), _len);
				//fprintf(stderr, "unbelievable! receive from diff ip:%u\n", _remote_addr.sin_addr.s_addr);
			}
		}
		return _len;
	}
	//return _len;
}

int CUdpClient::SendAndRecv(const void *ibuf, unsigned ibuflen, void *obuf, unsigned obuflen) throw(CCommException)
{
	Send(ibuf, ibuflen);
	return Recv(obuf, obuflen);
}

int CUdpClient::SendAndRecvQzoneProtocol(const void *ibuf, unsigned ibuflen, void *obuf, unsigned obuflen) throw(CCommException)
{
	return SendAndRecvProtocol<CQzoneParser>(ibuf, ibuflen, obuf, obuflen);
	/*-------------------------------------------
	int _len = SendAndRecv(ibuf, ibuflen, obuf, obuflen);
	if(_len < (int)sizeof(QzoneProtocol))
	{
				 CLOSE_THROW(EProtocolChk, "recved length %d less than qzone protocol head. ip:%s::%d recved-len:%d buf-len:%u", _len, m_ip.c_str(), m_port, _len, obuflen);
	}
	QzoneProtocolPtr _pkg = (QzoneProtocolPtr)obuf;
	if(_pkg->soh != QzoneProtocolSOH)
	{
				 CLOSE_THROW(EProtocolChk, "QzoneProtocolSOH not found at position 0. buf[0]:0x%x", _pkg->soh);
	}
	int _pkglen = ntohl(_pkg->head.len);
	if(_pkglen > _len)
	{
				 CLOSE_THROW(EProtocolChk, "packet-head-len large than actual recved buf len. buf-len:%u pkg-len:%u", _len, _pkglen);
	}
	if(((char*)obuf)[_pkglen - 1] != QzoneProtocolEOT)
	{
				 CLOSE_THROW(EProtocolChk, "QzoneProtocolEOT not found at the end of pkg. buf[%d]:0x%x recved-len:%d ip:%s:%d", _pkglen - 1, ((char*)obuf)[_pkglen - 1], _len, m_ip.c_str(), m_port);
	}
	return _len;
	-------------------------------------------*/
}

int CUdpClient::SendAndRecvQzaProtocol(const void *ibuf, unsigned ibuflen, void *obuf, unsigned obuflen) throw(CCommException)
{
	return SendAndRecvProtocol<CQzaParser>(ibuf, ibuflen, obuf, obuflen);
	/*-------------------------------------------
	const int DIFF_FLOW = -9000000;
	unsigned _send_flow = ((QZAHEAD*)ibuf)->GetFlow();
	int _len = 0;
	Send(ibuf, ibuflen);
	do
	{
				 _len = Recv(obuf, obuflen);
				 if(_len < (int)sizeof(QZAHEAD))
				 {
								 Close();
								 THROW_CODE_EX(EProtocolChk, "recved length %d less than qzone protocol head. ip:%s::%d recved-len:%d buf-len:%u", _len, m_ip.c_str(), m_port, _len, obuflen);
				 }
				 QZAHEAD* _pkg = (QZAHEAD*)obuf;
				 int _pkglen = _pkg->GetPackLen();
				 if(_pkglen > _len)
				 {
								 Close();
								 THROW_CODE_EX(EProtocolChk, "packet-head-len large than actual recved buf len. buf-len:%u pkg-len:%u", _len, _pkglen);
				 }
				 if(_pkg->GetFlow() != _send_flow)
				 {
								 TRACE("diff flow. send:%u recv:%u", _send_flow, _pkg->GetFlow());
								 _len = DIFF_FLOW;
								 continue;
				 }
				 break;
	}while(UpdateIOLeftTime());
	if(_len == DIFF_FLOW)
	{
				 Close();
				 THROW_CODE_EX(EProtocolChk, "diff flow. uin:%u send:%u recv:%u ip:%s:%d", ((QZAHEAD*)obuf)->GetUin(), _send_flow, ((QZAHEAD*)obuf)->GetFlow(), m_ip.c_str(), m_port);
	}
	return _len;
	-------------------------------------------*/
}
}
