
import os
import re 

import generate_model_cpu as gmc
import generate_model_gpu as gmg
import generate_kernel_gpu as gkg

class Func:
    def __init__(self, name):
        self.name = name
        self.output = {}
        self.input = {}
        self.body = []

    def load_input(self, content):
        self.input = parase_para(content)

    def load_output(self, content):
        self.output = parase_para(content)

    def load_operation(self, content):
        self.body = content
        
    def load_none(self, name):
        return

    def load_error(self, name):
        print "Unsupported " + name + " defined in function " + self.name 
        return self.load_none

    def load_type(self, content, type_name):
        load_types = {'input':self.load_input, 'output':self.load_output, 'body':self.load_operation}
        if type_name in load_types:
            load_types[type_name](content) 
        else:
            self.load_error(type_name)

    def load_content(self, content):
        blocks = parase_blocks(content)

        for block in blocks:
            block_obj = re.search('@(\w*)\W*(\w*)', block)
            if block_obj == None:
                continue

            block_type = block_obj.group(1)
            block_name = block_obj.group(2)
            self.load_type(blocks[block], block_type)


class Model:

    def __init__(self, model_type, name, model):
        self.name = name
        self.type = model_type
        self.paras = {}
        self.funcs = []
        self.model = model

    #def __init__(self, filename):
    #    f = open(filename)
    #    self.__init__(f.readlines())

    def __init__(self, content):

        if type(content) == str and os.path.exists(content):
            content = open(content).readlines()

        blocks = parase_blocks(content)

        if len(blocks) != 1:
            print "Don't support load multiple or None Models yet"
            self.name = 'Unknown'
            self.type = 'Unknown'
            self.model = ''
        else: 
            for block in blocks:
                block_obj = re.search('@(\w*)\W*(\w*)', block)
                if block_obj == None:
                    continue

                self.type = block_obj.group(1)
                self.name = block_obj.group(2)
                self.model = blocks[block]

        self.paras = {}
        self.funcs = []

        self.build_model()

    def load_para(self, content, name):
        self.paras = parase_para(content)

    def load_func(self, content, name):
        func = Func(name)
        func.load_content(content)
        self.funcs.append(func)

    def load_none(self, content, name):
        return

    def load_error(self, name):
        print "Unsupported " + name + " defined in " + self.type + " " + self.name 
        return self.load_none


    def load_type(self, content, type_name, name):
        load_types = {'variable':self.load_para, 'func':self.load_func}
        if type_name in load_types:
            load_types[type_name](content, name) 
        else:
            self.load_error(type_name)

    def build_model(self):
        blocks = parase_blocks(self.model)

        for block in blocks:
            block_obj = re.search('@(\w*)\W*(\w*)', block)
            if block_obj == None:
                continue

            block_type = block_obj.group(1)
            block_name = block_obj.group(2)
            self.load_type(blocks[block], block_type, block_name)

    def gen_code(self):
        type_name = self.name#.capitalize() 
        type_type = self.type.capitalize()
        path_name = "./" + type_type.lower() + "/" + type_name

        if not os.path.exists(path_name):
            os.makedirs(path_name)


        gmc.generate_h_file(self.paras, type_name, type_type, path_name)
        gmc.generate_cpp_file(self.paras, type_name, type_type, path_name)

        type_type += "s"
        gmg.generate_h_file(self.paras, type_name, type_type, path_name)
        gmg.generate_cpp_file(self.paras, type_name, type_type, path_name)
        gmg.generate_cu_file(self.paras, type_name, type_type, path_name)

        gkg.generate_h_file(self.paras, type_name, type_type, path_name)
        gkg.generate_cu_file(self.paras, type_name, type_type, path_name)

def parase_blocks(content):
    blocks = {}
    std_offset = -1
    #offset_stack = []
    block = []
    block_meta = ''
    for line in content:
            line = re.search('([^#]*)', line).group(1)
            line_obj = re.search('(^\s*([^#\s][^#]*[^#\s]))\s*', line)
            if line_obj == None:
                continue
            
            line = line_obj.group(1)

            offset = line_obj.start(2)
            if (std_offset == -1):
                std_offset = offset
            if (offset > std_offset):
                block.append(line)
            elif (offset == std_offset):
                if len(block) > 0:
                    if block_meta in blocks:
                        blocks[block_meta].extend(block)
                    else:
                        blocks[block_meta] = block

                if re.search('@', line) != None:
                    block_meta = line
                    block = []
                else:
                    block_meta = '@body:'
                    block = [line]

            else:
                print "Wrong style: Outer block inside a block"

    if len(block) > 0:
        if block_meta in blocks:
            blocks[block_meta].extend(block)
        else:
            blocks[block_meta] = block

    return blocks

def parase_para(content):
    para = {}
    for line in content:
        line_obj = re.search('(\S[^#]*)#?', line)
        if line_obj == None:
            continue

        line = line_obj.group(1)

        para_obj = re.search('(\S*)\s*:\s*(\S.*)', line)

        if para_obj == None:
            print "Parameter style wrong!"

        para[para_obj.group(1)] = para_obj.group(2).split(', ')

    return para
