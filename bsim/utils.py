
import re

def standardize(expression):
    names = '[a-zA-Z0-9_]'
    not_names = '[^a-zA-Z0-9_]'

    back = ''
    standard = []
    for i in expression:
        if (re.match(not_names, i) and not re.match(' ', i) and re.match(names , back))\
                or (re.match(not_names, back) and not re.match(' ', back) and re.match(names , i)):
            standard.append(' ' + i)
        else:
            standard.append(i)
        back = i

    standard = ''.join(standard)


    search = re.search('(.*)(\S+)=(.*)', standard)
    if search:
        standard = "%s= %s%s (%s )" % (search.group(1), search.group(1), search.group(2), search.group(3))

    search = re.search('(.*)\+\+' + names +'*', standard)
    if search:
        assert(re.search(names+'+', search.group(1))), 'Syntax error %s' % expression
        standard = "%s= %s+ 1" % (search.group(1), search.group(1))

    search = re.search('(.*)--' + names +'*', standard)
    if search:
        assert(re.search(names+'+', search.group(1))), 'Syntax error %s' % expression
        standard = "%s= %s- 1" % (search.group(1), search.group(1))

    eq = standard.count('=')
    assert eq <= 1, 'Only one assign per line'

    return standard

