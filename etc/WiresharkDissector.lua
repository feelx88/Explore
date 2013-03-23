-- load the udp.port table
udp_table = DissectorTable.get("udp.port")
-- declare our protocol
if explore_proto == nil then
	explore_proto = Proto("explore","Explore Protocol")
else
	udp_table:remove(6556,explore_proto)
end
-- create a function to dissect it
function explore_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "EXPLORE"
    local subtree = tree:add(trivial_proto,buffer(),"Explore Protocol Data")
    subtree:add(buffer(0,1),"Packet type: " .. buffer(0,1):string())
    subtree:add(buffer(1,1),"Connection ID: " .. buffer(1,1):uint())
	subtree:add(buffer(2,1),"Sequence counter: " .. buffer(2,1):uint())
	subtree:add(buffer(3,4),"UID: " .. buffer(3,4):uint())
	subtree:add(buffer(7,1),"ActionID: " .. buffer(7,1):uint())
	subtree:add(buffer(8,1),"TypeID: " .. buffer(8,1):uint())
	
	local bodyLength = buffer(9,4):uint()
	subtree:add(buffer(9,4),"Body length: " .. bodyLength)
	
	if( bodyLength > 0 ) then
		subtree:add(buffer(13),"Data: " .. buffer(13):string())
	end
	
	return nil
end
-- register our protocol to handle udp port 6556
udp_table:add(6556,explore_proto)