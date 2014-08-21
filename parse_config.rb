if ARGV.size != 2
	puts "parameters format = input_path output_path"
	exit
end
config_path = ARGV[0]
output_path = ARGV[1]

File.open(config_path, 'r'){|f|
	a = f.read
	p a
}
puts config_path