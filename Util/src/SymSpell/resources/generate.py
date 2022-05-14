
def main():
	dictionary_lines = []
	with open("./frequency_dictionary_en_82_765.txt", 'r') as file:
		for line in file.readlines():
			line_info = line.strip("\n").split(" ")
			if len(line_info) < 2:
				continue
			dictionary_lines.append(
				"{{\"{}\", {} }},\n".format(line_info[0], line_info[1])
			)
	with open("./dictionary.hpp", 'w') as file:
		file.write("#include <vector>\n#include <string>\n")
		file.write("std::vector<std::pair<std::string, int64_t>> BuildInWords = {\n")
		for line in dictionary_lines:
			file.write(line)
		file.write("\n};")

main()

