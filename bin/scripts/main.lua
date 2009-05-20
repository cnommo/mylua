#!/usr/bin/lua

MyConf=
{
	MaxSeqNo=10000,
	MaxNum=1000,
	TimeOut=10,
	AlarmPara="",
	AnsMessage=""
}


require "module.pccpdc"
require "module.tes2"
require "log"
require "message"
require "tools"
--assert(package.loadlib("extensions/message_1_2.so", "luaopen_message"))()

--table3[1] = table1
--table3[2] = table2
	
function checksum(n,...)
	for i=1,n+1 do
		command[i] = arg[i]
		print(arg[i])
	end
	return command
end

function run1(key)
	local k={}
	k={as="as", cd="sss"}
	for key, value in pairs(k) do
		print(key, value);
	end
	--return "&"..key.."="..get_value(key)
end

function main()
	local seqno= get_seqno("stack")
	local head = "SeqNoPP="..seqno
	local k=head
	set_value(seqno, "init", "good", "sex")
	set_value(seqno, "init", "good1", "sex1")
	set_value(seqno, "init", "good2", "sex2")
	set_value(seqno, "init", "good3", "sex3")
	--run1("10")
	for i=1,1 do
		local g=get_value(seqno, "init", "good")
		local g1=get_value(seqno, "init", "good1")
		local g2=get_value(seqno, "init", "good2")
		local g3=get_value(seqno, "init", "good3")
		k=head.."&g="..g.."&g1="..g1.."&g2="..g2.."&g3="..g3.."&None=none&"
		k=k..k..k
		k=k..k..k
		k=k..k..k
		k=k..k..k
		k=k..k..k
		--pccpdc.topccpdc(seqno)
		--print(type(testlog.mylog))
		--s=liblog.log();
		print(s)
		--print(libmsg)
		
		print(k)
		ret,res = libtools.encode_base64(k)
		print(res)
		ret,res = libtools.decode_base64(res)
		print(res)

		ret,res = libtools.encode_hex(k)
		print(res)
		ret,res = libtools.decode_hex(res)
		print(res)
		
		ret,res = libtools.encode_string(k)
		print(res)
		ret,res = libtools.decode_string(res)
		print(res)

		ret,res = libtools.decode_base64(res)
		if(ret) then
			print(res)
		end

		a=0
		c=0
		a, b = libmsg.dequeue(10039, k, 3, 1);
		--sleep(10)
		c, d = libmsg.enqueue(10039, 0, 1);
		--print("---");
		
		if(a<0) then
			print(a,b)
		end
		
		if(c<0) then
			print(c,d)
		end
		--print(a,c)
		--print(c,d)
		--print("---");
		--print(package.cpath);
		--if(seqno % 100 ==0) then
		--	print(k)
		--end
	end
	cls_seqno(seqno)
end

function my()
	print(table3[2][2])
	return table3
end
