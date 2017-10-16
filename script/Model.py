
import re 

class Func:
    
    def __init__(self, name, func_type):
        self.name = name
        self.output = {}
        self.input = {}
        self.operation = []

    def load_func(content):
        return None



class Model:

    def __init__(self, name, model_type):
        self.name = name
        self.type = model_type
        self.para = {}
        self.func = []


    def load_para(content):
        self.para = parase_para(content)

    def load_func(func):
        self.func.append(func)



    def load_model(content):
        blocks = parase_blocks(content)

        for block in blocks:


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
                    blocks[block_meta] = block
                block_meta = line
                block = []
            else:
                print "Wrong style: Outer block inside a block"

    if len(block) > 0:
        blocks[block_meta] = block

    return blocks



def parase_para(content):
    para = {}
    for line in content:
        line_obj = re.search('(\S[^#]*)#?', line)
        if not line_obj:
            continue

        line = line_obj.group(1)

        para_obj = re.search('(\S*)\s*:\s*(\S.*)', line)

        if not para_obj:
            print "Parameter style wrong!"

        para[para_obj.group(1)] = para_obj.group(2).split(', ')

        return para
