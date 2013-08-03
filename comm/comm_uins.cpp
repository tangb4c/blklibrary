#include "comm_uins.h"
namespace comm
{
	CUin::CUin(int fd)
	{
 	    if((m_fp = fdopen(fd, "r")) == NULL)
	    {
            THROW_EX("open file failed.%s", strerror(errno));
	    }
	}
	CUin::~CUin()
	{
		if(m_fp != NULL)
		{
			fclose(m_fp);
		}
	}
	int CUin::SetUinFile(const char* file)
	{
		if(m_fp != NULL)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
		m_uinfile = file;
 	    if((m_fp = fopen(file, "r")) == NULL)
	    {
            THROW_EX("open file failed.%s", strerror(errno));
	    }
		return RTN_SUCCESS;
	}
	bool CUin::GetNextUin(unsigned &uin)
	{
	    uin = 0;
	    while(uin < 10001 && fgets(m_buf, sizeof(m_buf), m_fp) != NULL)
	    {
	        uin = atoi(m_buf);
	    }
		return uin >= 10001;
	}
	bool CUin::BatchGetUins(set<unsigned>& sets, const unsigned needcnt)
	{
		unsigned _uin = 0;
		sets.clear();
		while(sets.size() < needcnt && GetNextUin(_uin))
		{
			sets.insert(_uin);
		}
		return sets.size() != 0;
	}
}
