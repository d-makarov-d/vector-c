import unittest
import sys

from vector import Vector


class MIterable:
    def __init__(self, n: int):
        self.n = n

    def __iter__(self):
        return self

    def __next__(self):
        if self.n > 0:
            self.n -= 1
            return self.n
        else:
            raise StopIteration


class MSequence:
    def __init__(self, n):
        self.n = n

    def __len__(self):
        return self.n

    def __getitem__(self, item):
        if self.n > item >= 0:
            return item + 1
        else:
            raise IndexError('Out of bounds')


class MSeqWrong(MSequence):
    def __getitem__(self, item):
        if self.n > item >= 0:
            return list()
        else:
            raise IndexError('Out of bounds')


class MIterWrong(MIterable):
    def __next__(self):
        if self.n > 0:
            self.n -= 1
            if self.n == 1:
                return list()
            else:
                return self.n
        else:
            raise StopIteration


class Test(unittest.TestCase):
    def test_class_constructor_tuple(self):
        vec = Vector((1, 2, 3))
        self.assertEqual((1, 2, 3), vec.cart)

    def test_class_constructor_list(self):
        vec = Vector([1, 2, 3])
        self.assertEqual((1, 2, 3), vec.cart)

    def test_class_constructor_set(self):
        vec = Vector({1, 2, 3})
        self.assertEqual((1, 2, 3), vec.cart)

    def test_class_constructor_iterator(self):
        vec = Vector(MIterable(3))
        self.assertEqual((2., 1., 0.), vec.cart)

    def test_class_constructor_sequence(self):
        vec = Vector(MSequence(3))
        self.assertEqual((1., 2., 3.), vec.cart)

    def test_constructor_iter__elem_insufficient_error(self):
        self.assertRaisesRegex(
            ValueError,
            'Vector constructor first argument must contain 3 elements, got 2',
            lambda: Vector(MIterable(2)),
        )

    def test_constructor_iter_elem_too_much_error(self):
        self.assertRaisesRegex(
            ValueError,
            'Vector constructor first argument must contain 3 elements, got more',
            lambda: Vector(MIterable(4)),
        )

    def test_constructor_iter_wrong_content(self):
        self.assertRaisesRegex(
            TypeError,
            'Vector constructor first argument must contain numeric values, got "list" at 1',
            lambda: Vector(MIterWrong(3)),
        )

    def test_constructor_seq__elem_insufficient_error(self):
        self.assertRaisesRegex(
            ValueError,
            'Vector constructor first argument must contain 3 elements, got 1',
            lambda: Vector(MSequence(1)),
        )

    def test_constructor_seq_elem_too_much_error(self):
        self.assertRaisesRegex(
            ValueError,
            'Vector constructor first argument must contain 3 elements, got 4',
            lambda: Vector(MSequence(4)),
        )

    def test_constructor_seq_wrong_content(self):
        self.assertRaisesRegex(
            TypeError,
            'Vector constructor first argument must contain numeric values, got "list" at 0',
            lambda: Vector(MSeqWrong(3)),
        )

    def test_constructor_wrong_param_type(self):
        self.assertRaisesRegex(
            TypeError,
            'Vector constructor first argument must be an Sequence or Iterable, got "object"',
            lambda: Vector(object()),
        )
