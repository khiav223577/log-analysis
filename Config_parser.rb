
class Config_Parser
	attr_reader   :setting_drop_after 
	attr_reader   :setting_max_size 
	def initialize
	end
	def parse_file(path)
		File.open(path, 'r'){|f| @input = f.read }
		return parse(@input)
	end
	def parse(input)
		@setting_drop_after = {}
		@setting_max_size = {}
		symbol_table = {}
		buffer = []
		line_counter = 0
		bparser = Boolean_parser.new
		for line in input.split("\n")
			line_counter += 1
			line.sub!(/^\s*/,"")
			line.sub!(/((?:"(?:\\"|[^"])*"|.)*?)\s*\/\/.*$/,'\1') #match "//" that are not in a string
			line.sub!(/\s*$/,"")
			#p line and next #debug show lines
			case
			when line =~ /^\s*$/ ; next
			when line =~ /\/\//
			when line =~ /^#(MAX_SIZE)\[([0-9]*)\] \s*(\w+)/
				@setting_max_size[$3] = ($2 == "" ? 511 : $2.to_i)
			when line =~ /^#(DROP_AFTER)\[(.*)\] \s*(\w+)/
				@setting_drop_after[$3] = ($2 == "" ? nil : $2)
			when line =~ /^(#(?:els)?if) \s*(.*)/
				buffer << [$1, bparser.parse($2)]
			when line =~ /^(#end)/
			else
				case
				when line =~ /^(Date)\[(.*)\] \s*(\w+)/
				when line =~ /^(String)\[(.*)\] \s*(\w+)/
				when line =~ /^(Int)\[([0-9]*)\] \s*(\w+)/
				when line =~ /^(IPv4)\[(.*)\] \s*(\w+)/
				when line =~ /^(DROP)\[(.*)\]/
				else
					puts "Syntax error at line %3d:  %s" % [line_counter, line] and next
				end
				#puts("duplicate symbol") if $3 != nil and symbol_table[$3] != nil #TODO
				#symbol_table[$3] = true #TODO
				buffer << ($3 == nil ? [$1,$2] : [$1,$2,$3])
				drop_after = @setting_drop_after[$1]
				buffer << ["DROP", drop_after] if drop_after
			end
		end
		#p @setting_drop_after
		#p @setting_max_size
		p buffer
		return buffer
	end
	def shift_result
		return @buffer.shift
	end
end