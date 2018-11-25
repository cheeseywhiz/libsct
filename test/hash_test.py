"""Code generator for hash test suite"""
import collections
import dataclasses
import struct
import numpy as np


def fnv_hash(byte_seq):
    # https://tools.ietf.org/html/draft-eastlake-fnv-15
    hash_val = np.uint64(0xcbf29ce484222325)  # FNV offset basis
    FNV_prime = np.uint64(0x100000001b3)

    for byte in map(np.uint8, byte_seq):
        hash_val ^= byte
        hash_val *= FNV_prime

    return int(hash_val)


class MutableSequence(collections.abc.MutableSequence):
    __slots__ = '__list'

    def __getitem__(self, item):
        return self.__list[item]

    def __setitem__(self, item, value):
        self.__list[item] = value

    def __delitem__(self, item):
        self.__list.__delitem__(item)

    def __len__(self):
        return len(self.__list)

    def insert(self, index, item):
        self.__list.insert(index, item)

    def __init__(self, *args, **kwargs):
        super().__init__()
        self.__list = list(*args, **kwargs)

    def __repr__(self):
        return repr(self.__list)


class CArray(MutableSequence):
    def __init__(self, name, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.name = name

    def __str__(self):
        tab = ' ' * 8
        inside = (',\n' + tab).join(map(str, self))
        return f'struct {self.name} {self.name}s[] = {{\n{tab}{inside},\n}};'


def c_repr(obj):
    if isinstance(obj, str):
        return ''.join(['"', repr(obj)[1:-1], '"'])

    if isinstance(obj, int):
        return hex(obj)

    return repr(obj)


@dataclasses.dataclass
class CStruct:
    def __str__(self):
        inside = ', '.join(
            c_repr(getattr(self, field.name))
            for field in dataclasses.fields(self)
        )
        return f'{{{inside}}}'


@dataclasses.dataclass
class HashCase(CStruct):
    item: str
    expected_hash: int


class StringCase(HashCase):
    def __init__(self, string, expected_hash=None):
        if expected_hash is None:
            expected_hash = fnv_hash(string.encode())

        super().__init__(string, expected_hash)


class IntCase(HashCase):
    def __init__(self, number, expected_hash=None):
        if expected_hash is None:
            expected_hash = fnv_hash(struct.pack('i', number))

        super().__init__(number, expected_hash)


def main():
    print(CArray(
        'string_case', map(StringCase, [
            'Hello World',
            'hello world',
            'hello worle',
            '',
            'abc',
            '123',
            '1234',
            'The quick brown fox jumps over the lazy dog.',
            'abcdefghijklmnopqrstuvwxyz',
        ])
    ))
    print(CArray(
        'int_case', map(IntCase, [
            0,
            1,
            100,
            1000000,
            1024,
            1025,
            2048,
            2049,
            0x7effffff,
            2 ** 31 - 1,
            2 ** 31 - 2,
        ])
    ))


if __name__ == '__main__':
    main()
