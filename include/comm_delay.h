#ifndef __COMM_DELAY_H_8000__
#define __COMM_DELAY_H_8000__
#include <sys/time.h>
#include "comm_macro.h"
#include "comm_base.h"
namespace comm
{
    using namespace std;
    class CFreqCtl : public CCommBase
    {
        public:
            CFreqCtl();
        public:
            //cnt_per_sec:每秒执行的频率
            void Sleep(int cnt_per_sec);
        protected:
            struct timeval  m_begin_tm;
            struct timeval  m_end_tm;
    };
    //this is blake made.
}
#endif

