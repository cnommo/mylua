#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "base64.h"
#include "hex.h"

static int ret_result(lua_State *L, int ret, char *msg=NULL)
{
	lua_pushinteger(L, ret);
	if(msg == NULL)
	{
		if(ret ==0)
			lua_pushstring(L, "fail");
		else
			lua_pushstring(L, "ok");
	}
	else
	{
		lua_pushstring(L, msg);
		free(msg);
	}
	return 2;
}

static int encode_base64(lua_State *L)
{
	const char *input = luaL_checkstring(L,-1);
	int iLen = strlen(input)/2*3;
	char *output= (char *)malloc(iLen);
	bzero(output, iLen);
	int ret = Base64_Encode((unsigned char *)input, strlen(input), output, iLen, &iLen);
	return ret_result(L, ret, output);
}

static int decode_base64(lua_State *L)
{
	const char *input = luaL_checkstring(L,-1);
	int iLen = strlen(input)/2*3;
	unsigned char *output= (unsigned char *)malloc(iLen);
	bzero(output, iLen);
	int ret = Base64_Decode(input, strlen(input), output, iLen, &iLen);
	return ret_result(L, ret, (char *)output);
}


static int encode_hex(lua_State *L)
{
	const char *input = luaL_checkstring(L,-1);
	char *output= NULL;
	int ret = HexString_Encode(input, &output);
	return ret_result(L, ret, output);
}

static int decode_hex(lua_State *L)
{
	const char *input = luaL_checkstring(L,-1);
	char *output= NULL;
	int ret = HexString_Decode(input, &output);
	return ret_result(L, ret, output);
}

static int encode_string(lua_State *L)
{
	const char *input = luaL_checkstring(L,-1);
	char *output= NULL;
	int ret = String_Encode(input, &output);
	return ret_result(L, ret, output);
}

static int decode_string(lua_State *L)
{
	const char *input = luaL_checkstring(L,-1);
	char *output= NULL;
	int ret = String_Decode(input, &output);
	return ret_result(L, ret, output);
}

static const struct luaL_Reg mylib[] =
{
	{"encode_base64", 	encode_base64},
	{"decode_base64", 	decode_base64},
	{"encode_hex",		encode_hex},
	{"decode_hex", 		decode_hex},
	{"encode_string", 	encode_string},
	{"decode_string", 	decode_string},
	{NULL, NULL}
};

LUALIB_API int luaopen_mylib(lua_State *L)
{
	luaL_register(L, "libtools", mylib);
	return 1;
}

#ifdef __cplusplus
}
#endif
