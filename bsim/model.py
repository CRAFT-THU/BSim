import os
from abc import ABC, abstractmethod


class Model(ABC):
    def __init__(self):
        self.dir = os.path.dirname(__file__)

    @abstractmethod
    def generate_h(self):
        pass

    @abstractmethod
    def generate_data_cu(self, debug=False):
        pass

    @abstractmethod
    def generate_compute_cu(self, debug=False):
        pass

    @abstractmethod
    def generate_py(self):
        pass





