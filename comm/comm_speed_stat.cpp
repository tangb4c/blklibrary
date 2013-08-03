#include "comm_speed_stat.h"
namespace comm
{
namespace utility
{
CSpeedStat::CSpeedStat():m_lasttime(0), m_total(0)
{
   memset(m_data, 0, sizeof(m_data));
}
void CSpeedStat::Reset()
{
	m_lasttime = m_total = 0;
   memset(m_data, 0, sizeof(m_data));
}
void CSpeedStat::AddCount(unsigned count)
{
	time_t _now = time(NULL);
	int _duration = _now - m_lasttime;
	if(!_duration)
	{
		m_data[_now % ARRSIZE] += count;
		return;
	}

	m_total += m_data[m_lasttime % ARRSIZE];
	m_total -= m_data[_now % ARRSIZE];
	m_data[_now % ARRSIZE] = count;
	if(_duration == 1)
	{
		m_lasttime = _now;
	}
	else if(_duration < int(ARRSIZE)) // duration more than 1
	{
		++m_lasttime;
		do
		{
			m_total -= m_data[m_lasttime % ARRSIZE];
			m_data[m_lasttime % ARRSIZE] = 0;
		}while(++m_lasttime < _now);
	}
	else
	{
		memset(m_data, 0, sizeof(m_data));
		m_data[_now % ARRSIZE] = m_total = count;
		m_lasttime = _now;
	}
}

unsigned CSpeedStat::GetAvgSpeed()
{
	AddCount(0); //update g_total
	return m_total / (ARRSIZE - 1);
	/*
	time_t _now = time(NULL);
	unsigned _total = 0, _index_count = 0;
	unsigned _endindex = m_lasttime % ARRSIZE;
	if(_now != m_lasttime)
	{
		++_index_count;
		_total = m_data[_endindex];
	}

	unsigned _index = ++_now % ARRSIZE;
	for (; _index != _endindex; _index = ++_now % ARRSIZE)
	{
		++_index_count;
		_total += m_data[_index];
	}
	return _index_count ? _total / _index_count : 0;
	*/
}

}
}
