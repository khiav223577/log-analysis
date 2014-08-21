class Boolean_parser
	#match string: "(?:\\"|[^"])*"
	@@regular_expr = /\s*(?:(\|\|)|(&&)|\((.*)\)|(\w+)\s*(==|!=)\s*"((?:\\"|[^"])*)"|\S+)/
	def parse(input)
		@postfix_buffer = []
		output_expr(input)
		return @postfix_buffer
		#return build_parsing_tree #debug
	end
	def output_op(op)
		case op
		when 1 ; @postfix_buffer << "|" #or
		when 2 ; @postfix_buffer << "&" #and	
		end
	end
	def output_expr(input)
		op_buffer = [0]
		input.gsub(@@regular_expr){|s| 
			#p [s,$1,$2,$3,$4,$5,$6]
			case 
			when $1 ; prior = 1                     #or
			when $2 ; prior = 2                     #and		
			when $3 ; output_expr($3)               #()
			when $4 ; @postfix_buffer << [$4,$5,$6] #symbol. Ex: [AAA, ==, "str"]
			else    ; puts("syntax error:" + s)	
			end
			next if prior == nil
			output_op(op_buffer.pop) while prior <= op_buffer[-1]
			op_buffer << prior
		}
		op_buffer.reverse_each{|op| output_op(op) }
		return @postfix_buffer
	end
	def build_parsing_tree
		buffer = [] #[[op, rExpr, lExpr], ...]
		@postfix_buffer.each{|s| buffer << (s.class == Array ? s : [s, buffer.pop, buffer.pop]) }
		return buffer.first
	end
	def self.debug
		parser = Boolean_parser.new
		p parser.parse(%{AAA == "allow" && BBB == "123" || WWW == "deny" && CCC == "test" || DDD == "123" })
		p parser.parse(%{AAA == "allow" && BBB == "123" || WWW == "deny" && (CCC == "test" || DDD == "123") || EEE == "ruby"})
		p parser.parse(%{AAA == "allow" && BBB == "123" || WWW == "deny" && CCC == "test" || DDD == "123" || EEE == "ruby"})
	end
end