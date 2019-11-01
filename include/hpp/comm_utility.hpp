/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_utility.h
 * 
 * Description:
 * 
 * --------------------
 * 2011-9-27, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_ULTILITY_HPP__092711__
#define __COMM_ULTILITY_HPP__092711__
namespace comm{
namespace utility{
/// <summary>
/// convert container(such as vector, set, list) to string
/// </summary>
/// <param name="ctr"></param>
/// <returns>string</returns>
template<typename T>
std::string ListToString(const T& ctr)
{
	if(ctr.size() == 0)return std::string();
	std::ostringstream oss;
	copy(ctr.begin(), ctr.end(), std::ostream_iterator<typename T::value_type>(oss, ","));
	return oss.str();
}
template<typename Container, typename Operator>
void Split(const std::string& source, const char *delim, Container& out_result, Operator op)
{
   if(source.empty())return;
   if(delim == NULL)
	   THROW_CODE_EX(EBadParams, "delim is null");
   char _buf[source.size() + 1];
   strncpy(_buf, source.c_str(), sizeof(_buf));
   char* _save_token = NULL;
   char* _token = strtok_r(_buf, delim, &_save_token);
   std::insert_iterator<Container> _inserter(out_result, out_result.end());
   while(_token)
   {
	   if(*_token != 0)*_inserter++ = op(_token);
	   _token = strtok_r(_save_token, delim, &_save_token);
   }
}
template<typename Container>
inline void Split(const std::string& source, const char* delim, Container& out_result)
{
   return Split(source, delim, out_result, DoNothing<typename Container::value_type>());
}
template<std::string::value_type b, std::string::value_type e>
std::string Substr(const std::string& str, unsigned ignore_prefix_count = 0)
{
    std::string::const_iterator _begin = std::find_if(str.begin(), str.end(), IsLetter(b, ignore_prefix_count));
    if(_begin != str.end())++_begin;
    std::string::const_iterator _end = std::find_if(_begin, str.end(), IsLetter(e, 0));
    return std::string(_begin, _end);
}
//trim
inline std::string TrimBegin(const std::string& str, char delimit)
{
   return str.substr(str.find_first_not_of(delimit));
}
inline std::string TrimEnd(const std::string& str, char delimit)
{
   return str.substr(0, str.find_last_not_of(delimit) + 1);
}
inline std::string Trim(const std::string& str, char delimit)
{
   std::size_t _bpos = str.find_first_not_of(delimit);
   if(_bpos == std::string::npos)return std::string();
   return str.substr(_bpos, str.find_last_not_of(delimit) - _bpos + 1);
}
template<typename T, size_t N>
inline size_t SizeOf(const T(&)[N]){return N;}//利用编译器得知数组大小

template<typename T, size_t N>
inline T* EndOf(T (&arr)[N]){return arr + N;}
template<typename T, size_t N>
inline const T* EndOf(const T (&arr)[N]){return arr + N;}
template<typename T>
inline T* EndOf(T (&arr)[0]){return arr;}
template<typename T>
inline const T* EndOf(const T (&arr)[0]){return arr;}

}
}
#endif /* __COMM_ULTILITY_HPP__092711__ */ 
