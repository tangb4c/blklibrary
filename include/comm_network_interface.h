/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_network_interface.h
 * 
 * Description:
 * 
 * --------------------
 * 2010-8-23, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_NETWORK_INTERFACE_H__082310__
#define __COMM_NETWORK_INTERFACE_H__082310__
#include <string>
#include "comm_base.h"
namespace comm
{
	class INetInterface:public CCommBase
	{
		public:
			virtual ~INetInterface(){};
			virtual unsigned GetIp() = 0;
			virtual unsigned GetPort() = 0;
			virtual const std::string& GetIpString() = 0;
			virtual	void SetHost(const std::string& ip, unsigned port, int timeout_ms = 100)throw(CCommException) = 0;
			/// <summary>
			/// recreate socket fd no matter what the current socket have be created or not.
			/// </summary>
			/// <returns></returns>
			virtual int Init()throw(CCommException) = 0;
			virtual void Close() = 0;
			virtual const std::string& GetHostInfo()const  = 0;
			virtual int Send(const void* ibuf, unsigned ibuflen, bool until_succ = false)throw(CCommException) = 0;
			virtual int Recv(void* obuf, unsigned obuflen)throw(CCommException) = 0;
			template<typename T>
			int Recv(void* obuf, unsigned obuflen, T& check_packet);
			virtual int SendAndRecv(const void* ibuf, unsigned ibuflen, void* obuf, unsigned obuflen)throw(CCommException)
			{
				this->Send(ibuf, ibuflen);
				return this->Recv(obuf, obuflen);
			}
			virtual int SendAndRecvQzoneProtocol(const void* ibuf __attribute__((unused)), unsigned ibuflen __attribute__((unused)), void* obuf __attribute__((unused)), unsigned obuflen __attribute__((unused)))throw(CCommException)
			{
				THROW_EX("not implement this funciton:SendAndRecvQzoneProtocol");
			}
			virtual int SendAndRecvQzaProtocol(const void* ibuf __attribute__((unused)), unsigned ibuflen __attribute__((unused)), void* obuf __attribute__((unused)), unsigned obuflen __attribute__((unused)))throw(CCommException)
			{
				THROW_EX("not implement this funciton:SendAndRecvQzaProtocol");
			}

	};
}

#endif /* __COMM_NETWORK_INTERFACE_H__082310__ */ 
