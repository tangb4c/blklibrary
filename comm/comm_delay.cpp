#include "comm_delay.h"
namespace comm
{
	CFreqCtl::CFreqCtl()
	{
		memset(&m_begin_tm,0,sizeof(struct timeval));
		m_end_tm = m_begin_tm;
		gettimeofday(&m_begin_tm,NULL);
	}
	void CFreqCtl::Sleep(int cnt_per_sec)
	{
		unsigned _count_need_usec = 1000000/cnt_per_sec;
		gettimeofday(&m_end_tm,NULL);
		unsigned _use_usec = (m_end_tm.tv_sec-m_begin_tm.tv_sec)*1000000 + 
						m_end_tm.tv_usec-m_begin_tm.tv_usec;
		if ( _use_usec < _count_need_usec )
		{
			usleep(_count_need_usec - _use_usec);
		}
		gettimeofday(&m_begin_tm,NULL);
	}
}
