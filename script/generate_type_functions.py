#!/usr/bin/python

import re
import shutil

TYPE = "Type"

type_file = open("../src/base/type.h", "r")
type_file_content = type_file.read();
type_file.close()

tmp = type_file_content[type_file_content.find(TYPE):]
type_content  = tmp[tmp.find("{")+1:tmp.find("}")]
#print "Before:"
#print type_file_content

type_content_without_comment = re.sub("/\*((?:.|\n)*?)\*/", "", type_content)
type_content_tmp = re.sub("\s*", "", type_content_without_comment)
type_content_main = re.sub("=\d*", "", type_content_tmp).split(',')
type_content_main.remove("TYPESIZE")

#print "After:"
#print type_content_main

func_file = open("../src/utils/TypeFunc.h", "r")
func_file_content = func_file.readlines();
func_file.close()

func_mains = []
func_names = []

for line in func_file_content:
    line = line.strip()
    if line.startswith("extern"):
        line = line[line.find("extern")+6:line.find(";")].strip()
        if not line.startswith("BlockSize"):
            get_name = re.search("\(\*(.*)\[", line)
            if get_name:
                func_names.append(get_name.group(1))
                func_mains.append(line.replace("TYPESIZE", ""))
            else:
                print line
#print func_names
#print func_mains

#shutil.copyfile("../src/utils/TypeFunc.cu", "../src/utils/TypeFunc.cu.copied")
#shutil.copyfile("../src/utils/TypeFunc.cpp", "../src/utils/TypeFunc.cpp.copied")
#shutil.copyfile("../src/mpi_utils/mpi_func.cpp", "../src/mpi_utils/mpi_func.cpp.copied")

cu_file = open("../src/utils/TypeFunc.cu", "w")
cpp_file = open("../src/utils/TypeFunc.cpp", "w")

warn_info = "/* This file is generated by scripts automatively.\n * do not change it by hand.\n */\n\n"
common_headers = '#include "../../include/GNeuron.h"\n#include "../../include/GSynapse.h"\n#include "../utils/TypeFunc.h"\n\n'

cu_content = []
cpp_content = []
mpi_content = []

cu_content.append(warn_info + '#include "../gpu_utils/gpu_func.h"\n\n' + common_headers)
cpp_content.append(warn_info + common_headers)

for (name, body) in zip(func_names, func_mains):
    tmp = body + " = {"

    if name.find("AllType") >= 0:
            for type_name in type_content_main:
                if type_content_main.index(type_name) >=  type_content_main.index("LIF"):
                    tmp += (" " + name.replace("Type", type_name) + ",")
                else:
                    tmp += (" " + name.replace("AllType", type_name) + ",")
    elif name.find("Connection") >= 0:
        for type_name in type_content_main:
            if type_content_main.index(type_name) < type_content_main.index("Static"):
                tmp += (" NULL,")
            else:
                tmp += (" " + name.replace("Type", type_name) + ",")
    else:
        for type_name in type_content_main:
            tmp += (" " + name.replace("Type", type_name) + ",")
        
    tmp = tmp[:-1] + "};\n\n"
    if name.startswith("cuda"):
        cu_content.append(tmp)
    elif name.startswith("mpi"):
        mpi_content.append(tmp)
    else:
        cpp_content.append(tmp)

cu_file.writelines(cu_content)
cpp_file.writelines(cpp_content)
cpp_file.writelines(mpi_content)
#print mpi_content

cu_file.close()
cpp_file.close()

