"""Generic code generator for tests"""
import collections
import dataclasses
import functools


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


@functools.singledispatch
def c_repr(obj, type_=None):
    return repr(obj)


@c_repr.register
def _(obj: str, type_=None):
    return ''.join(['"', repr(obj)[1:-1], '"'])


@c_repr.register
def _(obj: int, type_=None):
    if isinstance(type_, str):
        parts = []

        if 'hex' in type_:
            parts.append(hex(obj))
        else:
            parts.append(repr(obj))

        if 'unsigned' in type_:
            parts.append('U')

        for i in range(type_.count('long')):
            parts.append('L')

        return ''.join(parts)

    return repr(obj)


@dataclasses.dataclass
class CStruct:
    def __str__(self):
        inside = ', '.join(
            c_repr(getattr(self, field.name), field.type)
            for field in dataclasses.fields(self)
        )
        return f'{{{inside}}}'
