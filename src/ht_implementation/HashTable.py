import collections
import dataclasses
import random
import string
import struct
import typing
import numpy as np

ArrayT = typing.List
LinkedListT = typing.List
VoidPtr = typing.Any
size_t = int
ssize_t = int
uint64_t = int

HashT = uint64_t


def fnv_hash(byte_seq) -> HashT:
    # https://tools.ietf.org/html/draft-eastlake-fnv-15
    hash_val = np.uint64(0xcbf29ce484222325)  # FNV offset basis
    FNV_prime = np.uint64(0x100000001b3)

    for byte in map(np.uint8, byte_seq):
        hash_val ^= byte
        hash_val *= FNV_prime

    return int(hash_val)


KeyT = typing.TypeVar('KeyT')  # void*
HashKeyFunc = typing.Callable[[KeyT], HashT]  # typedef
KeyEqualsFunc = typing.Callable[[KeyT, KeyT], bool]  # typedef


@dataclasses.dataclass
class KeyType:
    hash_key: HashKeyFunc
    key_equals: KeyEqualsFunc


@dataclasses.dataclass
class Entry:
    key_hash: HashT
    key: KeyT
    value: VoidPtr


EntryIndexT = ssize_t
CollisionsT = LinkedListT[EntryIndexT]
BucketsT = ArrayT[CollisionsT]
EntriesT = ArrayT[Entry]


@dataclasses.dataclass(init=False)
class HashTable(collections.abc.MutableMapping):
    key_type: KeyType
    table_size: size_t
    buckets: BucketsT
    _entries: EntriesT

    def __init__(self, key_type: KeyType, *args, **kwargs):
        super().__init__()
        self.key_type = key_type
        self.table_size = 2
        self._entries = []
        self._init_buckets()
        super().update(dict(*args, **kwargs))

    def _do_resize(self, grow: bool) -> None:
        if grow:
            self.table_size <<= 1
        else:
            self.table_size >>= 1

        self._init_buckets()

    def _init_buckets(self) -> None:
        self.buckets = [[] for _ in range(self.table_size)]
        # trim deleted/None entries here?
        self._entries = self.entries()
        mask = self.table_size - 1

        for entry_i, entry in enumerate(self._entries):
            if entry is None:
                continue

            bucket = entry.key_hash & mask
            collisions = self.buckets[bucket]
            collisions.append(entry_i)

    def _exceeds_load_factor_limit(self, grow: bool) -> bool:
        # load factor limit = 2 / 3
        # load factor = length / size
        # factor > limit -> length / size > 2 / 3 -> length * 3 > size * 2

        if not grow:
            if self.table_size == 2:
                return False

            future_table_size = self.table_size >> 1
            return len(self) * 3 < future_table_size * 2

        return len(self) * 3 > self.table_size * 2

    def _resize(self, grow: bool) -> None:
        if self._exceeds_load_factor_limit(grow):
            self._do_resize(grow)

    def _resolve_collisions(
            self, key: KeyT, key_hash: HashT, collisions: CollisionsT,
            strict: bool,
    ) -> EntryIndexT:
        for collision_i, entry_i in enumerate(collisions):
            entry = self._entries[entry_i]

            if entry is None:
                continue

            if key is key_hash \
                    or key_hash == entry.key_hash \
                    or self.key_type.key_equals(key, entry.key):
                return entry_i
        else:
            if strict:
                raise KeyError(key)
            else:
                return -1

    def _method_init(
            self, key: KeyT, strict: bool,
    ) -> typing.Tuple[HashT, CollisionsT, EntryIndexT]:
        key_hash = self.key_type.hash_key(key)
        mask = self.table_size - 1
        bucket = key_hash & mask
        collisions = self.buckets[bucket]
        entry_i = self._resolve_collisions(key, key_hash, collisions, strict)
        return key_hash, collisions, entry_i

    def __setitem__(self, key: KeyT, value: VoidPtr) -> None:
        key_hash, collisions, entry_i = self._method_init(key, False)

        if entry_i >= 0:
            self._entries[entry_i].value = value
            return

        new_entry = Entry(key_hash, key, value)
        entry_i = len(self._entries)
        self._entries.append(new_entry)
        collisions.append(entry_i)
        self._resize(True)

    def __delitem__(self, key: KeyT) -> None:
        key_hash, collisions, entry_i = self._method_init(key, True)
        self._entries[entry_i] = None
        collisions.remove(entry_i)
        self._resize(False)

    def __getitem__(self, key: KeyT) -> VoidPtr:
        key_hash, collisions, entry_i = self._method_init(key, True)
        return self._entries[entry_i].value

    def entries(self) -> EntriesT:
        return [
            entry
            for entry in self._entries
            if entry is not None
        ]

    def __len__(self) -> ssize_t:
        return len(self.entries())

    def __iter__(self):
        return iter(
            entry.key
            for entry in self.entries()
        )


class StringType(KeyType):
    def hash_key(key: KeyT) -> HashT:
        return fnv_hash(key.encode())

    def key_equals(key: KeyT, other: KeyT) -> bool:
        return key == other


class IntType(KeyType):
    def hash_key(key: KeyT) -> HashT:
        # cast to char*
        return fnv_hash(struct.pack('q', key))

    def key_equals(key: KeyT, other: KeyT) -> bool:
        return key == other


fib_ht = HashTable(IntType)


def fibonacci(n):
    answer: uint64_t = fib_ht.get(n)

    if answer is not None:
        return answer

    if n <= 1:
        answer = n
    else:
        answer = fibonacci(n - 1) + fibonacci(n - 2)

    fib_ht[n] = answer
    return answer


def main():
    alphabet = string.ascii_letters + string.digits
    ht = HashTable(StringType)

    for i in range(86):
        key = ''.join(random.sample(alphabet, k=8))
        ht[key] = i

    for key in random.sample(list(ht), k=len(ht) // 2):
        del ht[key]

    for i in range(len(ht) // 4):
        key = ''.join(random.sample(alphabet, k=8))
        ht[key] = i

    print(ht)
    print(dict(ht))
    print(fibonacci(93))  # max for a 64 bit integer answer
    return ht


if __name__ == '__main__':
    main()
