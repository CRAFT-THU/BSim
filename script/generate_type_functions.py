#!/usr/bin/python

import re
import shutil

TYPE = "Type"

type_file = open('../src/base/type.h', "r")

type_file_content = type_file.read();

tmp = type_file_content[type_file_content.find(TYPE):]
type_content  = tmp[tmp.find("{")+1:tmp.find("}")]
print "Before:"
print type_file_content

type_content_without_comment = re.sub("/\*((?:.|\n)*?)\*/", "", type_content)
type_content_tmp = re.sub("\s*", "", type_content_without_comment)
type_content_main = re.sub("=\d*", "", type_content_tmp).split(',')
type_content_main.remove("TYPESIZE")

print "After:"
print type_content_main

shutil.copyfile("../src/utils/TypeFunc.cu", "../src/utils/TypeFunc.cu.copied")
shutil.copyfile("../src/utils/TypeFunc.cpp", "../src/utils/TypeFunc.cpp.copied")

cpp_file = open("../src/utils/TypeFunc.cpp", "w")
cu_file = open("../src/utils/TypeFunc.cu", "w")

