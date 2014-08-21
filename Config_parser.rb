class Config_Parser
	def initialize
	end
	def parse_file(path)
		File.open(config_path, 'r'){|f| @input = f.read }
		parse(@input)
	end
	def parse(input)
		setting_drop_after = {}
		symbol_table = {}
		buffer = []
		for line in input.split("\n").delete_if{|s| s =~ /^\s*$/}
			line.sub!(/^\s*/,"")
			line.sub!(/((?:"(?:\\"|[^"])*"|.)*?)\s*\/\/.*$/,'\1') #match "//" that are not in a string
			line.sub!(/\s*$/,"")
			#p line and next #debug show lines
			case
			when line =~ /^\s*$/ ; next
			when line =~ /\/\//
			when line =~ /#(DROP_AFTER)\[(.*)\] \s*(\w+)/
				setting_drop_after[$3] = ($2 == "" ? nil : $2)
			when line =~ /#(if) \s*(.*)/
			else
				case
				when line =~ /(Date)\[(.*)\] \s*(\w+)/
				when line =~ /(String)\[(.*)\] \s*(\w+)/
				when line =~ /(Int)\[(.*)\] \s*(\w+)/
				when line =~ /(IPv4)\[(.*)\] \s*(\w+)/
				when line =~ /(DROP)\[(.*)\]/
				end
				raise("duplicate symbol") if $3 != nil and symbol_table[$3] != nil
				symbol_table[$3] = true
				drop_after = setting_drop_after[$1]
				buffer << ($3 == nil ? [$1,$2] : [$1,$2,$3])
				buffer << ["DROP", drop_after] if drop_after
			end
		end
		p setting_drop_after
		p buffer
	end
	def shift_result
		return @buffer.shift
	end
end