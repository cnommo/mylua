#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define MAX_MSG_LEN 1024*64

typedef struct _mtext
{
	unsigned long mlen;
	char mcontent[MAX_MSG_LEN+4];
}mtext_t;

typedef struct _msgbuf
{
	long mtype;
	mtext_t mtext;
	_msgbuf() { memset(&mtext, 0x0, sizeof(mtext)); }
}msgbuf_t;


static int enqueue(lua_State *L)
{
	int key = luaL_checkint(L,1);
	int type = luaL_checkint(L,2);
	int flag = luaL_checkint(L,3);
	
	int ret =0;
	msgbuf_t sMsg;
	int msgid = msgget(key, 0);
	if(msgid < 0)
	{
		msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0644);
		if(msgid <0)
		{
			snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "msgget fail: errno=%d,%s", errno, strerror(errno));
			sMsg.mtext.mlen = strlen(sMsg.mtext.mcontent);
			ret = -1;
		}
	}
	else
	{
		
		if(flag) flag = IPC_NOWAIT;
		ret = msgrcv(msgid, &sMsg, sizeof(mtext_t), type, flag | MSG_NOERROR);
		if(ret < 0)
		{
			snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "msgrcv fail: errno=%d,%s", errno, strerror(errno));
			sMsg.mtext.mlen = strlen(sMsg.mtext.mcontent);
		}
		else if((unsigned)ret < sizeof(long) || (unsigned)ret+4 > sizeof(mtext_t) || 
				(unsigned)ret != sMsg.mtext.mlen+sizeof(long))
		{
			snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "getmsg fail: msg size error(%d)", ret);
			sMsg.mtext.mlen = strlen(sMsg.mtext.mcontent);
			ret = -2;
		}
		else
			ret -= sizeof(long);
	}

	lua_pushinteger(L, ret);
	lua_pushlstring(L, sMsg.mtext.mcontent, sMsg.mtext.mlen);
	return  2;
}

static int dequeue(lua_State *L)
{
	int key = luaL_checkint(L,1);
	const char *p = luaL_checkstring(L,2);
	int type = luaL_checkint(L,3);
	int flag = luaL_checkint(L,4);

	int ret = 0;
	size_t len = strlen(p);
	msgbuf_t sMsg;
	if(len+1 > MAX_MSG_LEN)
	{
		snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "retmag fail: msg size error(%d)", len);
		sMsg.mtext.mlen = strlen(sMsg.mtext.mcontent);
		ret =-2;
	}
	else
	{
		int msgid = msgget(key, 0);
		if(msgid < 0 )
		{
			msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0644);
			if(msgid < 0)
			{
				snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "msgget fail: errno=%d,%s", errno, strerror(errno));
				sMsg.mtext.mlen = strlen(sMsg.mtext.mcontent);
				ret =-1;
			}
		}
		else
		{
			sMsg.mtype = type;
			sMsg.mtext.mlen = len;
			snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "%s", p);
			if(flag) flag = IPC_NOWAIT;
			ret = msgsnd(msgid, &sMsg, len+sizeof(long), flag);
			if(ret < 0)
			{
				snprintf(sMsg.mtext.mcontent, MAX_MSG_LEN, "msgsnd fail: errno=%d,%s", errno, strerror(errno));
				sMsg.mtext.mlen = strlen(sMsg.mtext.mcontent);
			}
		}
	}
	lua_pushinteger(L, ret);
	lua_pushlstring(L, sMsg.mtext.mcontent, sMsg.mtext.mlen);
	return 2;
}

static const struct luaL_Reg mylib[] =
{
	{"enqueue", enqueue},
	{"dequeue", dequeue},
	{NULL, NULL}
};

LUALIB_API int luaopen_mylib(lua_State *L)
{
	luaL_register(L, "libmsg", mylib);
	return 1;
}

#ifdef __cplusplus
}
#endif
