#!/usr/bin/env python3

import re

func_pat = re.compile(r"^(\w+( )?){2,}\([^!@#$+%^]+?\)")

def camelcase_to_snakecase(name):
	return re.sub(r'(?<!^)(?=[A-Z])', '_', name).lower()


with open("api.cpp") as f:
	lines = f.readlines()
	lines = [line.strip() for line in lines]
	lines = [line for line in lines if line]
	res = []
	for line in lines:
		m = func_pat.match(line)
		if m:
			func_name = m.group(1)
			# print(func_name, "->", camelcase_to_snakecase(func_name))
			res.append(line.replace(func_name, camelcase_to_snakecase(func_name))+'\n')

with open("api2.cpp", "w") as f:
	f.writelines(res)

		# print(line)

