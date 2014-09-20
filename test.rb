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
require 'date'
if !$IN_C_CODE
  a = %{
    #MAX_SIZE[511] String
    #DROP_AFTER[,] Int
    #DROP_AFTER[~] IPv4
    DROP[@]      //123
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
  @config = ConfigParser.new
  p @config.parse(a)
  exit
end

class ConfigReaderInterface
  @result_counter = 0
  @result_buffer = nil
  @buffer_symbol_mappings = nil
  @config = ConfigParser.new
  @hash = {}
  def self.read_config(config_path)
    result = @config.parse(IO.read(config_path))
    @result_buffer = result[:buffer]
    @buffer_symbol_mappings = result[:buffer_symbol_mappings]
  end
  def self.register_hash(keys)
    keys.each_with_index{|key, idx| @hash[key] = idx}
  end
  def self.get_next_result
    data = @result_buffer[@result_counter] #[Type, format, vocabulary, hash]
    @result_counter += 1
    return nil if data == nil
    data[0] = (@hash[data[0]] || 0) #0 is invalid
    return data
  end
#---------------------------------
#  ACCESS
#---------------------------------
  def self.get_buffer_index(variable_name)
    index = @buffer_symbol_mappings[variable_name]
    p "variable: #{variable_name} not defined." if index == nil
    return index
  end
end
class QueryInterface
  def self.parseIP(addr)
    array = addr.split(".")
    return nil if array.size != 4
    array.map!{|s|
      next (0..255).to_a if s == "*"
      next [s.to_i]
    }
    size_xsum = array[0].size * array[1].size * array[2].size * array[3].size
    return nil if size_xsum > 1000000 or size_xsum <= 0
    output = Array.new(size_xsum)
    array[0].each{|a| array[1].each{|b| array[2].each{|c| array[3].each{|d| 
      output[size_xsum -= 1] = ((a << 24) | (b << 16) | (c << 8) | (d << 0))
    }}}}
    return [size_xsum, output]
  end
  def self.showIP(array)
    return array.map{|s| (s.class == Array ? "*" : s)}.join(".")
  end
  def self.parseDate(date)
    date_time = DateTime.parse(date)
    return nil if date_time.year > 2037 || date_time.year < 1970 #prevent overflow?
    return [date_time.year, date_time.month, date_time.day, date_time.hour, date_time.minute, date_time.second]
  end
  def self.showDate(array)
    DateTime.new(*array).strftime("%Y-%m-%d %H:%M:%S")
  end
public
  def self.wait_query
    loop do
      print "Input your query: "
      inputArr = $stdin.gets.chomp.split(",")
      size = inputArr.size
      if size == 1  
        case inputArr[0]
        when "exit" ; break
        when "help" ; next puts "example: 192.168.0.120, 192.168.*.*, 2012/7/12 12:01:03, 2012/7/15 19:00:00, 300"
        end
      end
      if size == 5
        outputArr = []
        outputArr[0] = parseIP(inputArr[0])
        outputArr[1] = parseIP(inputArr[1])
        outputArr[2] = parseDate(inputArr[2])
        outputArr[3] = parseDate(inputArr[3])
        outputArr[4] = inputArr[4].to_i
        next puts "Format Error: #{inputArr[0]} is not a IP address" if outputArr[0] == nil
        next puts "Format Error: #{inputArr[1]} is not a IP address" if outputArr[1] == nil
        next puts "Format Error: #{inputArr[2]} is not a Date" if outputArr[2] == nil
        next puts "Format Error: #{inputArr[3]} is not a Date" if outputArr[3] == nil
        next puts "start time should earlier than end time" if inputArr[2] > inputArr[3] 
        next puts "time span should not lower than 1." if outputArr[4] <= 0 || outputArr[4] > 100000000
        print "Query the traffice of IP #{showIP(outputArr[0])} against all #{showIP(outputArr[1])} "
        print "from #{showDate(outputArr[2])} to #{showDate(outputArr[3])} for evey #{outputArr[4]} second(s)...\n"
        return outputArr
      end
      puts "Unown input. Please try again. inputArr = #{inputArr}"
    end
    return nil
  end
end
#QueryInterface.wait_query
##192.168.0.120, 192.168.*.*, 2012/7/12 12:01:03, 2012/7/15 19:00:00, 300












