/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * qzone_protocol_parser.h
 * 
 * Description:
 * 
 * --------------------
 * 2010-5-26, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __QZONE_PROTOCOL_PARSER_H__052610__
#define __QZONE_PROTOCOL_PARSER_H__052610__
#include <arpa/inet.h>
#include "comm_base.h"
#include "qzone_protocol.h"
namespace comm
{
#define HEADPTR(buf) (((QzoneProtocolPtr)(buf))->head)
	class CQzoneParser:public CCommBase
	{
	public:
		/// <summary>
		/// 包体存在错误，返回RTN_ERROR
		/// </summary>
		/// <param name="qzone_packet"></param>
		/// <param name="len"></param>
		/// <returns></returns>
		int CheckPacket(const void *qzone_packet, int len)throw();
		/// <summary>
		/// 若包体存在错误，返回RTN_ERROR
		/// 若仅流水号不匹配，返回RTN_WARN
		/// </summary>
		/// <param name="qzone_packet"></param>
		/// <param name="len"></param>
		/// <param name="flow">若flow为0, 则不检查流水号是否一致</param>
		/// <returns></returns>
		int CheckPacket(const void *qzone_packet, int len, unsigned flow)throw();
		/// <summary>
		/// 用于S++中HandleInput，本方法的返回值可直接返回给SPP,可用于TCP/UDP
		/// </summary>
		/// <param name="qzone_packet"></param>
		/// <param name="len"></param>
		/// <returns>返回0,表示继续接收，返回负数，表示发生错误， 返回正数，表示包的长度。</returns>
		int CheckPacketAtSpp(const void *qzone_packet, int len)throw();
	public:
		inline void ResetLen(const void *buf)   			{HEADPTR(buf).len = 0;}
		//void SetParseBuffer(void *buf, int buf_len){m_qzone = (QzoneProtocolPtr)buf; m_qzone_len = buf_len;}
		//get
		inline unsigned char GetVersion(const void *buf)    {return HEADPTR(buf).version;}
		inline unsigned GetCmd(const void *buf)             {return ntohl(HEADPTR(buf).cmd);}
		inline unsigned short GetChksum(const void *buf)    {return ntohs(HEADPTR(buf).checksum);}
		inline unsigned GetSerialNo(const void *buf)        {return ntohl(HEADPTR(buf).serialNo);}
		inline int GetRspcode(const void *buf)  		    {return unsigned(GetRspFlag(buf)) << 16 | GetRspInfo(buf);}
		inline char GetRspFlag(const void *buf)             {return HEADPTR(buf).serverResponseFlag;}
		inline short GetRspInfo(const void *buf)            {return ntohs(HEADPTR(buf).serverResponseInfo);}
		inline unsigned GetProtocolStructLen()              {return sizeof(QzoneProtocol) + sizeof(char);}
		inline unsigned GetLen(const void *buf)             {return ntohl(HEADPTR(buf).len);}
		inline char* GetBody(const void *buf)               {return((QzoneProtocolPtr)(buf))->body;}
		//compute body length according to header info.
		inline unsigned GetHeadLen(const void *)			{return GetProtocolStructLen();}
		inline int GetBodyLen(const void *buf)              {return ntohl(HEADPTR(buf).len) - GetProtocolStructLen();}
		//
		inline int ComputeMaxBodyLen(int max_buf_len)       {return max_buf_len - GetProtocolStructLen();}

		//set
		inline void SetDefault(void* buf)					{memset(buf, 0, GetProtocolStructLen());}
		inline void SetVersion(void *buf, unsigned char version)       {HEADPTR(buf).version = version;}
		inline void SetCmd(void *buf, unsigned int cmd)                    {HEADPTR(buf).cmd = htonl(cmd);}
		inline void SetChksum(void *buf, unsigned short chksum)            {HEADPTR(buf).checksum = htons(chksum);}
		inline void SetSerialNo(void *buf, unsigned serialno)          {HEADPTR(buf).serialNo = htonl(serialno);}
		inline void SetRspFlag(void *buf, char rspflag, short rspinfo) {HEADPTR(buf).serverResponseFlag = rspflag;HEADPTR(buf).serverResponseInfo = htons(rspinfo);}
		inline void SetPacketLen(void *buf, unsigned packet_len)       {HEADPTR(buf).len = htonl(packet_len);*(char*)buf = QzoneProtocolSOH;((char*)buf)[packet_len - 1] = QzoneProtocolEOT;}
		inline unsigned SetPacketLenWithBodyLen(void *buf, unsigned body_len){unsigned __len = body_len + sizeof(QzoneProtocol) + sizeof(char);HEADPTR(buf).len = htonl(__len);*(char*)buf = QzoneProtocolSOH;((char*)buf)[__len - 1] = QzoneProtocolEOT;return __len;}
	};
#undef HEADPTR
}

#endif /* __QZONE_PROTOCOL_PARSER_H__052610__ */ 
