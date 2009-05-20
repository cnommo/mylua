#include <stdio.h>
#include <dlfcn.h>

typedef int (*FUNC)(int);
int main()
{
	while(1)
	{
		void *handle =dlopen("test.so", RTLD_NOW);
		if(!handle)
		{
			printf("%s\n", dlerror());
			return 0;
		}
		FUNC func = (FUNC)dlsym(handle, "deal");
		//while(1)
		{
			FUNC funcc = (FUNC)dlsym(handle, "deal");
			//printf("res=%d\n",func(10));
			//sleep(1);
		}
		dlclose(handle);
		//sleep(3);
	}
}
