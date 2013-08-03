#include <strings.h>
#include "comm_config_helper.h"
namespace comm
{
	bool CConfigHelper::ExistItem(const char * prefix)throw(CCommException)
	{
		return  !(*this)[prefix].empty();
	}
	void CConfigHelper::GetIPandPort(const char * prefix, int posindex, std::string & out_ip, unsigned & port, char delimiter)throw(CCommException)
	{
		snprintf(m_buf, sizeof(m_buf), "%s%d", prefix, posindex);
		std::string _s = (*this)[m_buf];
		if(_s.empty())
		{
			THROW_EX("%s not exist!", m_buf);
		}
		unsigned _pos = _s.find(delimiter);
		if(_pos == std::string::npos)
		{
			THROW_EX("%c not exist!", delimiter);
		}

		out_ip.assign(_s, 0, _pos);
		port = atoi(&_s[_pos+1]);
		if(out_ip.empty() || port < 1 || port > 65535)
		{
			THROW_EX("Invalid content. ip:%s, port:%u", out_ip.c_str(), port);
		}
	}

	std::string& CConfigHelper::GetString(const char * prefix, int posindex)throw(CCommException)
	{
		snprintf(m_buf, sizeof(m_buf), "%s%d", prefix, posindex);
		return GetString(m_buf);
	}
	std::string& CConfigHelper::GetString(const char * prefix)throw(CCommException)
	{
		if((*this)[prefix].empty())
		{
			THROW_EX("%s not exist.", prefix);
		}
		return (*this)[prefix];
	}
	int CConfigHelper::GetInt(const char * prefix, int posindex)throw(CCommException)
	{
		snprintf(m_buf, sizeof(m_buf), "%s%d", prefix, posindex);
		if((*this)[m_buf].empty())
		{
			THROW_EX("%s not exist.", m_buf);
		}

		return atoi((*this)[m_buf].c_str());
	}

	int CConfigHelper::GetInt(const char* prefix)throw(CCommException)
	{
		if(this->operator [](prefix).empty())
		{
			THROW_EX("%s not exist.", prefix);
		}

		return atoi((*this)[prefix].c_str());
	}
	
	bool CConfigHelper::GetBoolean(const char* prefix, int posindex)throw(CCommException)
	{
		snprintf(m_buf, sizeof(m_buf), "%s%d", prefix, posindex);
		return GetBoolean(m_buf);
	}
	bool CConfigHelper::GetBoolean(const char* prefix)throw(CCommException)
	{
		string _value = this->operator [](prefix);
		if(_value.empty())
		{
			THROW_EX("%s not exist.", prefix);
		}
		if(strcasecmp(_value.c_str(), "true") == 0)
			return true;
		else if(strcasecmp(_value.c_str(), "false") == 0)
			return false;

		THROW_EX("value:%s is not boolean.", _value.c_str());
	}
	void CConfigHelper::SplitString(const char* prefix, int posindex, vector<string>& out_sp, const char* delimiter)throw(CCommException)
	{
		snprintf(m_buf, sizeof(m_buf), "%s%d", prefix, posindex);
		SplitString(m_buf, out_sp, delimiter);
	}
	void CConfigHelper::SplitString(const char* prefix, vector<string>& out_sp, const char* delimiter)throw(CCommException)
	{
		std::string _s = (*this)[prefix];
		if(_s.empty())
		{
			THROW_EX("%s not exist!", prefix);
		}
		char _buf[_s.size() + 4];
		strncpy(_buf, _s.c_str(), sizeof(_buf));
		char* _str = _buf;
		while(char* token = strtok_r(_str, delimiter, &_str))
		{
			out_sp.push_back(token);
		}
		/*-------------------------------------------
		string::size_type _pos_begin(0), _pos_end;
		do
		{
			_pos_end = _s.find(delimiter, _pos_begin);
			out_sp.push_back(_s.substr(_pos_begin, _pos_end - _pos_begin));
			_pos_begin = _pos_end + 1;
		}while(_pos_end != string::npos);
		-------------------------------------------*/
	}
	template<>
	bool CConfigHelper::Get(const char* prefix)throw(CCommException)
	{
		std::string _s = (*this)[prefix];
		if(_s.empty())
		{
			THROW_EX("%s not exist!", prefix);
		}
		if(strcasecmp(_s.c_str(), "true") == 0)
			return true;
		if(strcasecmp(_s.c_str(), "false") == 0)
			return false;
		THROW_EX("%s 's value:%s not a valid boolean string!", prefix, _s.c_str());
	}
}
