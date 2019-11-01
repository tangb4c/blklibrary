/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * qza_protocol_parser.h
 * 
 * Description:
 * 
 * --------------------
 * 2011-12-2, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __QZA_PROTOCOL_PARSER_H__120211__
#define __QZA_PROTOCOL_PARSER_H__120211__

#include <arpa/inet.h>
#include "comm_base.h"
#include "qza_protocol.h"
namespace comm
{
    #define HEADPTR(buf) ((QZAHEAD*)(buf))
    class CQzaParser:public CCommBase
    {
        public:
            /// <summary>
            /// 包体存在错误，返回RTN_ERROR
            /// </summary>
            /// <param name="qza_packet"></param>
            /// <param name="len"></param>
            /// <returns></returns>
            int CheckPacket(const void *qza_packet, int len)throw();
            /// <summary>
            /// 若包体存在错误，返回RTN_ERROR
			/// 若仅流水号不匹配，返回RTN_WARN
            /// </summary>
            /// <param name="qza_packet"></param>
            /// <param name="len"></param>
            /// <param name="flow">若flow为0, 则不检查流水号是否一致</param>
            /// <returns></returns>
            int CheckPacket(const void *qza_packet, int len, unsigned flow)throw();
			/// <summary>
			/// 用于S++中HandleInput，本方法的返回值可直接返回给SPP,可用于TCP/UDP
			/// </summary>
			/// <param name="qza_packet"></param>
			/// <param name="len"></param>
			/// <returns>返回0,表示继续接收，返回负数，表示发生错误， 返回正数，表示包的长度。</returns>
			 int CheckPacketAtSpp(const void *qza_packet, int len)throw();
        public:
            //void SetParseBuffer(void *buf, int buf_len){m_qzone = (QzoneProtocolPtr)buf; m_qzone_len = buf_len;}
			 inline void ResetLen(const void *buf)   			{HEADPTR(buf)->_pack_len = 0; HEADPTR(buf)->_detail_info._pack_len_high = 0; HEADPTR(buf)->_detail_info._extend_len = 0;}
            //get
             inline unsigned char GetVersion(const void *buf)    {return HEADPTR(buf)->_version;}
             inline unsigned GetCmd(const void *buf)             {return ntohs(HEADPTR(buf)->_detail_info._req_cmd_item);}
             inline unsigned short GetChksum(const void *)	     {return 0;}
             inline unsigned GetSerialNo(const void *buf)        {return ntohl(HEADPTR(buf)->_pack_flow);}
             inline int GetRspcode(const void *buf)  		    {return ntohs(HEADPTR(buf)->_rsp_code);}
             inline char GetRspFlag(const void *buf)  		    {return ntohs(HEADPTR(buf)->_rsp_code);}
             inline short GetRspInfo(const void *buf) 		    {return ntohs(HEADPTR(buf)->_rsp_code);}
             inline unsigned GetProtocolStructLen()				{return sizeof(QZAHEAD);}
             inline unsigned GetLen(const void *buf)             {return HEADPTR(buf)->GetPackLen();}
			 inline char* GetBody(const void *buf)				{return HEADPTR(buf)->GetBody();}
			//compute body length according to header info.
			 inline int GetHeadLen(const void *buf)				{return HEADPTR(buf)->GetHeadLen();}
             inline int GetBodyLen(const void *buf)           	{return HEADPTR(buf)->GetPackLen() - HEADPTR(buf)->GetHeadLen();}
			//
			 inline int ComputeMaxBodyLen(int max_buf_len)		{return max_buf_len - sizeof(QZAHEAD);}
			
            //set
			 inline void SetVersion(void *buf, unsigned char version)		{HEADPTR(buf)->_version = version;}
			 inline void SetCmd(void *buf, unsigned int cmd)					{HEADPTR(buf)->_detail_info._req_cmd_item = htons(cmd);}
			 inline void SetChksum(void *, unsigned short )					{}
			 inline void SetSerialNo(void *buf, unsigned serialno)			{HEADPTR(buf)->_pack_flow = htonl(serialno);}
			 inline void SetRspFlag(void *buf, char , short rspinfo)			{HEADPTR(buf)->SetRspCode(rspinfo);}
			 inline void SetPacketLen(void *buf, unsigned packet_len)		{HEADPTR(buf)->SetPackLen(packet_len);}
			 inline unsigned SetPacketLenWithBodyLen(void *buf, unsigned body_len){unsigned __len = body_len + HEADPTR(buf)->GetHeadLen(); HEADPTR(buf)->SetPackLen(__len);return __len;}
    };
    #undef HEADPTR
}
#endif /* __QZA_PROTOCOL_PARSER_H__120211__ */ 
