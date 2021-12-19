import sys
from typing import IO

from model import AbstractModel, LabModel


def create_model() -> AbstractModel:
    return LabModel()


def get_symbols(file: IO):
    for line in file:
        for sym in map(int, line.split()):
            yield sym


def simulate(model: AbstractModel, input_file: IO = sys.stdin):
    for sym in get_symbols(input_file):
        out = model.simulate(sym)
        sys.stdout.write(f'{out}\n')


def main():
    model = create_model()
    simulate(model)


if __name__ == '__main__':
    main()
