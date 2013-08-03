#include "comm_info.h"
namespace comm
{
	CInfo::CInfo():m_procfd(0)
	{
	}
	CInfo::~CInfo()
	{
		if(m_procfd > 0)
		{
			close(m_procfd);
		}
	}
	const char* CInfo::GetLocalIP(const char* interface) throw(CCommException)
	{
		const int family = PF_INET;//IPV4
		struct ifaddrs *ifap0 = NULL, *ifap = NULL;
		char buf[NI_MAXHOST];
		struct sockaddr_in *addr4;
		struct sockaddr_in6 *addr6;

		if( getifaddrs(&ifap0) )
		{
			THROW_EX("get interface[%s] failed.%s", interface, strerror(errno));
		}

		for( ifap = ifap0; ifap != NULL; ifap=ifap->ifa_next )
		{
			if( strcmp(interface, ifap->ifa_name) != 0 ) continue;
			if( ifap->ifa_addr==NULL ) continue;
			if( (ifap->ifa_flags & IFF_UP) == 0 ) continue;
			if( family != ifap->ifa_addr->sa_family ) continue;

			if( AF_INET == ifap->ifa_addr->sa_family )
			{
				addr4 = (struct sockaddr_in *)ifap->ifa_addr;
				if( NULL != inet_ntop(ifap->ifa_addr->sa_family, (void *)&(addr4->sin_addr), buf, NI_MAXHOST) )
				{
					m_ip.assign(buf);
				}
				break;
			}
			else if( AF_INET6 == ifap->ifa_addr->sa_family )
			{
				addr6 = (struct sockaddr_in6 *)ifap->ifa_addr;
				if( IN6_IS_ADDR_MULTICAST(&addr6->sin6_addr) )
				{
					continue;
				}
				if( IN6_IS_ADDR_LINKLOCAL(&addr6->sin6_addr) )
				{
					continue;
				}
				if( IN6_IS_ADDR_LOOPBACK(&addr6->sin6_addr) )
				{
					continue;
				}
				if( IN6_IS_ADDR_UNSPECIFIED(&addr6->sin6_addr) )
				{
					continue;
				}
				if( IN6_IS_ADDR_SITELOCAL(&addr6->sin6_addr) )
				{
					continue;
				}
				if( NULL != inet_ntop(ifap->ifa_addr->sa_family, (void *)&(addr6->sin6_addr), buf, NI_MAXHOST) )
				{
					m_ip.assign(buf);
				}
				break;
			}
		}

		freeifaddrs(ifap0);
		return m_ip.c_str();
	}
	unsigned CInfo::GetLocalAddr(const char* interface) throw(CCommException)
	{
		const int family = PF_INET;//IPV4
		struct ifaddrs *ifap0 = NULL, *ifap = NULL;
		struct sockaddr_in *addr4  = NULL;

		if( getifaddrs(&ifap0) )
		{
			THROW_EX("get interface[%s] failed.%s", interface, strerror(errno));
		}

		for( ifap = ifap0; ifap != NULL; ifap=ifap->ifa_next )
		{
			if( strcmp(interface, ifap->ifa_name) != 0 ) continue;
			if( ifap->ifa_addr==NULL ) continue;
			if( (ifap->ifa_flags & IFF_UP) == 0 ) continue;
			if( family != ifap->ifa_addr->sa_family ) continue;

			if( AF_INET == ifap->ifa_addr->sa_family )
			{
				addr4 = (struct sockaddr_in *)ifap->ifa_addr;
				break;
			}
		}
		freeifaddrs(ifap0);
		if( addr4 )
			return *(unsigned *)&addr4->sin_addr;
		else
			return 0;
	}
	unsigned CInfo::GetUtilization(int stat_interval_msec)throw(CCommException)
	{
		if(m_procfd < 1)
		{
			m_procfd = open("/proc/stat", O_RDONLY);
			if( m_procfd < 0 )THROW_EX("open /proc/stat failed. error:%m");
		}
		if(stat_interval_msec <= 0)
			stat_interval_msec = 500 * 1000;
		else
			stat_interval_msec *= 1000;
		unsigned long long _begin, _end, _begin_idle, _end_idle;
		_begin = GetCpuTime(_begin_idle);
		usleep(stat_interval_msec);
		_end = GetCpuTime(_end_idle);
		//close(m_procfd);
		//m_procfd = 0;
		if(_end == _begin)
			THROW_EX("end equal to begin. %llu", _begin);
		return 100 - 100 * (_end_idle - _begin_idle) / (_end - _begin);
	}
	unsigned long long CInfo::GetCpuTime(unsigned long long &idle)throw(CCommException)
	{
		lseek(m_procfd, 0, SEEK_SET);
		char _buffer[128];
		int _count = read(m_procfd, _buffer, sizeof(_buffer));
		if(_count < 0)
		{
			THROW_EX("read from /proc/stat failed. ret:%d error:%m", _count);
		}
		unsigned long long use, nice, system, iowait, irq, softirq;
		sscanf(_buffer, "%*[^0-9]%llu\t%llu\t%llu\t%llu\t%llu\t%llu\t%llu", &use, &nice, &system, &idle, &iowait, &irq, &softirq);
		return use + nice + system + idle + iowait + irq + softirq;
	}
}

