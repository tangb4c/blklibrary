#ifndef __COMM_FACTORY_H_8006__
#define __COMM_FACTORY_H_8006__
#include <vector>
namespace comm
{
    template<typename T>
    class CFactory
    {
        
      public:
        inline CFactory& AddHost(T& host)
        {
            m_hosts.push_back(host);
            return *this;
        }
        inline T& GetHostByUin(unsigned uin)
        {
            return m_hosts.at(uin % m_hosts.size());
        }
        inline T& GetHostByRandom()
        {
            return m_hosts.at(_rand_num++ % m_hosts.size());
        }
        inline int HostCount()
        {
            return m_hosts.size();
        }
        inline T& LastHost()
        {
            return m_hosts.back();
        }
      protected:
        unsigned _rand_num;
        std::vector<T> m_hosts;
    };
}
#endif
