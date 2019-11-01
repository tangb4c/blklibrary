/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 *
 * comm_speed_stat.h
 *
 * Description:
 *
 * --------------------
 * 2013/5/8, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_SPEED_STAT_H__050813__
#define __COMM_SPEED_STAT_H__050813__
#include "comm_base.h"
namespace comm
{
namespace utility
{
/// <summary>
/// Used for sync process, not any lock
/// </summary>
/// <returns></returns>
class CSpeedStat
{
public:
   CSpeedStat();
public:
   void Reset();
   void AddCount(unsigned count);
   unsigned GetAvgSpeed();
protected:
   static const unsigned ARRSIZE = 6;
   time_t m_lasttime;
   unsigned m_total;
   unsigned m_data[ARRSIZE];
};

}
}

#endif /* __COMM_SPEED_STAT_H__050813__ */
