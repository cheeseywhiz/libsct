import pprint
import random
import string
import numpy as np

ALPHABET = string.ascii_letters + string.digits
TABLE_SIZE = 1 << 8
MASK = TABLE_SIZE - 1
N_ITEMS = (TABLE_SIZE << 1) // 3


def chi_sq(freqs):
    expected = sum(freqs) / len(freqs)
    filled = len(list(filter(None, freqs)))
    chi_sq_list = [
        (i, observed, ((observed - expected) ** 2) / expected)
        for i, observed in enumerate(freqs)
    ]
    chi_sq_ret = sum(
        chi_sq_value
        for i, observed, chi_sq_value in chi_sq_list
    )
    chi_sq_list.sort(key=lambda item: -item[2])
    pprint.pprint(chi_sq_list)
    print(f'{filled} / {len(freqs)} = {filled / len(freqs)}')
    print(f'expected = {expected!r}')
    return chi_sq_ret


def fnv(byte_seq):
    hash_val = np.uint64(0xcbf29ce484222325)  # FNV offset basis
    FNV_prime = np.uint64(0x100000001b3)

    for byte in map(np.uint8, byte_seq):
        hash_val ^= byte
        hash_val *= FNV_prime

    return int(hash_val)


def str_to_bucket(string):
    return fnv(string.encode()) & MASK


def main():
    freqs = [0] * TABLE_SIZE

    for _ in range(N_ITEMS):
        string = ''.join(random.sample(ALPHABET, 8))
        bucket = str_to_bucket(string)
        freqs[bucket] += 1

    print(f'chi_sq = {chi_sq(freqs)}')


if __name__ == '__main__':
    main()
