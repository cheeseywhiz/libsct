"""Generic code generator for tests"""
import collections
import dataclasses


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


def c_repr(obj, type_):
    if isinstance(obj, str):
        return ''.join(['"', repr(obj)[1:-1], '"'])

    if isinstance(obj, int):
        if isinstance(type_, str):
            suffix_parts = []

            if 'unsigned' in type_:
                suffix_parts.append('U')

            for i in range(type_.count('long')):
                suffix_parts.append('L')

            suffix = ''.join(suffix_parts)
            return f'{obj!r}{suffix}'

        if type_ == hex:
            return hex(obj)

        return repr(obj)

    return repr(obj)


@dataclasses.dataclass
class CStruct:
    def __str__(self):
        inside = ', '.join(
            c_repr(getattr(self, field.name), field.type)
            for field in dataclasses.fields(self)
        )
        return f'{{{inside}}}'
