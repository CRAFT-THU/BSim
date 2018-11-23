
class BaseModel(object):
    def __init__(self):
        self.name =''
        self.expressions = {}
        self.parameters = {}

    def compile_(self):
        self._compile_h()
        self._compile_c()
        self._compile_cu()
        self._compile_py()

        return

    def _compile_h(self):

        return

    def _compile_c(self):
        return

    def _compile_cu(self):
        return

    def _compile_py(self):
        return


