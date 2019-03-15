
import re
from ctypes import Structure
from typing import Iterable


def to_list(data):
    if isinstance(data, Iterable):
        return list(data)
    else:
        return [data]

def standardize(expression):
    names = '[a-zA-Z0-9_]'
    not_names = '[^a-zA-Z0-9_]'

    one_name = '^[a-zA-Z0-9_]+$'
    #not_one_name = '.*[^a-zA-Z0-9_].*'

    back = ''
    standard = []
    for i in expression:
        if (re.match(not_names, i) and not re.match(' ', i) and re.match(names, back))\
                or (re.match(not_names, back) and not re.match(' ', back) and re.match(names , i)):
            standard.append(' ' + i)
        else:
            standard.append(i)
        back = i

    standard = ''.join(standard)


    search = re.search('(.*)(\S+)=(.*)', standard)
    if search:
        if not re.match(one_name, search.group(3).strip()):
            standard = "%s = %s %s ( %s )" % (search.group(1).strip(), search.group(1).strip(),
                                             search.group(2).strip(), search.group(3).strip())
        else:
            standard = "%s = %s %s %s" % (search.group(1).strip(), search.group(1).strip(),
                                          search.group(2).strip(), search.group(3).strip())

    search = re.search('(.*)\+\+' + names +'*', standard)
    if search:
        assert(re.search(names+'+', search.group(1))), 'Syntax error %s' % expression
        standard = "%s = %s + 1" % (search.group(1).strip(), search.group(1).strip())

    search = re.search('(.*)--' + names +'*', standard)
    if search:
        assert(re.search(names+'+', search.group(1))), 'Syntax error %s' % expression
        standard = "%s = %s - 1" % (search.group(1).strip(), search.group(1).strip())

    eq = standard.count('=')
    assert eq <= 1, '%s should only one assign per line' % standard

    return standard


#def code_line(line, tab: int=1):
#    return '%s%s;\n' % (tab*'\t', line)
#
#
#def code_line_no_end(line, tab: int=1):
#    return '%s%s\n' % (tab*'\t', line)
#
#
#def malloc(ret: str="", type_: str="", num='1', tab: int=1):
#    return code_line(
#        line='%s * %s = static_cast<%s*>(malloc(sizeof(%s)*%s))' %
#             (type_, ret, type_, type_, num),
#        tab=tab
#    )

