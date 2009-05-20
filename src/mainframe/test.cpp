#include <stdio.h>
#include <string>
#include <map>
#include <stack>
#include <signal.h>
#include <pthread.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

typedef struct _node
{
	time_t tm;
	std::map<std::string, std::map<std::string, std::string> > content;
}Node;

std::string strExtensionsPath = "extensions";
std::string strScriptsPath = "scripts";
std::string strFile = "main.lua";
std::string strFunc = "main";

std::stack<int> g_stseq;
std::map<int, Node> g_mapVal;
pthread_mutex_t seqno_lock = PTHREAD_MUTEX_INITIALIZER;

int get_data(lua_State *L, int index, int& num)
{
	if(!lua_isnumber(L, index))
		return -1;
	num = (int)lua_tonumber(L, index);
	return 0;
}

int get_data(lua_State *L, int index, std::string& str)
{
	if(!lua_isstring(L,index))
		return -1;
	size_t len=0;
	const char *p=lua_tolstring(L, index, &len);
	if(p) str = p;
	return 0;
}

int put_data(lua_State *L, int num)
{
	lua_pushnumber(L, num);
	return 0;
}

int put_data(lua_State *L, std::string &str)
{
	lua_pushstring(L, str.c_str());
	return 0;
}

int init()
{
	std::string strTmp = strExtensionsPath + "/?.so;";
	if(getenv("LUA_CPATH"))
		strTmp += getenv("LUA_CPATH");
	strTmp += ";;";
	setenv("LUA_CPATH", strTmp.c_str(),1);
	
	strTmp = strScriptsPath + "/?.lua;";
	if(getenv("LUA_PATH"))
		strTmp += getenv("LUA_PATH");
	strTmp += ";;";
	setenv("LUA_PATH", strTmp.c_str(),1);
	g_stseq.push(1);
	return 0;
}

int clear()
{
	//system("unset LUA_PATH");
	return 0;
}

int get_seqno(lua_State *L)
{
	static int seqno = 1;
	std::string str;
	get_data(L, 1, str);
	int current_seqno = 0;
	pthread_mutex_lock(&seqno_lock);
	if(str == "stack")
	{
		//must have one element
		current_seqno = g_stseq.top();
		g_stseq.pop();
		if(g_stseq.size() ==0)
			g_stseq.push(current_seqno+1);
	}
	else
		current_seqno = seqno ++;
	pthread_mutex_unlock(&seqno_lock);
	put_data(L, current_seqno);
	return 1;
}

int cls_seqno(lua_State *L)
{
	int seqno=-1;
	get_data(L, 1, seqno);
	pthread_mutex_lock(&seqno_lock);
	if(g_mapVal.count(seqno) ==0)
		printf("wrong...seqno=%d\n", seqno);
	else
	{
		g_mapVal.erase(seqno);
		g_stseq.push(seqno);
	}
	pthread_mutex_unlock(&seqno_lock);
	return 0;
}


int get_value(lua_State *L)
{
	int index = -1;
	if(get_data(L, 1, index) || index <=0)
		return 0;
	std::string tmp1, tmp2, tmp3;
	if(get_data(L, 2, tmp1) || get_data(L, 3, tmp2))
		return 0;
	pthread_mutex_lock(&seqno_lock);
	put_data(L, g_mapVal[index].content[tmp1][tmp2]);
	pthread_mutex_unlock(&seqno_lock);
	return 1;
}

int set_value(lua_State *L)
{
	int index = -1;
	if(get_data(L, 1, index) || index <=0)
		return 0;
	std::string tmp1, tmp2, tmp3;
	if(get_data(L, 2, tmp1) || get_data(L, 3, tmp2) || get_data(L, 4, tmp3))
		return 0;

	pthread_mutex_lock(&seqno_lock);
	if(g_mapVal.count(index) ==0)
		g_mapVal[index].tm = time(NULL);
	g_mapVal[index].content[tmp1][tmp2] = tmp3;
	pthread_mutex_unlock(&seqno_lock);
	return 0;
}

int get_message()
{
	//sleep(1);
	//printf("get...message\n");
	return 1;
}

bool bExit = false;
bool bUpdate = false;

void * check_timeout()
{
	time_t cur_tm = time(NULL), pre_tm=cur_tm;
	while(!bExit)
	{
		cur_tm = time(NULL);
		if(cur_tm < pre_tm + 10 )
		{
			sleep(3);
			continue;
		}
		pthread_mutex_lock(&seqno_lock);
		for(std::map<int, Node>::iterator it=g_mapVal.begin(); it!=g_mapVal.end(); it++)
		{
			if(cur_tm < it->second.tm + 10)
				pre_tm = it->second.tm;
			else
				pre_tm = it->second.tm;
		}
		pthread_mutex_unlock(&seqno_lock);
	}
	return 0;
}

void * deal(void *p)
{
	printf("pid=%d enter...\n", getpid());
	while(!bExit)
	{
		printf("start update...\n");
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		lua_register(L, "get_seqno", get_seqno);
		lua_register(L, "cls_seqno", cls_seqno);
		lua_register(L, "get_value", get_value);
		lua_register(L, "set_value", set_value);
		
		std::string strTmp = strScriptsPath + "/" + strFile;
		if(luaL_loadfile(L, strTmp.c_str()) || lua_pcall(L, 0, 0, 0))
		{
			const char *p = luaL_checkstring(L,1);
			printf("load [%s, %s] error...\n", strTmp.c_str(), p);
			break;
		}
		
		lua_getglobal(L, "init");
		lua_pcall(L, 0, 0, 0);

		bUpdate = false;
		while(!bUpdate && get_message())
		{
			lua_settop(L,0);
			lua_getglobal(L, strFunc.c_str());
			if(lua_pcall(L, 0, 0, 0))
			{
				const char *p = luaL_checkstring(L,1);
				printf("lua_pcall [function:%s, %s] error...\n", strFunc.c_str(), p);  
				sleep(3);
				break;
			}
		}
		
		lua_getglobal(L, "clear");
		lua_pcall(L, 0, 0, 0);

		lua_close(L);
	}
	printf("pid=%d exit...\n", getpid());
	return 0;
}

void signal_handle(int signo)
{
	switch(signo)
	{
		case SIGTERM:
			bExit = true;
		case SIGUSR1:
			bUpdate = true;
		default:;
	}
}

int main( int argc, char *argv[])
{
	signal(SIGUSR1, signal_handle);
	signal(SIGTERM, signal_handle);

	switch(argc)
	{
		case 5:
			strExtensionsPath = argv[4];
		case 4:
			strScriptsPath = argv[3];
		case 3:
			strFunc = argv[2];
		case 2:
			strFile = argv[1];
			break;

		default:;
	}

	init();

	pthread_t tid[1000];
	int k=1;
	for(int i=0;i<k;i++)
		pthread_create(&tid[i], 0, deal, NULL);
	for(int i=0;i<k;i++)
		pthread_join(tid[i], NULL);
	//asleep(3);
	//clear();
}
