
from abc import ABC, abstractmethod


class BaseModel(ABC):
    def __init__(self):
        self.name = ''
        self.expressions = {}
        self.parameters = {}

    def compile_(self):
        self._compile_h()
        self._compile_c()
        self._compile_cu()
        self._compile_py()

        return

    @abstractmethod
    def _compile_h(self):
        """
        Generate .h file for the model
        """

    @abstractmethod
    def _compile_py(self):
        """
        Generate .py file for the model
        """

    @abstractmethod
    def _compile_c(self):
        """
        Generate .cpp file for the model
        """

    @abstractmethod
    def _compile_cu(self):
        """
        Generate .cu file for the model
        """
