
import re 

#class Parameter:
#
#    def __init__(self, name, data_type):
#        self.name = name
#        self.type = data_type

class Func:
    
    def __init__(self, name):
        self.name = name
        self.output = {}
        self.input = {}
        self.operation = []

def parase_para(content):
    para = {}
    for line in content:
        line_obj = re.search('(\S[^#]*)#?', line)
        if !line_obj:
            continue

        line = line_obj.group(1)

        para_obj = re.search('(\S*)\s*:\s*(\S.*)', line)

        if !para_obj:
            print "Parameter style wrong!"

        para[para_obj.group(1)] = para_obj.group(2).split(', ')

        return para

class Model:

    def __init__(self, name, model_type):
        self.name = name
        self.type = model_type
        self.para = {}
        self.func = []


    def load_para(content):
        self.para = parase_para(content)



    def load_model(content):
        content_offset = 0
        offset_stack = []
        content_list = []

        last_obj = ''

        for line in content:

            line_obj = re.search('(\S[^#]*)#?', line)
            if !line_obj:
                continue
            
            line = line_obj.group(1)
            if re.match("@parameter", line)
            #offset = line_obj.start()
            #if offset > content_offset:
            #    offset_stack.append(content_offset)
            #    content_offset = offset
            #elif offset < content_offset:
            #    print content_list
            #    content_list = []
            #    while (offset < content_offset):
            #        content_offset = offset_stack.pop()
            #    
            #content_list.append(line)
                


