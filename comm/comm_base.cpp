#include "comm_base.h"
namespace comm
{
	const char* GetCurrentTimestring()
	{
		 static char _buf[24];
		 time_t _time = ::time(NULL);
		 strftime(_buf, sizeof _buf, "%T", localtime(&_time));
		 return _buf;
	}
	 CCommException::CCommException(const char* fmt...) throw():m_errcode(-1)
	 {
	 	/*
		//栈的信息
		void * _array[15];
		int _size = backtrace(_array, 15);
		char ** _symbols = backtrace_symbols(_array, _size);
		m_errmsg.reserve(CCommException::MAXSIZE);
		for (int i = 0; i < _size; i++)
		{
			m_errmsg.append(_symbols[i]);
			m_errmsg.append("\n");
		}
		free(_symbols);
		*/
	 	va_list ap;
		va_start(ap, fmt);
	 	AppendMsg(fmt, ap);
		va_end(ap);
	 }

	 CCommException::CCommException(int errcode, const char* fmt...) throw():m_errcode(errcode)
	 {
	 	/*
		 //栈的信息
		 void * _array[15];
		 int _size = backtrace(_array, 15);
		 char ** _symbols = backtrace_symbols(_array, _size);
		 m_errmsg.reserve(CCommException::MAXSIZE);
		 for (int i = 0; i < _size; i++)
		 {
			 m_errmsg.append(_symbols[i]);
			 m_errmsg.append("\n");
		 }
		 free(_symbols);
		 */ 
		 va_list ap;
		 va_start(ap, fmt);
		 AppendMsg(fmt, ap);
		 va_end(ap);
	 }
	 const char* CCommException::what() const throw()
	 {
	 	return m_errmsg.c_str();
	 }
	 std::string& CCommException::what_string()  throw()
	 {
		 return m_errmsg;
	 }
	 void CCommException::AppendMsg(const char* fmt, va_list args) throw()
	 {
	 	int _size = m_errmsg.size();
		if(_size < 0)_size = 0;
		int _newsize = _size > 256? 2 * _size: MAXSIZE;

		m_errmsg.resize(_newsize);
	 	_size += vsnprintf((char*)m_errmsg.data() + _size,  _newsize - _size, fmt, args);
		m_errmsg.resize(_size);
	 }
	 void CCommException::Append(const char* fmt...) throw()
	 {
	 	va_list ap;
		va_start(ap, fmt);
		AppendMsg(fmt, ap);
		va_end(ap);
	 }

}
