"""Code generator for hash test suite"""
import collections
import dataclasses
import struct
import typing
import numpy as np
from code_gen import CArray, CStruct


def fnv_hash(byte_seq):
    # https://tools.ietf.org/html/draft-eastlake-fnv-15
    hash_val = np.uint64(0xcbf29ce484222325)  # FNV offset basis
    FNV_prime = np.uint64(0x100000001b3)

    for byte in map(np.uint8, byte_seq):
        hash_val ^= byte
        hash_val *= FNV_prime

    return int(hash_val)


@dataclasses.dataclass(init=False)
class StringCase(CStruct):
    string: str
    expected_hash: hex

    def __init__(self, string, expected_hash=None):
        if expected_hash is None:
            expected_hash = fnv_hash(string.encode())

        self.string = string
        self.expected_hash = expected_hash


@dataclasses.dataclass(init=False)
class IntCase(CStruct):
    number: int
    expected_hash: hex

    def __init__(self, number, expected_hash=None):
        if expected_hash is None:
            expected_hash = fnv_hash(struct.pack('i', number))

        self.number = number
        self.expected_hash = expected_hash


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
