
def main():
	dictionary = ""
	with open("./frequency_dictionary_en_82_765.txt", 'r') as file:
		# for line in file.readlines():
		# 	line_info = line.strip("\n").split(" ")
		# 	if len(line_info) < 2:
		# 		continue
		# 	dictionary_lines.append(
		# 		"{{\"{}\", {} }},\n".format(line_info[0], line_info[1])
		# 	)
		dictionary = file.read()
	with open("./dictionary.hpp", 'w') as file:
		file.write("#define STR(X) #X\n")
		file.write("const char* BuildInWords = {\n")
		file.write("#include \"dictionary.txt\"")
		file.write("\n};")
	with open("./dictionary.txt", 'w') as file:
		file.write("STR(")
		file.write(dictionary)
		file.write(")")
		

main()

