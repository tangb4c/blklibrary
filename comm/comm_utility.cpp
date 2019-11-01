#include <limits.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "comm_utility.h"
using namespace std;
namespace comm
{
namespace utility
{
/// <summary>
/// convert time_t to str. str is store at static char array.
/// </summary>
/// <param name="time"></param>
/// <returns></returns>
const char* GetTimeStr(time_t abs_time)
{
	static char _buf[24];
	strftime(_buf, sizeof _buf, "%T", localtime(&abs_time));
	return _buf;
}
/// <summary>
/// get current time str
/// </summary>
/// <returns></returns>
const char* GetTimeStr()
{
	return GetTimeStr(::time(NULL));
}
const char* GetFullTimeStr(time_t abs_time)
{
	static char _buf[48];
	time_t _value = (unsigned)abs_time; //for 64bit,  abs_time will cause coredump if abs_time is invalid value.
	strftime(_buf, sizeof _buf, "%F %T", localtime(&_value));
	return _buf;
}

unsigned GetLocalAddress(const char *interface)
{
	const int family = PF_INET; //IPV4
	struct ifaddrs *ifap0 = NULL, *ifap = NULL;
	struct sockaddr_in *addr4 = NULL;

	if(getifaddrs(&ifap0))
	{
		THROW_EX("get interface[%s] failed.%m", interface);
	}

	for (ifap = ifap0; ifap != NULL; ifap = ifap->ifa_next)
	{
		if(ifap->ifa_addr == NULL) continue;
		if((ifap->ifa_flags & IFF_UP) == 0) continue;
		if(family != ifap->ifa_addr->sa_family) continue;
		if(strcmp(interface, ifap->ifa_name) != 0) continue;
		addr4 = (struct sockaddr_in *)ifap->ifa_addr;
		break;
	}
	freeifaddrs(ifap0);
	if(addr4)
		return *(unsigned *)&addr4->sin_addr;
	else
		return 0;
}
void GetAllLocalAddress(std::map<unsigned, std::string>& result)
{
	const int family = PF_INET; //IPV4
	struct ifaddrs *ifap0 = NULL, *ifap = NULL;
	struct sockaddr_in *addr4 = NULL;

	if(getifaddrs(&ifap0))
	{
		THROW_EX("getifaddrs failed.%m");
	}
	result.clear();
	for (ifap = ifap0; ifap != NULL; ifap = ifap->ifa_next)
	{
		if(ifap->ifa_addr == NULL) continue;
		if((ifap->ifa_flags & IFF_UP) == 0) continue;
		if(family != ifap->ifa_addr->sa_family) continue;
		addr4 = (struct sockaddr_in *)ifap->ifa_addr;
		result.insert(make_pair<unsigned, string>(*(unsigned *)&addr4->sin_addr, ifap->ifa_name));
	}
	freeifaddrs(ifap0);
}
char* GetIpString(unsigned ip)
{
	struct in_addr _in = {ip};
	return inet_ntoa(_in);
}
unsigned GetIp(const string& ipstr)
{
	return inet_addr(ipstr.c_str());
}
unsigned GetIp(const char *ipstr)
{
	return inet_addr(ipstr);
}
unsigned GetRandom()
{
	static bool inited = false;
	if(likely(inited)) return lrand48();
	inited = true;
	unsigned _seed = 0;
	struct timeval _val;
	gettimeofday(&_val, NULL);
	_seed = (_val.tv_usec << 12) + getpid();
	srand48(_seed);
	return lrand48();
	/*
	int _fd = open("/dev/random", O_RDONLY);
	if(_fd > 0)
	{
		read(_fd, &_seed, sizeof(_seed));
		close(_fd);
		fprintf(stderr, "pid:%d /dev/random init rand seed:%u\n", getpid(), _seed);
	}
	if(!_seed)
	{
		do
		{
			struct timeval _val;
			gettimeofday(&_val, NULL);
			//_seed = (_val.tv_sec << 20) + _val.tv_usec;
			_seed = (_val.tv_usec << 20) + getpid();
		}while (_seed == 0);
		fprintf(stderr, "pid:%d init rand seed:%u\n", getpid(), _seed);
	}
	srand(_seed);
	return rand();
	*/
}
std::string Binary2String(const void *buf, int len)
{
	std::ostringstream _hex;
	const unsigned char *_begin = (const unsigned char *)buf, *_end = _begin + len;
	_hex << std::hex << std::uppercase << std::setfill('0');
	while (_begin < _end)
	{
		_hex << std::setw(2) << static_cast<unsigned>(*_begin++) << " ";
	}
	return _hex.str();
}
std::string ReadLink(const char *link)
{
	std::string _path;
	_path.resize(256);
	int _ret = readlink(link, (char *)_path.data(), _path.capacity());
	if(_ret < 0)
		THROW_CODE_EX(ESystemFailed, "readlink %s failed. %m", link);
	_path.resize(_ret);
	return _path;
}
std::string GetAppFullPathName()
{
	return ReadLink("/proc/self/exe");
}
std::string GetCurrentWorkPath()
{
	return ReadLink("/proc/self/cwd");
}
std::string GetAppPath()
{
	std::string _appname = GetAppFullPathName();
	return _appname.substr(0, _appname.find_last_of('/'));
}

bool IsRunningOfPid(pid_t pid)
{
	char _buf[24];
	snprintf(_buf, sizeof(_buf), "/proc/%d", pid);
	return access(_buf, F_OK) == 0;
}

unsigned long long GetPhysicalMemorySize()
{
	unsigned long long pages = sysconf(_SC_PHYS_PAGES);
	unsigned long long page_size = sysconf(_SC_PAGE_SIZE);
	return pages * page_size;
}

void Mkdir(const std::string& path)
{
	if(path.empty())
	{
		THROW_CODE_EX(EBadParams, "empty path");
	}
	struct stat _stat;
	if(lstat(path.c_str(), &_stat))
	{
		if(errno == ENOENT)
		{
			if(mkdir(path.c_str(), 0777))
				THROW_CODE_EX(ESystemFailed, "mkdir %s failed. %m", path.c_str());
			return;
		}
		else
		{
			THROW_CODE_EX(ESystemFailed, "stat %s failed. %m", path.c_str());
		}
	}
	if((_stat.st_mode & S_IFLNK) == S_IFLNK)
	{
		std::string _link_path = ReadLink(path.c_str());
		return Mkdir(_link_path);
	}
	else if((_stat.st_mode & S_IFDIR) != S_IFDIR)
	{
		THROW_CODE_EX(ESystemFailed, "%s existed, but not directory.", path.c_str());
	}
}
std::string GetAbsolutePath(const std::string& path)
{
	char _abs_path[PATH_MAX] = { 0 };
	if(realpath(path.c_str(), _abs_path) == NULL)
		THROW_CODE_EX(EBadParams, "convert path:%s to absolute path failed. %m\n", path.c_str());
	return std::string(_abs_path);
}
time_t GetBootTime()
{
	FILE *_file = fopen("/proc/uptime", "r");
	if(_file == NULL)
		THROW_CODE_EX(ESystemFailed, "open uptime failed. %m");
	long long _reboot_time;
	int _ret = fscanf(_file, "%lld", &_reboot_time);
	if(_ret != 1)
	{
		fclose(_file);
		THROW_CODE_EX(ESystemFailed, "get time failed.");
	}
	fclose(_file);
	time_t _now = ::time(NULL);
	return _now - _reboot_time;
}
bool GetProcessInfo(pid_t pid, ProcessStat& info)
{
	char _stat_path[32];
	if(pid <= 0)
		snprintf(_stat_path, sizeof(_stat_path), "/proc/self/stat");
	else
		snprintf(_stat_path, sizeof(_stat_path), "/proc/%u/stat", pid);
	FILE *_fp = fopen(_stat_path, "r");
	if(!_fp) return false;
	int _ret = fscanf(_fp, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu",
							&info.pid, info.name, &info.stat, &info.ppid, &info.pgrp,
							&info.sessionid, &info.tty_nr, &info.tpgid, &info.flag, &info.minflt,
							&info.cminflt, &info.majflt, &info.cmajflt, &info.utime, &info.stime);
	fclose(_fp);
	return _ret == 15;
}
void GetProcessTime(pid_t pid, ProcessStat& statinfo)
{
	char _stat_path[32];
	if(pid <= 0)
		snprintf(_stat_path, sizeof(_stat_path), "/proc/self/stat");
	else
		snprintf(_stat_path, sizeof(_stat_path), "/proc/%u/stat", pid);
	FILE *_fp = fopen(_stat_path, "r");
	if(!_fp)
		THROW_CODE_EX(ESystemFailed, "open %s failed.", _stat_path);
	fscanf(_fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
			 &statinfo.utime, &statinfo.stime);
	fclose(_fp);
}

std::string GetProcessNameByPid(pid_t pid)
{
	char _stat_path[64];
	snprintf(_stat_path, sizeof(_stat_path), "/proc/%u/stat", pid);
	FILE *_fp = fopen(_stat_path, "r");
	if(!_fp)
	{
		if(errno == EPERM)
			THROW_CODE_EX(ESystemFailed, "parse %s failed. %m", _stat_path);
		return std::string();
	}
	int _ret = fscanf(_fp, "%*d (%[^)])", _stat_path);
	fclose(_fp);
	if(_ret != 1) return std::string();
	return std::string(_stat_path);
}
}
}
