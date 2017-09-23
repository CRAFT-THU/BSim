#!/usr/bin/python

import os

def generate_h_file(paras, type_name, type_type, path_name):
    obj_type = type_name + type_type

    filename = os.path.join(path_name, obj_type + ".h")
    f = open(filename, "w+")

    f.write("#ifndef "+ obj_type.upper() + "_H\n")
    f.write("#define "+ obj_type.upper() + "_H\n")
    f.write("\n")
    f.write('#include <stdio.h>\n')
    if type_type == "Synapse":
        f.write('#include <list>\n')
    f.write('#include "../../base/' + type_type + 'Base.h"\n')
    f.write('\n')

    if type_type == "Synapse":
        f.write('using std::list;\n')
    f.write('\n')

    f.write("class " + obj_type + " : public " + type_type + "Base {\n")
    f.write("public:\n")

    f.write("\t" + obj_type + "(ID id")
    for para in paras:
        t = paras[para]
        f.write(", " + t + " " + para)
        if type_type == "Synapse":
            f.write(", real delay, real tau_syn")
    f.write(");\n")
    f.write("\t" + obj_type + "(const " + obj_type + " &" + type_type.lower() + ", ID id);\n")
    f.write("\t" + "~" + obj_type + "();\n")
    f.write('\n')

    if type_type == "Neuron":
        #f.write("\tvirtual real get_vm() override;\n")
        f.write("\tvirtual int recv(real I)  override;\n")
        f.write("\n")
    elif type_type == "Synapse":
        f.write("\tvirtual int recv()  override;\n")
        f.write("\tvirtual void setDst(NeuronBase *p)  override;\n")
        f.write("\n")
    else:
        f.write("\n")

    f.write("\tvirtual Type getType() override;\n")
    f.write('\n')
    f.write("\tvirtual int reset(SimInfo &info) override;\n")
    f.write("\tvirtual int update(SimInfo &info) override;\n")
    f.write("\tvirtual void monitor(SimInfo &info) override;\n")
    f.write('\n')
    f.write("\tvirtual size_t getSize() override;\n")
    f.write("\tvirtual int getData(void *data) override;\n")
    f.write("\tvirtual int hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) override;\n")
    f.write('\n')

    f.write("\tconst static Type type;\n")
    f.write("protected:")
    f.write('\n')
    for para in paras:
        t = paras[para]
        f.write("\t" + t + " _" + para + ";\n")

    if type_type == "Synapse":
        f.write("\tlist<int> delay_queue;\n")
        f.write("\tNeuronBase *pDest;\n")


    f.write("};\n\n")

    f.write("#endif /* " + obj_type.upper() + "_H */\n")

    f.close()


def generate_cpp_file(paras, type_name, type_type, path_name):
    obj_type = type_name + type_type

    filename = os.path.join(path_name, obj_type + ".cpp")
    f = open(filename, "w+")

    f.write("\n")
    f.write('#include <math.h>\n')
    f.write("\n")
    f.write('#include "../third_party/json/json.h"\n')
    f.write('#include "' + obj_type + '.h"\n')
    f.write('#include "G' + obj_type +'s.h"\n')
    f.write("\n")

    f.write("const Type " + obj_type + "::type = " + type_name + ";\n")
    f.write("\n")

    f.write(obj_type + "::" + obj_type + "(ID id")
    for para in paras:
        t = paras[para]
        f.write(", " + t + " " + para)
        if type_type == "Synapse":
            f.write(", real delay, real tau_syn")
    f.write(")\n")
    f.write("\t: " + type_type + "Base(id)")
    for para in paras:
        t = paras[para]
        f.write(", _" + para + "(" + para + ")")
    f.write("\n")
    f.write("{\n")
    f.write("\tthis->monitored = false;\n")
    f.write("}\n\n")


    f.write(obj_type + "::" + obj_type + "(const " + obj_type + " &" + type_type.lower() + ", ID id) : " + type_type + "Base(id)\n")
    f.write("{\n")
    for para in paras:
        t = paras[para]
        f.write("\tthis->_" + para + " = " + type_type.lower() + "._" + para + ";\n")
    f.write("\tthis->monitored = false;\n")
    f.write("}\n\n")

    f.write(obj_type + "::" + "~" + obj_type + "()\n")
    f.write("{\n")
    f.write("\tdelay_queue.clear();\n")
    f.write("}\n\n")

    if type_type == "Neuron":
        #f.write("real " + obj_type + "::" + "get_vm()\n")
        #f.write("{\n")
        #f.write("}\n\n")
        f.write("int " + obj_type + "::" + "recv(real I)\n")
        f.write("{\n")
        f.write("}\n\n")
    elif type_type == "Synapse":
        f.write("void " + obj_type + "::" + "setDst(NeuronBase *p)\n")
        f.write("{\n")
        f.write("\tpDest = p;\n")
        f.write("}\n\n")
        f.write("int " + obj_type + "::" + "recv()\n")
        f.write("{\n")
        f.write("}\n\n")
    else:
        f.write("\n")



    f.write("Type " + obj_type + "::" + "getType()\n")
    f.write("{\n")
    f.write("\treturn type;\n")
    f.write("}\n\n")

    f.write("int " + obj_type + "::" + "reset(SimInfo &info)\n")
    f.write("{\n")
    f.write("}\n\n")

    f.write("int " + obj_type + "::" + "update(SimInfo &info)\n")
    f.write("{\n")
    f.write("}\n\n")

    f.write("void " + obj_type + "::" + "monitor(SimInfo &info)\n")
    f.write("{\n")
    f.write("}\n\n")

    f.write("size_t " + obj_type + "::" + "getSize()\n")
    f.write("{\n")
    f.write("\treturn sizeof(G" + obj_type + "s);\n")
    f.write("}\n\n")

    f.write("int " + obj_type + "::" + "getData(void *data)\n")
    f.write("{\n")
    f.write("\tJson::Value *p = (Json::Value *)data;\n")
    f.write('\t(*p)["id"] = getID().getId();\n')
    for para in paras:
        f.write('\t(*p)["' + para + '"] = _' + para + ";\n")
    f.write("\n")
    f.write("\treturn 0;\n")
    f.write("}\n\n")

    f.write("int " + obj_type + "::" + "hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)\n")
    f.write("{\n")
    f.write("\tG" + obj_type + "s *p = (G" + obj_type + "s *) data;\n")
    f.write("\tid2idx[getID()] = idx + base;\n")
    f.write("\tsetIdx(idx+base);\n")
    f.write("\tidx2id[idx+base] = getID();\n")
    for para in paras:
        f.write("\tp->p_" + para + "[idx] = _" + para + ";\n")
    f.write("\n")
    f.write("\treturn 1;\n")
    f.write("}\n\n")

    f.close()
