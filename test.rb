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
$LOAD_PATH.unshift File.expand_path('lib/ruby-1.9.1', __FILE__)
require './boolean_parser.rb'
require './config_parser.rb'
#require 'rbconfig'
require 'date'
if !$IN_C_CODE and false
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
    @result_counter = 0
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
  @current_input_string = ""
  @current_query_string = ""
  def self.current_query_string ; return @current_query_string ; end
  def self.current_input_string ; return @current_input_string ; end
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
    counter = -1
    array[0].each{|a| array[1].each{|b| array[2].each{|c| array[3].each{|d| 
      output[counter += 1] = ((a << 24) | (b << 16) | (c << 8) | (d << 0))
    }}}}
    return [size_xsum, output]
  end
  def self.parseDate(date)
    date_time = DateTime.parse(date)
    return nil if date_time.year > 2037 || date_time.year < 1970 #prevent overflow?
    return date_time.to_time.to_i
  end
  def self.showDate(seconds)
    DateTime.strptime(seconds.to_s,'%s').strftime("%Y-%m-%d %H:%M:%S")
  end
public
  def self.wait_query
    loop do
      print "Input your query: "
      @current_input_string = $stdin.gets.chomp
      inputArr = @current_input_string.split(",")
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
        total_span = outputArr[3] - outputArr[2]
        next puts "start time should earlier than end time" if total_span < 0
        next puts "start time is overflow: #{outputArr[2]}, class = #{outputArr[2].class}" if outputArr[2] > (1 << 31)
        next puts "end time is overflow: #{outputArr[3]}, class = #{outputArr[3].class}" if outputArr[3] > (1 << 31)
        next puts "time span should not lower than 1." if outputArr[4] <= 0 || outputArr[4] > 100000000
        span_num = (total_span / outputArr[4]) + 1
        next puts "wtf span_num < 0 ???" if span_num < 0
        next puts "span_num is too large: #{span_num} > 100000" if span_num > 100000
        #p "outputArr = #{outputArr}"
        @current_query_string = "Query the traffice of IP #{inputArr[0]} against all #{inputArr[1]} "
        @current_query_string << "from #{showDate(outputArr[2])} to #{showDate(outputArr[3])} for every #{outputArr[4]} second(s)...\n"
        print @current_query_string
        outputArr[5] = span_num;
        return outputArr
      end
      puts "Unown input. Please try again. inputArr = #{inputArr}"
    end
    return nil
  end
end
class GraphInterface
  def self.lineChart(start_time, time_span, span_num, *lines)
    output = "output.html"
    file = File.open(output, "w")
    lines_str = lines.map{|line|
      str = ""
      time = start_time
      line.each{|s| 
        str << "{x:" << time.to_s << ",y:" << s.to_s << "},"
        time += time_span
      }
      next "[" << str << "]"
    }
    file.write(<<-HTML_CONTENT
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <link href="lib/novus-nvd3-6616c9e/src/nv.d3.css" rel="stylesheet" type="text/css">
    <script src="lib/novus-nvd3-6616c9e/all.min.js"></script>
</head>
<body>
  input: #{QueryInterface.current_input_string}<br>
  #{QueryInterface.current_query_string}
  <div id="chartZoom"><a href="#" id="zoomIn">Zoom In</a> <a href="#" id="zoomOut">Zoom Out</a></div>
  <div id="chart1" class='with-transitions' style="width: 100%; height: 90%"><svg></svg></div>
</body>
<script>
  chart = addGraph(function(chart){
    chart.xAxis.tickFormat(function(d){ 
      var date = new Date(0);
      date.setSeconds(d);
      return d3.time.format('%H:%M:%S')(date); 
      return d3.time.format('%Y-%m-%d %H:%M:%S')(date); 
    });
    chart.yAxis
      .axisLabel('Voltage (v)')
      .tickFormat(d3.format(',d'));
  },function(){
    return [
      {
        values: #{lines_str[0]},
        key: "Bytes Sent",
        color: "#ff7f0e",
        area: false
      },
      {
        values: #{lines_str[1]},
        key: "Bytes Received",
        color: "#2ca02c",
        area: false      //area - set to true if you want this line to turn into a filled area chart.
      }
    ];
  });
</script>
</html>

HTML_CONTENT
      )
    file.close
    self.system_call_open(output)
  end
  def self.system_call_open(link)
    #host_os = RbConfig::CONFIG['host_os']
    if $host_os =~ /mswin|mingw|cygwin/
      system "start #{link}"
    elsif $host_os =~ /darwin/
      system "open #{link}"
    elsif $host_os =~ /linux|bsd/
      system "xdg-open #{link}"
    else
      p "Unown host: #{$host_os}"
    end
  end
end
#QueryInterface.wait_query
##140.109.23.120, 140.109.254.5, 2013/12/03 04:00:00, 2013/12/03 04:02:00, 10












