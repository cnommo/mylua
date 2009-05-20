#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <aio.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef enum _level_t {debug=0, info, error, none} level_t;

#define BUFSIZE 1024*16

typedef struct _fdate
{
	int m_fd;
	int m_maxsize;
	int m_cursize;
	int m_num;
	int m_level;
	int m_flag;
	char *m_data;
	int m_dlen;
	int m_dpos;
	std::string fname;
	struct aiocb m_acb;
	_fdate(const char *pfile, int isize, int inum, int ilevel, int iflag)
	{
		fname = pfile;
		m_maxsize = isize;
		m_cursize =0;
		m_num = inum;
		m_level = ilevel;
		m_flag = iflag;
		m_data = new char[BUFSIZE];
		m_dpos=0;
	};
	~_fdate()
	{
		delete[] m_data;
	};
	int _open()
	{
		if(m_flag & 1) m_flag = O_APPEND;
			m_flag |= O_RDWR;
		int ret = ::open(fname.c_str(), m_flag);
		if(ret<0)
		{
			m_cursize =0;
			ret = ::open(fname.c_str(), m_flag | O_CREAT | O_EXCL);
		}	
		if(ret >=0)
		{
			m_fd = ret;
			memset(m_data, 0x0, sizeof(m_data));
			memset(&m_acb, 0x0, sizeof(m_acb));
			m_acb.aio_fildes = m_fd;
			m_acb.aio_buf = m_data;
			m_acb.aio_nbytes = 0;
			m_dpos=0;
			aio_write(&m_acb);
		}
		return ret;
	};
	int _close()
	{
		m_cursize =0;
		return ::close(m_fd);
	}
}fdate, *pfdate;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static int lopen(lua_State *L)
{
	const char *p=luaL_checkstring(L,1);
	int size = luaL_checkint(L,2);
	int num = luaL_checkint(L,3);
	int level = luaL_checkint(L,4);
	int flag = luaL_checkint(L,5);
	
	if(level < debug || level > none)
		level = debug;
	fdate* fp = new fdate(p, size, num, level, flag);
	int ret = fp->_open();
	std::string strbuf= ret <0 ? strerror(errno) : "OK";
	lua_pushinteger(L, ret);
	lua_pushstring(L, strbuf.c_str());
	lua_pushlightuserdata(L, fp);
	return 3;
}

static int lclose(lua_State *L)
{
	printf("in close...\n");
	fdate *fp = (fdate*)lua_touserdata(L,1);
	aio_cancel(fp->m_fd, NULL);
	fp->_close();
	delete fp;
	return 0;
}
static int lgetlevel(lua_State *L)
{
	fdate *fp = (fdate*)lua_touserdata(L,1);
	lua_pushinteger(L, fp->m_level);
	return 1;
}

static int lsetlevel(lua_State *L)
{
	fdate *fp = (fdate*)lua_touserdata(L,1);
	int level = luaL_checkint(L,2);
	if(level >= debug && level <= none)
		fp->m_level = level;
	return 0;
}

#define myitoa(buf,k) \
	(memset(buf,0x0,sizeof(buf)), snprintf(buf,sizeof(buf),"%d",k), buf)

static int llog(lua_State *L)
{
	fdate *fp = (fdate*)lua_touserdata(L,1);
	int level = luaL_checkint(L, 2);
	const char *p = luaL_checkstring(L, 3);

	int ret =0;
	std::string strbuf = "OK";
	
	ret = aio_error(&(fp->m_acb));
	if(ret==0)
	{
		ret = aio_return(&(fp->m_acb));
		if(ret <0)
			strbuf = strerror(errno);
		else
		{
			printf("enter OK...%d,%d\n", fp->m_dpos, ret);
			int len = fp->m_dpos - ret;
			memcpy(fp->m_data+ret, fp->m_data, len);
			if(len+strlen(p)<BUFSIZE)
			{
				memcpy(fp->m_data+len, p, strlen(p));
				fp->m_dpos = len + strlen(p);
				fp->m_acb.aio_nbytes = fp->m_dpos;
				aio_write(&(fp->m_acb));
			}
			else
			{
				fp->m_dpos = len;
				fp->m_acb.aio_nbytes = fp->m_dpos;
				aio_write(&(fp->m_acb));
				ret =-1;
				strbuf = "too long log oh";
				printf("error...too long log\n");
			}
		}
	}
	else if(ret == EINPROGRESS)
	{
		ret =0;
		printf("enter EINPROGRESS...%d,%d\n", fp->m_dpos, strlen(p));
		if(fp->m_dpos + 1024*2 > BUFSIZE)
		{
			printf("enter EINPROGRESS(suspend 1s)...%d,%d\n", fp->m_dpos, strlen(p));
			struct timespec tm;
			tm.tv_sec =1;
			tm.tv_nsec = 0;
			struct aiocb *cblist[2]={0};
			cblist[0] = &(fp->m_acb);
			ret = aio_suspend(cblist,1,&tm);
			if(ret <0)
				strbuf = strerror(errno);
		}
		if(ret>=0)
		{
			if(fp->m_dpos + strlen(p) < BUFSIZE)
			{
				memcpy(fp->m_data+fp->m_dpos, p, strlen(p));
				fp->m_dpos += strlen(p);
			}
			else
			{
				printf("error to much log...\n");
				ret = -1;
				strbuf = "too much log";
			}
		}
	}
	else if(ret == ECANCELED)
	{
		printf("enter ECANCEL...%d,%d\n", fp->m_dpos, ret);
		if(fp->m_dpos+strlen(p) < BUFSIZE)
		{
			memcpy(fp->m_data+fp->m_dpos, p, strlen(p));
			fp->m_dpos += strlen(p);
			fp->m_acb.aio_nbytes = fp->m_dpos;
			aio_write(&(fp->m_acb));
		}
		else
		{
			ret =-1; 
			strbuf = "too much log";
		}
	}
	else
		strbuf = strerror(errno);
#if 0
	if(level >= fp->m_level)
	{
		struct stat buf;
		ret = ::fstat(fp->m_fd, &buf);
		if(ret < 0)
			strbuf = strerror(errno);
		else
		{
			fp->m_cursize = buf.st_size;
			if(fp->m_maxsize>0 && fp->m_cursize >= fp->m_maxsize)
			{
				if(fp->m_num >=0)
				{
					printf("move...%d\n", fp->m_maxsize);
					pthread_mutex_lock(&lock);
					ret = ::stat(fp->fname.c_str(), &buf);
					if(ret<0)
						strbuf=strerror(errno);
					else if(buf.st_size > fp->m_maxsize)
					{
						printf("###Need move...\n");
						std::string tmp1,tmp2;
						for(int i=fp->m_num-2; i>=0; i--)
						{
							char szTmp[16]={0};
							tmp1 = fp->fname;
							if(i>0) tmp1 += std::string(".") + myitoa(szTmp, i);
							tmp2 = fp->fname + "." + myitoa(szTmp, i+1);
							rename(tmp1.c_str(), tmp2.c_str());
						}
					}
					pthread_mutex_unlock(&lock);
					fp->_close();
					fp->_open();
				}
				else
				{
					ret = fp->m_num;
					strbuf = "beyond file maxsize";
				}
			}
			ret = ::write(fp->m_fd, p, strlen(p));
			if(ret < 0 )
				strbuf = strerror(errno);
		}
	}
#endif
	lua_pushinteger(L, ret);
	lua_pushstring(L, strbuf.c_str());
	return 2;
}

static const struct luaL_Reg mylib[] =
{
	{"open", lopen},
	{"close", lclose},
	{"setlevel", lsetlevel},
	{"getlevel", lgetlevel},
	{"log", llog},
	{NULL, NULL}
};

LUALIB_API int luaopen_mylib(lua_State *L)
{
	luaL_register(L, "liblog", mylib);
	return 0;
}

#ifdef __cplusplus
}
#endif
