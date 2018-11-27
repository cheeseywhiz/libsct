import dataclasses
import functools
from code_gen import CStructArray, CStruct


@functools.lru_cache(None)
def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)


@dataclasses.dataclass(init=False)
class FibCase(CStruct):
    n: int
    fib: 'unsigned'

    def __init__(self, n, fib=None):
        if fib is None:
            fib = fibonacci(n)

        self.n = n
        self.fib = fib


def main():
    print(CStructArray('fib_case', map(FibCase, range(93 + 1))))


if __name__ == '__main__':
    main()
