#!/usr/bin/lua

table1 ={0,1,2,3}
table2 ={0,"asasasa",2,3}
table3={}
table3[1] = table1
table3[2] = table2

command= {}
function checksum(n,...)
	for i=1,n+1 do
		command[i] = arg[i]
		print(arg[i])
	end
	return command
end

function my()
	print(table3[2][2])
	return table3
end

print "hello,world"
s=my()
print(s[1][3])
