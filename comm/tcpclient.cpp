#include "tcpclient.h"
#include "qzone_protocol.h"
#include "qza_protocol.h"
namespace comm
{
CTcpClient::CTcpClient(bool keeplive) : m_port(0), m_timeout_ms(100), m_sock_fd(0), m_keeplive(keeplive)
{
   m_time.tv_sec = 0;
   TRACE("Tcpclient constructor. sock fd:%d keeplive:%s", m_sock_fd, m_keeplive ? "true" : "false");
}
CTcpClient::CTcpClient(const CTcpClient& client) throw(CCommException)
   : INetInterface(), m_ip(client.m_ip), m_port(client.m_port), m_timeout_ms(client.m_timeout_ms), m_sock_fd(0), m_keeplive(client.m_keeplive)
{
   m_time.tv_sec = 0;
   TRACE("Tcpclient Copy. Current sock fd:%d, client sock fd:%d", m_sock_fd, client.m_sock_fd);
}
CTcpClient& CTcpClient::operator = (const CTcpClient& client) throw(CCommException)
{
   m_port = client.m_port;
   m_ip = client.m_ip;
   m_timeout_ms = client.m_timeout_ms;
   m_sock_fd = 0;
   m_time.tv_sec = 0;
   m_keeplive = client.m_keeplive;
   m_hostinfo = client.m_hostinfo;
   return *this;
}
CTcpClient::~CTcpClient()
{
   if(m_sock_fd > 0) close(m_sock_fd);
   TRACE("~Tcpclient sock fd:%d", m_sock_fd);
}
unsigned CTcpClient::GetIp()
{
   if(m_addr.sin_addr.s_addr) return m_addr.sin_addr.s_addr;
   if(!m_ip.empty())
   {
	  inet_aton(m_ip.c_str(), &m_addr.sin_addr);
   }
   return m_addr.sin_addr.s_addr;
}
unsigned CTcpClient::GetPort()
{
   return m_port;
}
const string& CTcpClient::GetIpString()
{
   return m_ip;
}
void CTcpClient::SetHost(const string& ip, unsigned port, int timeout_ms/* = 100*/) throw(CCommException)
{
   if(ip.empty() || port == 0 || port > 65535 || timeout_ms <= 0)
   {
	  THROW_CODE_EX(EBadParams, "param invalid. ip:%s, port:%u, timeout_msec:%u", ip.c_str(), port, timeout_ms);
   }
   m_ip = ip;
   m_port = port;
   m_timeout_ms = timeout_ms;
   m_tv_timeout.tv_sec = m_timeout_ms / 1000;
   m_tv_timeout.tv_usec = m_timeout_ms % 1000 * 1000;
   m_time.tv_sec = 0;
   char _buf[32];
   snprintf(_buf, sizeof(_buf), "%s:%u/TCP", ip.c_str(), port);
   m_hostinfo.assign(_buf);
}
const string& CTcpClient::GetHostInfo() const
{
   return m_hostinfo;
}
int CTcpClient::Init() throw(CCommException)
{
   if(m_sock_fd > 0)
   {
	  TRACE("reuse connect. socket-id:%d", m_sock_fd);
	  return RTN_SUCCESS;
   }
   if((m_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
	  THROW_CODE_EX(EInitFailed, "create socket failed.");
   }
   //bzero(&m_addr, sizeof(m_addr));
   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons(m_port);
   if(inet_aton(m_ip.c_str(), &m_addr.sin_addr) < 0)
   {
	  Close();
	  THROW_CODE_EX(EInitFailed, "init server address[%s] failed.errmsg:%s", m_ip.c_str(), strerror(errno));
   }
   struct linger _linger;
   _linger.l_onoff = 1;
   _linger.l_linger = 0;
   setsockopt(m_sock_fd, SOL_SOCKET, SO_LINGER,(const char *)&_linger,sizeof(_linger));
   setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout));
   setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_tv_timeout, sizeof(m_tv_timeout));
   int _ret;
   /*
   if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
   {
	  Close();
	  THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
   }
   if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
   {
	  Close();
	  THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
   }*/
   /*-------------------------------------------
   int _reuse = 1;
   if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &_reuse, sizeof(_reuse))) < 0)
   {
	   Close();
	   THROW_CODE_EX("setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
   }
   -------------------------------------------*/
   int _addr_len = sizeof(m_addr);
   _ret = connect(m_sock_fd, (struct sockaddr *)&m_addr, _addr_len);
   if(_ret < 0)
   {
	  Close();
	  THROW_CODE_EX(ENetwork, "socket connect ip:%s port:%d ret:%d errno:%d err:%s\n", m_ip.c_str(), m_port, _ret, errno, strerror(errno));
   }
   return RTN_SUCCESS;
}
bool CTcpClient::UpdateIOLeftTime()
{
   gettimeofday(&m_time, NULL);
   int _use_time = (m_time.tv_sec - m_begintime.tv_sec) * 1000 + (m_time.tv_usec - m_begintime.tv_usec) / 1000;
   if(_use_time >= (int)m_timeout_ms) return false;
   _use_time = m_timeout_ms - _use_time; //left time
   if(_use_time < 10) //less than 10ms, need more time
   {
	  m_time.tv_sec = 0;
	  m_time.tv_usec = 20 * 1000;
   }
   else
   {
	  m_time.tv_sec = _use_time / 1000;
	  m_time.tv_usec = _use_time % 1000 * 1000;
   }
   int _ret;
   if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_time, sizeof(m_time))) < 0)
   {
	  THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
   }
   if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_time, sizeof(m_time))) < 0)
   {
	  THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
   }
   return true;
}
void CTcpClient::BeginIOTime()
{
   int _ret;
   if(m_time.tv_sec < 1000) //setsockopt at UpdateIOLeftTime have executed. 1000s is a magic value(suppose m_timeout less than 1000s), not any special mean.
   {
	  if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
	  {
		 THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
	  }
	  if((_ret = setsockopt(m_sock_fd, SOL_SOCKET, SO_SNDTIMEO, &m_tv_timeout, sizeof(m_tv_timeout))) < 0)
	  {
		 THROW_CODE_EX(EInitFailed, "setsockopt failed. ret = %d.errmsg:%s", _ret, strerror(errno));
	  }
	  m_time.tv_sec = 1000;
   }
   gettimeofday(&m_begintime, NULL);
}
void CTcpClient::Close()
{
   if(m_sock_fd > 0)
   {
	  close(m_sock_fd);
	  m_sock_fd = 0;
   }
}
int CTcpClient::Send(const void *ibuf, unsigned ibuflen, bool until_succ __attribute__((unused))) throw(CCommException)
{
   TRACE("pre-connection:%s, Send length:%d", m_keeplive ? "true" : "false", ibuflen);
   Init();
   BeginIOTime();
   unsigned _sendlen = 0;
   do
   {
	  int _ret = write(m_sock_fd, (const char *)ibuf + _sendlen, ibuflen - _sendlen);
	  if(_ret < 0)
	  {
		 if((errno == EINTR || errno == EAGAIN))
		 {
			TRACE("EINTR or EAGAIN happend. continue send data. ret:%d errno:%d msg:%s", _ret, errno, strerror(errno));
			continue;
		 }
		 else
		 {
			Close();
			THROW_CODE_EX(ENetwork, "send ip:%s port:%d buf-len:%d send-len:%d ret:%d errno:%d error:%s",
						  m_ip.c_str(), m_port, ibuflen, _sendlen, _ret, errno, strerror(errno));
		 }
	  }
	  _sendlen += (unsigned)_ret;
   }while (_sendlen < ibuflen && UpdateIOLeftTime());
   if(_sendlen < ibuflen)
   {
	  Close();
	  THROW_CODE_EX(ENetwork, "Timeout at send step. send length less than buf-len, have-send length:%u need-send buf-len:%u timeout:%dms, ip:%s port:%d",
					_sendlen, ibuflen, m_timeout_ms, m_ip.c_str(), m_port);
   }

   return _sendlen;
}
int CTcpClient::Recv(void *obuf, unsigned obuflen) throw(CCommException)
{
   int _recvlen = 0;
   do
   {
	  int _ret = read(m_sock_fd, (char *)obuf + _recvlen, obuflen - _recvlen);
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
		 THROW_CODE_EX(ENetwork, "error happend.recv from ip:%s port:%d buf-len:%d recv-len:%d, ret:%d errno:%d msg:%s",
					   m_ip.c_str(), m_port, obuflen, _recvlen, _ret, errno, strerror(errno));
	  }
	  if(_ret == 0)
	  {
		 break;
	  }
	  _recvlen += _ret;
   }while (_recvlen < (int)obuflen && UpdateIOLeftTime());
   if(_recvlen <= 0)
   {
	  Close();
	  THROW_CODE_EX(ENetwork, "Timeout at recving data. recved-len:%d need-recv-len:%d timout:%dms errno:%d msg:%s ip:%s:%d", _recvlen, obuflen, m_timeout_ms, errno, strerror(errno), m_ip.c_str(), m_port);
   }
   if(!m_keeplive) Close();
   return _recvlen;
}
int CTcpClient::SendAndRecv(const void *ibuf, unsigned ibuflen, void *obuf, unsigned obuflen) throw(CCommException)
{
   Send(ibuf, ibuflen);
   return Recv(obuf, obuflen);
}
int CTcpClient::SendAndRecvQzoneProtocol(const void *ibuf, unsigned ibuflen, void *obuf, unsigned obuflen) throw(CCommException)
{
   if(obuflen < sizeof(QzoneProtocol))
	  THROW_CODE_EX(EBufSize, "not enough buf-len for save qzone protocol. buf-len:%d", obuflen);
   bool _reopen = false;
BEGIN:
   Send(ibuf, ibuflen);
   //receive qzone protocol
   int _recvlen = 0, _need_recv = sizeof(QzoneProtocol);
   //receive head
   do
   {
	  int _ret = read(m_sock_fd, (char *)obuf + _recvlen, obuflen - _recvlen);
	  if(_ret < 0)
	  {
		 if(errno == EINTR || errno == EAGAIN)
		 {
			TRACE("EINTR or EAGAIN happend. continue recv data. ret:%d errno:%d msg:%s", _ret, errno, strerror(errno));
			continue;
		 }
		 else
		 {
			Close();
			THROW_CODE_EX(ENetwork, "error happend.recv from ip:%s port:%d buf-len:%d recv-len:%d, ret:%d errno:%d msg:%s",
						  m_ip.c_str(), m_port, obuflen, _recvlen, _ret, errno, strerror(errno));
		 }
	  }
	  if(_ret == 0)
	  {
		 Close();
		 if(m_keeplive && !_reopen)
		 {
			_reopen = true;
			TRACE("try to resend, due to server close the connection.");
			goto BEGIN;
		 }
		 else
		 {
			THROW_CODE_EX(ENetwork, "server close current connection.recv from ip:%s port:%d buf-len:%d recv-len:%d, ret:%d errno:%d msg:%s",
						  m_ip.c_str(), m_port, obuflen, _recvlen, _ret, errno, strerror(errno));
		 }
	  }
	  _recvlen += _ret;
	  if(_need_recv > (int)sizeof(QzoneProtocol)) continue;
	  if(_recvlen >= (int)sizeof(QzoneProtocol))
	  {
		 QzoneProtocolPtr _pkg = (QzoneProtocolPtr)obuf;
		 if(_pkg->soh != QzoneProtocolSOH)
		 {
			Close();
			THROW_CODE_EX(EProtocolChk, "QzoneProtocolSOH not found at position 0. buf[0]:0x%x", _pkg->soh);
		 }
		 _need_recv = ntohl(_pkg->head.len);
		 if(_need_recv > (int)obuflen)
		 {
			Close();
			THROW_CODE_EX(EBufSize, "not enough buf-len for save whole qzone protocol. buf-len:%u pkg-len:%u", obuflen, _need_recv);
		 }
		 if(_need_recv < (int)sizeof(QzoneProtocol))
		 {
			Close();
			THROW_CODE_EX(EProtocolChk, "bad packet length %d from qzone protocol head. recved-len:%u ip:%s port:%d buf-len:%u", _recvlen, _need_recv, m_ip.c_str(), m_port, obuflen);
		 }
	  }
   }while (_recvlen < _need_recv && UpdateIOLeftTime());
   if(_recvlen < _need_recv)
   {
	  Close();
	  THROW_CODE_EX(ENetwork, "Timeout at recving data. recved-len:%d need-recv-len:%d timout:%dms errno:%d msg:%s ip:%s:%d", _recvlen, _need_recv, m_timeout_ms, errno, strerror(errno), m_ip.c_str(), m_port);
   }

   if(((char *)obuf)[_need_recv - 1] != QzoneProtocolEOT)
   {
	  Close();
	  THROW_CODE_EX(EProtocolChk, "QzoneProtocolEOT not found at the end of pkg. buf[%d]:0x%x recved-len:%d ip:%s:%d", _need_recv - 1, ((char *)obuf)[_need_recv - 1], _recvlen, m_ip.c_str(), m_port);
   }
   if(!m_keeplive) Close();
   return _recvlen;
}
int CTcpClient::SendAndRecvQzaProtocol(const void *ibuf, unsigned ibuflen, void *obuf, unsigned obuflen) throw(CCommException)
{
   if(obuflen < sizeof(QZAHEAD))
	  THROW_CODE_EX(EBufSize, "not enough buf-len for save qza protocol. buf-len:%d", obuflen);

   Send(ibuf, ibuflen);
   //receive qza protocol
   int _recvlen = 0, _need_recv = sizeof(QZAHEAD);
   //receive head
   do
   {
	  int _ret = read(m_sock_fd, (char *)obuf + _recvlen, obuflen - _recvlen);
	  if(_ret < 0)
	  {
		 if(errno == EINTR || errno == EAGAIN)
		 {
			TRACE("EINTR or EAGAIN happend. continue recv data. ret:%d ip:%s:%d errno:%d msg:%s", _ret, m_ip.c_str(), m_port, errno, strerror(errno));
			continue;
		 }
		 else
		 {
			Close();
			THROW_CODE_EX(ENetwork, "error happend.recv from ip:%s:%d buf-len:%d recv-len:%d, ret:%d errno:%d msg:%s",
						  m_ip.c_str(), m_port, ibuflen, _recvlen, _ret, errno, strerror(errno));
		 }
	  }
	  if(_ret == 0)
	  {
		 Close();
		 THROW_CODE_EX(ENetwork, "Server close current connection.recv from ip:%s:%d buf-len:%d recv-len:%d, ret:%d errno:%d msg:%s",
					   m_ip.c_str(), m_port, ibuflen, _recvlen, _ret, errno, strerror(errno));
	  }

	  _recvlen += _ret;
	  if(_need_recv > (int)sizeof(QZAHEAD)) continue;
	  if(_recvlen >= (int)sizeof(QZAHEAD))
	  {
		 QZAHEAD *_pkg = (QZAHEAD *)obuf;
		 _need_recv = _pkg->GetPackLen();
		 if((unsigned)_need_recv > obuflen)
		 {
			Close();
			THROW_CODE_EX(EBufSize, "not enough buf-len for save whole qza protocol. buf-len:%u pkg-len:%u ip:%s:%d", obuflen, _need_recv, m_ip.c_str(), m_port);
		 }
		 if(_need_recv < (int)sizeof(QZAHEAD))
		 {
			Close();
			THROW_CODE_EX(EProtocolChk, "bad packet length %d from qza protocol head. recved-len:%u ip:%s:%d buf-len:%u", _recvlen, _need_recv, m_ip.c_str(), m_port, obuflen);
		 }
	  }
   }while (_recvlen < _need_recv && UpdateIOLeftTime());
   if(_recvlen < _need_recv)
   {
	  Close();
	  THROW_CODE_EX(ENetwork, "Timeout at recving data. recved-len:%d need-recv-len:%d buf-max-len:%d timout:%dms errno:%d msg:%s ip:%s:%d",
					_recvlen, _need_recv, obuflen, m_timeout_ms, errno, strerror(errno), m_ip.c_str(), m_port);
   }
   if(!m_keeplive) Close();
   return _recvlen;
}
}
