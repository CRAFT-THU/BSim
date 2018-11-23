
from bsim.model import BaseModel


class SynapseModel(BaseModel):
    def __init__(self, computation: str='', pre: str='', post: str='', name: str=''):
        """
        Create SynapseModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.learn = not (pre == '' and post == '')
        self.expressions, self.parameters = compile_({
            'computation': computation,
            'pre': pre,
            'post': post
        })
        self.parameters['variable'].add('weight')
        self.parameters['constant'].add('delay')
