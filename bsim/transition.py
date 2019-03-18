import re
a ='''
v = exp(-dt/tau) * v + (1-exp(-dt/tau)) * (i_offset * (tau/c) + v_rest) + 
i_exec * (exp(-dt/tau_e) - exp(-dt/tau)) * ((tau/c) * (tau_e/(tau_e-tau))) + 
i_inh * (exp(-dt/tau_i) - exp(-dt/tau)) * ((tau/c) * (tau_i/(tau_i-tau))); 
i_exec *= exp(-dt/tau_e);
i_inh *= exp(-dt/tau_i)
'''
# "data->p_v[nid] = data->p_C[nid] * data->p_v[nid] + data->p_v_tmp[nid] + "
#                      "data->p_i_exec[nid] * data->p_C_exec[nid] + data->p_i_inh[nid] * data->p_C_inh[nid];")
# "data->p_i_exec[nid] *= data->p_Cexec[nid];"
# "data->p_i_inh[nid] *= data->p_Cinh[nid];"

class TranSition():

    def __init__(self, strings):
        self.strings = strings
    #生成字符串列表
    def tran_list(self):
        list1 = self.strings.split('\n')
        strings2 = ''
        for i in list1:
            strings2 = strings2 + re.sub(' +', ' ', i)
        print(strings2)
        f = re.compile('[a-zA-Z_]+')
        list_a = f.findall(strings2)
        list_a = list(set(list_a))
        list_math = ['exp', 'log', 'max', 'min', 'sin', 'cos', 'tan', 'abs', 'round', 'sqrt', 'pow', 'ceil', 'floor']
        for i in list_a:
            if i in list_math:
                list_a.remove(i)
        for i in list_a:
            strings2 = strings2.replace(i, 'data->p_{}[nid]'.format(i))
        print(strings2)
        str_list = strings2.split(';')
        return str_list

if __name__ == '__main__':
    tran = TranSition(a)
    strings2 = tran.tran_list()
