/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_protocol_parser_interface.h
 * 
 * Description:
 * 
 * --------------------
 * 2011-6-14, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_PROTOCOL_PARSER_INTERFACE_H__061411__
#define __COMM_PROTOCOL_PARSER_INTERFACE_H__061411__

#include "comm_base.h"
namespace comm
{
	class IProtocolParser
	{
		public:
			virtual ~IProtocolParser(){};
			virtual	void CheckPacket(const void* buf, int len)throw(CCommException) = 0;
			virtual int CheckPacketAtSpp(const void *buf, int len) throw() = 0;
			virtual char* GetBody(const void* buf, int len) = 0;
			virtual int GetBodyLen(const void* buf, int len) = 0;
	};
}
#endif /* __COMM_PROTOCOL_PARSER_INTERFACE_H__061411__ */ 
