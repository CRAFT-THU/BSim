
import re

def standardize(expression):
    back = ''
    standard = []
    for i in expression:
        if (re.match('[^a-zA-z0-9_]', i) and not re.match('\s', i) and re.match('[a-zA-z0-9_]' , back))\
                or (re.match('[^a-zA-z0-9_]', back) and not re.match('\s', back) and re.match('[a-zA-z0-9_]' , i)):
            standard.append(' ' + i)
        else:
            standard.append(i)
        back = i

    return ''.join(standard)

if __name__ == '__main__':
    print(standardize('a= b+c_---aaa'))
