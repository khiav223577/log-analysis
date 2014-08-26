class String
	def extract_escape_symbol
		return self.gsub(/\\(?:\\|n|r|t|"|')/){|t| #處理config中跳脫字元
			case t
			when '\r'   ; "\r"
			when '\n'   ; "\n"
			when '\t'   ; "\t"
			when "\\\\" ; "\\"
			when "\\\"" ; "\""
			when "\\\'" ; "\'"
			end
		}
	end
end
require './Boolean_parser.rb'
require './Config_parser.rb'

if !$IN_C_CODE
	a = %{
		#MAX_SIZE[511] String
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
	a = %{
		String[\\n] Action
		#if Action == "allow" or (Action == "deny" and Action == "deny")
			Int[10] string1
		#elsif Action == "alert"
			String[,] string1
		#else
			String[,] string5
		#end
	}
	a = %{
		String[,] IP_Protocol
		String[,] Action
		Char formatTest{
		    :input  => false, //consume
		    :input  => false, //consume
		    :output => false, //store
		    :output => false, //store
		}
	}
	$config = Config_Parser.new
	$config.parse(a)
	
	exit
end
$config = Config_Parser.new


def read_config(config_path)
	$result_buffer = $config.parse(IO.read(config_path))
end
$hash = {}
def register_hash(keys)
	keys.each_with_index{|key, idx| $hash[key] = idx}
end
def return_string
	data = $result_buffer.shift #[Type, format, vocabulary, hash]
	return nil if data == nil
	data[0] = ($hash[data[0]] || 0) #0 is invalid
	return data
end














