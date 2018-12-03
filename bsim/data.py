
import os
from abc import ABC, abstractmethod


class Data(ABC):
    def __init__(self, debug=False):
        self.dir = os.path.dirname(__file__)
        self.debug = debug
        self._so = None
        self.c_type = None

    def so(self):
        if not self._so:
            self.compile_()
        return self._so

    @abstractmethod
    def to_c(self):
        pass

    @abstractmethod
    def to_gpu(self):
        pass

    @abstractmethod
    def from_gpu(self, gpu, only_struct=True):
        pass

    @abstractmethod
    def compile_(self):
        pass

    @abstractmethod
    def _generate_h(self):
        pass

    @abstractmethod
    def _generate_data_cu(self):
        pass

    @abstractmethod
    def _generate_py(self):
        pass

