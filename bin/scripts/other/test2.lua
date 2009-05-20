#! /usr/bin/lua

a={}
--a[1]={"a", "b"}
--a[2]={"a1", "b1"}

a = {"one", "two", "three"}
for i, v in ipairs(a) do
		print(i, v)
end

