require './Boolean_parser.rb'
require './Config_parser.rb'

if !$IN_C_CODE
	a = %{
		#DROP_AFTER[,] Int
		#DROP_AFTER[~] IPv4
		DROP[@]  		//123
		Date[MMM d HH:mm:ss ] time1
		String[ ] string1
		//////////////////
		#if string1 == "I'm not comment" //"//\/\/" comment here
		  //DROP[@] //I'm comment. "///" test
		#end
		//////////////////
		Int[10] Domain
		IPv4[.] Destination_address
		#if Action == "allow" or Action == "deny"
			Int[10] Bytes
		#elsif Action == "alert"
			String[,] string2
		#end
	}
	$config = Config_Parser.new
	$config.parse(a)
	
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


$config = Config_Parser.new













