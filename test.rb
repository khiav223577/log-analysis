if !$IN_C_CODE
	a = %{
		#DROP_AFTER[,] Int
		#DROP[,]
		Date[MMM d HH:mm:ss ] time1
		String[ ] string1
		Int[10] Domain
		IPv4[.] Destination_address
	}
	for line in a.split("\n").delete_if{|s| s == ""}
		case
		when line =~ /#DROP_AFTER\[(.*)\] \s*(\w+)/
		when line =~ /Date\[(.*)\] \s*(\w+)/
		when line =~ /String\[(.*)\] \s*(\w+)/
		when line =~ /Int\[(.*)\] \s*(\w+)/
		when line =~ /IPv4\[(.*)\] \s*(\w+)/
		when line =~ /#DROP\[(.*)\]/
		end
		p "=============================="
		p line
		p [$1,$2,$3]
		
	end
	exit
end
$aaa = ["Date[MMM d HH:mm:ss ] time1;","String:[ ] string1;"]
def read_config(config_path)
	File.open(config_path, 'r'){|f| $buffer = f.read }
	$buffer = $buffer.split("\n").delete_if{|s| s == ""}
end
def return_string
	return [$aaa.shift,"123"]
end
class Config_Parser
	def initialize
	end
	def read_input(path)
		File.open(config_path, 'r'){|f| @buffer = f.read }
		for line in @buffer.split("\n").delete_if{|s| s == ""}
			case
			when line =~ /^\s*#define\s+(\w*)\[(.*)\]/
			end
		end

	end
	def shift_result
		return @buffer.shift
	end
end

$config = Config_Parser.new













