#ifdef __cplusplus
extern "C"
{
#endif


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


static int example(lua_State *L)
{
	//your codes
	return 0;
}

static const struct luaL_Reg mylib[] =
{
	{"my_example", example},
	{NULL, NULL}
};

LUALIB_API int luaopen_mylib(lua_State *L)
{
	luaL_register(L, "testlib", mylib);
	return 1;
}

#ifdef __cplusplus
}
#endif
