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
        self.__list = []
        super().extend(list(*args, **kwargs))

    def __repr__(self):
        return repr(self.__list)


def tab_over(string, n_tabs=1):
    tab = '\t' * n_tabs
    parts = []

    for line in string.splitlines(True):
        parts.append(tab)
        parts.append(line)

    return ''.join(parts)


class CArray(MutableSequence):
    def __init__(self, type_, name, *args, ptr=0, **kwargs):
        super().__init__(*args, **kwargs)
        self.type_ = type_
        self.name = name
        self.ptr = ptr

    def __str__(self):
        inside = tab_over(',\n'.join(map(c_repr, self)))
        ptr = '*' * self.ptr
        return tab_over(f'{self.type_} {ptr}{self.name}[] = '
                        f'{{\n{inside},\n}};')


class StringArray(CArray):
    def __init__(self, name, *args, **kwargs):
        super().__init__('char', name, *args, ptr=1, **kwargs)


class CStructArray(CArray):
    def __init__(self, struct_name, *args, **kwargs):
        if struct_name is not None:
            type_ = f'struct {struct_name}'
            name = f'{struct_name}s'

        super().__init__(type_, name, *args, **kwargs)


@functools.singledispatch
def c_repr(obj, type_=None):
    return repr(obj)


def cr_cls(obj, type_=None):
    return str(obj)


def register_class(cls):
    c_repr.register(cls, cr_cls)
    return cls


c_repr.register_class = register_class


@c_repr.register
def cr_str(obj: str, type_=None):
    return ''.join(['"', repr(obj)[1:-1], '"'])


@c_repr.register
def cr_int(obj: int, type_=None):
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


@c_repr.register_class
@dataclasses.dataclass
class CStruct:
    def __str__(self):
        inside = ', '.join(
            c_repr(getattr(self, field.name), field.type)
            for field in dataclasses.fields(self)
        )
        return f'{{{inside}}}'


def comment(string):
    print(f'/* {string} */')
