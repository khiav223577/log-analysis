class ConfigParser
  attr_reader   :setting_drop_after 
  attr_reader   :setting_max_size 
  MAP_OPERATOR = {"==" => "=".ord, "!=" => "!".ord, ">" => ">".ord, "<" => "<".ord, ">=" => ".".ord, "<=" => ",".ord}
  def parse(input)
    @setting_drop_after    = {}
    @setting_max_size      = {}
    global_symbols         = {} #記錄當前有效的所有symbol
    local_symbols          = {:buffer_start => 0} #recode outer symbol hidden by local symbol.
    nested_symbols         = [local_symbols]
    buffer_symbol_mappings = {} #記錄變數名字對應到buffer index的位置
    buffer                 = []
    buffer_item_counter    = 0
    line = nil
    line_count = 0
    bparser = BooleanParser.new
    perror = lambda{|msg| puts msg + " at line %3d:  %s" % [line_count, line]}
    exit_block = lambda{ 
      buffer << ["EXIT_BLOCK"]
      nested_symbols.pop.each{|key, value| global_symbols[key] = value } 
    }
    input.gsub!(/({[^}]*})/){|hash| hash.gsub(/(\/\/.*)\n/, "").delete("\t \n") } #let hash in one line.
    for line in input.split("\n")
      line_count += 1
      line.sub!(/((?:"(?:\\"|[^"])*"|.)*?)\s*\/\/.*$/,'\1') #match "//" that are not in a string
      line.strip! #removes leading and trailing whitespaces
      #p line and next #debug show lines
      case
      when line =~ /^\s*$/ ; next
      when line =~ /\/\//
      when line =~ /^#(MAX_SIZE)\[([0-9]+)\] \s*(\w+)/
        @setting_max_size[$3] = ($2 == "" ? 511 : $2.to_i)
      when line =~ /^#(DROP_AFTER)\[(.*)\] \s*(\w+)/
        @setting_drop_after[$3] = ($2 == "" ? nil : $2)
      when line =~ /^(#(?:els)?if) \s*(.*)/
        exit_block.call if $1 == "#elsif" #exit block and get into another block
        nested_symbols << (local_symbols = {:buffer_start => buffer_item_counter})
        imfs = bparser.parse($2)
        perror.call("Syntax error") and next if imfs == nil
        imfs.each{|s| #s = ["name", "==", "string"]
          next if s.class != Array
          idx = global_symbols[s[0]]
          perror.call("undefined symbol '%s'" % s[0]) and next if idx == nil
          s[0] = idx[0]         #map variable name to global_index
          s[1] = MAP_OPERATOR[s[1]]  #map operator to integer
        }
        buffer << [$1, imfs]
        buffer_item_counter += 1
      when line =~ /^(#else)/
        exit_block.call
        nested_symbols << (local_symbols = {:buffer_start => buffer_item_counter})
        buffer << [$1]
      when line =~ /^(#end)/
        exit_block.call
        local_symbols = nested_symbols[-1]
        buffer << [$1]
      else
        hash = {:drop => false, :peek => false}
        case
        when line =~ /^(Date)\[(.*)\] \s*(\w+)\s*({.*?})?/
        when line =~ /^(String)\[(.*)\] \s*(\w+)\s*({.*?})?/
          hash[:max_size] = (@setting_max_size["String"] || 511)
        when line =~ /^(Char)() \s*(\w+)\s*({.*?})?/
        when line =~ /^(Int)\[([0-9]+)\] \s*(\w+)\s*({.*?})?/
        when line =~ /^(IPv4)\[(.*)\] \s*(\w+)\s*({.*?})?/
        when line =~ /^(DROP)\[(.*)\]/
          hash[:drop] = true
        when line =~ /^(#DEBUG) (.*)/
        else ; perror.call("Syntax error") and next
        end
        if $3 != nil
          perror.call("Duplicate symbol") and next if local_symbols[$3] != nil
          local_symbols[$3] = global_symbols[$3]
          buffer_symbol_mappings[$3] = buffer_item_counter
          global_symbols[$3] = [buffer_item_counter, local_symbols[:buffer_start]]
        end
        eval($4).each{|key, value| hash[key] = value} if $4 != nil
        buffer << [$1,$2.extract_escape_symbol, $3, hash] #[Type, format, vocabulary, hash] 
        buffer_item_counter += 1
        if (drop_after = @setting_drop_after[$1])
          buffer << ["DROP", drop_after, nil, {:drop => true, :peek => false}]
          buffer_item_counter += 1
        end
      end
    end
    #p @setting_drop_after
    #p @setting_max_size
    #p global_symbols
    #p nested_symbols
    #p buffer, buffer.size
    return {:buffer => buffer, :buffer_symbol_mappings => buffer_symbol_mappings}
  end
end