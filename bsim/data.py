
import os
from abc import ABC, abstractmethod


class Data(ABC):
    def __init__(self):
        self._so = None

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

    def so(self):
        if not self._so:
            self.compile_()
        return self._so

