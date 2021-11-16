from abc import ABC, abstractmethod
from typing import List


class AbstractModel(ABC):
    def simulate(self, sym: int):
        res = self._eval(sym)
        self._tick()
        return res

    @abstractmethod
    def _tick(self):
        raise NotImplementedError

    @abstractmethod
    def _eval(self, sym: int):
        raise NotImplementedError


class LabModel(AbstractModel):
    def __init__(self):
        self.curr_stamp = 0
        self.input = Input(0)

        first_and = And([self.input])

        inv = Not(input_=first_and)

        self.triggers = [DTrigger(input_=inv), DTrigger(input_=first_and)]

        first_and.add_input(input_=self.triggers[0])

        self.out = And([self.input, self.triggers[1]])

    def _tick(self):
        self.curr_stamp += 1
        for trig in self.triggers:
            trig.tick()

    def _eval(self, sym: int):
        self.input.set(sym)
        return self.out.eval(self.curr_stamp)


class AbstractEvaluableLogic(ABC):
    @abstractmethod
    def eval(self, stamp: int) -> int:
        raise NotImplementedError

    @abstractmethod
    def out(self) -> int:
        raise NotImplementedError


class Input(AbstractEvaluableLogic):
    def __init__(self, init_value=0):
        self.value = init_value

    def set(self, sym: int):
        self.value = sym

    def eval(self, _) -> int:
        return self.out()

    def out(self):
        return self.value


class And(AbstractEvaluableLogic):
    def __init__(self, inputs: List[AbstractEvaluableLogic]):
        self.last_stamp = -1
        self.inputs = inputs
        self.out_value = 0

    def eval(self, stamp) -> int:
        if self.last_stamp < stamp:
            self.last_stamp = stamp
            self.out_value = 1
            for in_ in self.inputs:
                if in_.eval(stamp) == 0:
                    self.out_value = 0

        return self.out_value

    def out(self):
        return self.out_value

    def add_input(self, input_: AbstractEvaluableLogic):
        self.inputs.append(input_)


class Not(AbstractEvaluableLogic):
    def __init__(self, input_: AbstractEvaluableLogic):
        self.last_stamp = -1
        self.input = input_

    def eval(self, stamp: int) -> int:
        if self.last_stamp < stamp:
            self.last_stamp = stamp
            return 1 - self.input.eval(stamp)
        else:
            return 1 - self.input.out()

    def out(self):
        return 1 - self.input.out()


class DTrigger(AbstractEvaluableLogic):
    def __init__(self, input_: AbstractEvaluableLogic, init_value=0):
        self.last_stamp = -1
        self.input = input_
        self.value = init_value

    def eval(self, stamp: int):
        if self.last_stamp < stamp:
            self.last_stamp = stamp
            self.input.eval(stamp)

        return self.value

    def tick(self):
        self.value = self.input.out()

    def out(self):
        return self.value
