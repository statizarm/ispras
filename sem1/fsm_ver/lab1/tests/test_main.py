from _pytest.fixtures import fixture

from main import simulate
from model import LabModel


@fixture
def lab_model():
    return LabModel()


def get_test_sequences(filepath):

    def get_sequence(seq_line):
        return tuple(map(lambda x: tuple(x.split('/')), seq_line.split()))

    res = []
    with open(filepath) as file:
        for line in file:
            res.append(get_sequence(line))

    return res


def test_simulate(lab_model):
    for seq in get_test_sequences('tests/w_tests/w'):
        lab_model = LabModel()

        for stimulus, reaction in seq:
            res = lab_model.simulate(int(stimulus))
            assert res == int(reaction)
