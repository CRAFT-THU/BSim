
import inspect

class ArrayObj(object):
    def __init__(self, name = '', num = 1):
        self.name = name
        self.num = num
        self.shared = []

    def __len__(self):
        return self.num

    def __getitem__(self, key):
        t = self.__class__

        for name, value in vars(self):
            if name not in ('name', 'num'):
                if value not in self.shared:
                    t.__dict__[name] = value[key]
                else:
                    t.__dict__[name] = value

        t.name = self.name + '_' + str(key)
        t.num = '1'
        #t.pre = self.pre[key]
        #t.post = self.post[key]

        return t



