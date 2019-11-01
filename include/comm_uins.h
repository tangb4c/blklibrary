#ifndef __COMM_UIN_H_8000__
#define __COMM_UIN_H_8000__
#include <stdio.h>
#include <set>
#include <string>
#include "comm_macro.h"
#include "comm_base.h"
namespace comm
{
    using namespace std;
	class CUin:public CCommBase
	{
        public:
            CUin():m_fp(NULL){}
            CUin(int fd);
            inline CUin(const char* uinfile):m_fp(NULL){SetUinFile(uinfile);}
            inline CUin(string& uinfile):m_fp(NULL){SetUinFile(uinfile.c_str());}
            ~CUin();
		public:
            int SetUinFile(const char* uinfile);
            inline int SetUinFile(string& uinfile){return SetUinFile(uinfile.c_str());}
            
            //从文件<每行一个uin>中获取Uin,并对uin进行有效性判断
            //返回RTN_ERROR，表示文件数据读完，无数据
            //返回RTN_SUCCESS,表示读取成功
            bool GetNextUin(unsigned &uin);
            
            //返回RTN_ERROR，表示文件数据读完,无数据
            //返回RTN_SUCCESS,表示读取成功
            bool BatchGetUins(set<unsigned>& sets, const unsigned needcnt);
        protected:
            string m_uinfile;
            FILE* m_fp;
            char m_buf[64];
	};
}
#endif

