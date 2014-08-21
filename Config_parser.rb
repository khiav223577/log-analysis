class Config_Parser
	attr_reader   :setting_drop_after 
	attr_reader   :setting_max_size 
	MAP_OPERATOR = {"==" => "=".ord, "!=" => "!".ord, ">" => ">".ord, "<" => "<".ord, ">=" => ".".ord, "<=" => ",".ord}
	def parse(input)
		@setting_drop_after = {}
		@setting_max_size   = {}
		global_symbols      = {}
		local_symbols       = {:buffer_start => 0} #recode outer symbol hidden by local symbol.
		nested_symbols      = [local_symbols]
		buffer              = create_buffer()
		line = nil
		line_count = 0
		bparser = Boolean_parser.new
		perror = lambda{|msg| puts msg + " at line %3d:  %s" % [line_count, line]}
		exit_block = lambda{ 
			buffer << ["EXIT_BLOCK"]
			nested_symbols.pop.each{|key, value| global_symbols[key] = value } 
		}
		for line in input.split("\n")
			line_count += 1
			line.sub!(/^\s*/,"")
			line.sub!(/((?:"(?:\\"|[^"])*"|.)*?)\s*\/\/.*$/,'\1') #match "//" that are not in a string
			line.sub!(/\s*$/,"")
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
				nested_symbols << (local_symbols = {:buffer_start => buffer.size})
				imfs = bparser.parse($2)
				perror.call("Syntax error") and next if imfs == nil
				imfs.each{|s| #["name", "==", "string"]
					next if s.class != Array
					idx = global_symbols[s[0]]
					perror.call("undefined symbol '%s'" % s[0]) and next if idx == nil
					s[0] = idx[0] 				#map variable name to global_index
					s[1] = MAP_OPERATOR[s[1]]	#map operator to integer
				}
				buffer << [$1, imfs]
			when line =~ /^(#else)/
				exit_block.call
				nested_symbols << (local_symbols = {:buffer_start => buffer.size})
				buffer << [$1]
			when line =~ /^(#end)/
				exit_block.call
				local_symbols = nested_symbols[-1]
				buffer << [$1]
			else
				case
				when line =~ /^(Date)\[(.*)\] \s*(\w+)/
				when line =~ /^(String)\[(.*)\] \s*(\w+)/
				when line =~ /^(Int)\[([0-9]+)\] \s*(\w+)/
				when line =~ /^(IPv4)\[(.*)\] \s*(\w+)/
				when line =~ /^(DROP)\[(.*)\]/
				when line =~ /^(#DEBUG) (.*)/
				else ; perror.call("Syntax error") and next
				end
				if $3 != nil
				    perror.call("Duplicate symbol") and next if local_symbols[$3] != nil
					local_symbols[$3] = global_symbols[$3]
					global_symbols[$3] = [buffer.size, local_symbols[:buffer_start]]
				end
				data = [$1,$2.extract_escape_symbol]
				data << $3 if $3
				buffer << data #[Type, format, vocabulary, extra]
				if (drop_after = @setting_drop_after[$1])
					buffer << ["DROP", drop_after]
				end
			end
		end
		#p @setting_drop_after
		#p @setting_max_size
		#p global_symbols
		#p nested_symbols
		#p buffer, buffer.size
		return buffer
	end
	def create_buffer
		buffer = []
		buffer.instance_variable_set(:@buffer_len, 0)
		def buffer.<<(other)
			super(other)
			@buffer_len += 1
		end
		def buffer.size
			return @buffer_len
		end
		return buffer
	end
end