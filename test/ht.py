import dataclasses
import itertools
import string
import random
from code_gen import CArray, CStructArray, CStruct, comment, c_repr, \
    StringArray

ALPHABET = string.ascii_letters + string.digits


@dataclasses.dataclass
class KeyValuePair(CStruct):
    key: str
    value: str


class KVPArray(CArray):
    def __init__(self, name, *args, **kwargs):
        super().__init__('struct key_value_pair', name, *args, **kwargs)

    @classmethod
    def from_starmap(cls, iterable, name=None):
        return cls(name, itertools.starmap(KeyValuePair, iterable))

    def sample(self, name=None, k=8):
        cls = type(self)
        return cls(name, random.sample(self, k=k))

    def keys(self, type_, name, ptr=0):
        return CArray(type_, name, [pair.key for pair in self], ptr=ptr)


def random_string(k=8):
    return ''.join(random.sample(ALPHABET, k=k))


@c_repr.register_class
class Null:
    def __str__(self):
        return f'{type(self).__name__.upper()}'


null = Null()


def sometimes_null(obj):
    if not random.randrange(3):
        return null

    return obj


@dataclasses.dataclass
class SetGetCase(CStruct):
    key: str
    value1: str
    value2: str

    @classmethod
    def from_kvp(cls, kvp):
        return cls(
            kvp.key, sometimes_null(kvp.value),
            sometimes_null(random_string()),
        )


@c_repr.register_class
class Sentinel:
    def __str__(self):
        return f'&{type(self).__name__.lower()}'


sentinel = Sentinel()


def main():
    random.seed(1)
    comment('init_test_table')
    key_value_pairs = KVPArray.from_starmap([
        ('hello', 'world'),
        ('goodbye', 'earth'),
        ('123', 'numbers'),
        ('', 'blank'),
        ('__doc__', 'docstring'),
        ('__eq__', 'equals'),
        ('__getitem__', 'self explanatory'),
        ('__new__', 'constructor'),
        ('__del__', 'destructor'),
        ('__enter__', 'context manager enter'),
        ('__exit__', 'context manager exit'),
        ('__init__', 'initializer'),
        ('__repr__', 'representation'),
        ('__str__', 'string'),
        ('__lt__', 'less than'),
        ('__gt__', 'greater than'),
        ('__hash__', 'self explanatory'),
        ('__bool__', 'boolin'),
    ], 'key_value_pairs')
    print(key_value_pairs)

    comment('test_pop')
    print(key_value_pairs.sample('pop_cases'))

    comment('test_contains')
    print(key_value_pairs.sample().keys('char', 'contains_cases', ptr=1))

    comment('test_get')
    get_cases = key_value_pairs.sample('get_cases')
    get_cases += KVPArray.from_starmap(
        (random_string(), sentinel)
        for _ in range(8)
    )
    print(get_cases)

    comment('test_set_get')
    print(CStructArray(
        'set_get_case', map(SetGetCase.from_kvp, key_value_pairs)
    ))

    comment('test_length')
    print(StringArray('random_keys', (random_string() for _ in range(8))))


if __name__ == '__main__':
    main()
