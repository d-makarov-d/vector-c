import unittest
import numpy as np
import pickle
from astropy.coordinates import cartesian_to_spherical, spherical_to_cartesian

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


class Vector1(Vector):
    def __init__(self, cart):
        cart[0] += 1
        super().__init__(cart)


class Vector2(Vector):
    def __add__(self, other):
        if issubclass(other.__class__, Vector2):
            return Vector2([a + b for a, b in zip(self.cart, other.cart)])
        return super().__add__(other)


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

    def test_cart_assign_tuple(self):
        vec = Vector((1, 2, 3))
        vec.cart = (3, 3, 3)
        self.assertEqual((3, 3, 3), vec.cart)

    def test_cart_assign_list(self):
        vec = Vector([1, 2, 3])
        vec.cart = [3, 3, 3]
        self.assertEqual((3, 3, 3), vec.cart)

    def test_cart_assign_set(self):
        vec = Vector({1, 2, 3})
        vec.cart = {1, 2, 4}
        self.assertEqual((1, 2, 4), vec.cart)

    def test_cart_assign_iterator(self):
        vec = Vector([0, 0, 0])
        vec.cart = MIterable(3)
        self.assertEqual((2., 1., 0.), vec.cart)

    def test_cart_assign_sequence(self):
        vec = Vector([0, 0, 0])
        vec.cart = MSequence(3)
        self.assertEqual((1., 2., 3.), vec.cart)

    def test_cart_assign_iter_elem_insufficient_error(self):
        def f():
            Vector([0, 0, 0]).cart = MIterable(2)

        self.assertRaisesRegex(
            ValueError,
            'Vector cartesian component must contain 3 elements, got 2',
            f,
        )

    def test_cart_assign_iter_elem_too_much_error(self):
        def f():
            Vector([0, 0, 0]).cart = MIterable(4)

        self.assertRaisesRegex(
            ValueError,
            'Vector cartesian component must contain 3 elements, got more',
            f,
        )

    def test_cart_assign_iter_wrong_content(self):
        def f():
            Vector([0, 0, 0]).cart = MIterWrong(3)

        self.assertRaisesRegex(
            TypeError,
            'Vector cartesian component must contain numeric values, got "list" at 1',
            f,
        )

    def test_cart_assign_seq__elem_insufficient_error(self):
        def f():
            Vector([0, 0, 0]).cart = MSequence(1)

        self.assertRaisesRegex(
            ValueError,
            'Vector cartesian component must contain 3 elements, got 1',
            f,
        )

    def test_cart_assign_seq_elem_too_much_error(self):
        def f():
            Vector([0, 0, 0]).cart = MSequence(4)

        self.assertRaisesRegex(
            ValueError,
            'Vector cartesian component must contain 3 elements, got 4',
            f,
        )

    def test_cart_assign_seq_wrong_content(self):
        def f():
            Vector([0, 0, 0]).cart = MSeqWrong(3)

        self.assertRaisesRegex(
            TypeError,
            'Vector cartesian component must contain numeric values, got "list" at 0',
            f,
        )

    def test_cart_assign_wrong_param_type(self):
        def f():
            Vector([0, 0, 0]).cart = object()

        self.assertRaisesRegex(
            TypeError,
            'Vector cartesian component must be an Sequence or Iterable, got "object"',
            f,
        )

    def test_add(self):
        v1 = Vector([1, 2, 3])
        v2 = Vector([3, 3, 3])
        self.assertEqual((4, 5, 6), (v1 + v2).cart)
        r, lat, lon = cartesian_to_spherical(4, 5, 6)
        self.assertEqual((4, 5, 6), (v1 + v2).cart)

    def test_add_wrong_type(self):
        v = Vector([1, 2, 3])
        self.assertRaisesRegex(
            TypeError,
            r"unsupported operand type\(s\) for \+: 'vector\.Vector' and 'int'",
            lambda: v + 1,
        )

    def test_add_subclass_and_class(self):
        v1 = Vector1([1, 2, 3])
        v2 = Vector([1, 2, 3])
        self.assertEqual((3, 4, 6), (v1 + v2).cart)

    def test_add_subclass_and_subclass(self):
        v1 = Vector1([1, 2, 3])
        v2 = Vector2([1, 2, 3])
        self.assertEqual((3, 4, 6), (v1 + v2).cart)

    def test_subtract(self):
        v1 = Vector([1, 2, 3])
        v2 = Vector([3, 2, 1])
        self.assertEqual((-2, 0, 2), (v1 - v2).cart)

    def test_subtract2(self):
        v1 = Vector([1, 2, 3])
        v1 -= Vector([3, 2, 1])
        self.assertEqual((-2, 0, 2), v1.cart)

    def test_neg(self):
        v = Vector([1, 2, 3])
        v = -v
        self.assertEqual((-1, -2, -3), v.cart)

    def test_multiply(self):
        v1 = Vector([1, 2, 3])
        v2 = Vector([4, -6, 1])

        self.assertEqual((2, 4, 6), (v1 * 2).cart)

        a = np.array(v1.cart)
        b = np.array(v2.cart)
        self.assertEqual(tuple(np.cross(a, b)), (v1 * v2).cart)

    def test_hash(self):
        self.assertEqual(hash((1, 2, 3)), hash(Vector([1, 2, 3])))

    def test_compare(self):
        self.assertEqual(Vector([1, 2, 3]), Vector([1, 2, 3]))
        s = {Vector([1, 2, 3]), Vector([2, 2, 3]), Vector([1, 2, 3]), Vector([2, 2, 3])}
        self.assertEqual(2, len(s))

    def test_repr(self):
        self.assertEqual(Vector([1, 2, 3]).__repr__(), 'vector.Vector([1.000000, 2.000000, 3.000000])')

    def test_str(self):
        self.assertEqual(Vector([1, 2, 3]).__str__(), '[1.000000, 2.000000, 3.000000>')


class Components(unittest.TestCase):
    def test_x(self):
        v = Vector([1, 2, 3])
        self.assertEqual(1, v.x)
        v.x = 4
        self.assertEqual(4, v.x)
        v.x += 1
        self.assertEqual(5, v.x)
        def f():
            v.x = "g"
        self.assertRaisesRegex(
            TypeError,
            'Vector.x must be numeric, got "str"',
            f,
        )

    def test_y(self):
        v = Vector([1, 2, 3])
        self.assertEqual(2, v.y)
        v.y = 4
        self.assertEqual(4, v.y)
        v.y += 1
        self.assertEqual(5, v.y)
        def f():
            v.y = "g"
        self.assertRaisesRegex(
            TypeError,
            'Vector.y must be numeric, got "str"',
            f,
        )

    def test_z(self):
        v = Vector([1, 2, 3])
        self.assertEqual(3, v.z)
        v.z = 4
        self.assertEqual(4, v.z)
        v.z += 1
        self.assertEqual(5, v.z)
        def f():
            v.z = "g"
        self.assertRaisesRegex(
            TypeError,
            'Vector.z must be numeric, got "str"',
            f,
        )

    def test_r(self):
        v = Vector([1, 2, 3])
        self.assertEqual(v.r, (1 + 4 + 9) ** 0.5)
        v.r = 4
        self.assertEqual(v.r, 4)
        r, lat, lon = cartesian_to_spherical(*v.cart)
        self.assertEqual(v.r, r.value)
        self.assertEqual(v.sph[1], lat.value)
        self.assertEqual(v.sph[2], lon.value)
        def f():
            v.r = "g"
        self.assertRaisesRegex(
            TypeError,
            'Vector.r must be numeric, got "str"',
            f,
        )

    def test_lat(self):
        v = Vector([0.1, 0.2, 0.3])
        v.lat = 1.5
        self.assertEqual(v.lat, 1.5)
        r, lat, lon = cartesian_to_spherical(*v.cart)
        self.assertEqual(v.r, r.value)
        self.assertEqual(v.lat, lat.value)
        self.assertEqual(v.sph[2], lon.value)
        def f():
            v.lat = "g"
        self.assertRaisesRegex(
            TypeError,
            'Vector.lat must be numeric, got "str"',
            f,
        )

    def test_llon(self):
        v = Vector([0.1, 0.2, 0.3])
        v.lon = 1.5
        self.assertEqual(v.lon, 1.5)
        r, lat, lon = cartesian_to_spherical(*v.cart)
        self.assertEqual(v.r, r.value)
        self.assertEqual(v.lat, lat.value)
        self.assertEqual(v.lon, lon.value)
        def f():
            v.lon = "g"
        self.assertRaisesRegex(
            TypeError,
            'Vector.lon must be numeric, got "str"',
            f,
        )


class Pickle(unittest.TestCase):
    def test_pickle(self):
        v = Vector([1, 2, 3])
        p = pickle.dumps(v)
        v_load = pickle.loads(p)
        self.assertEqual(v.cart, v_load.cart)


class Math(unittest.TestCase):
    def test_to_spherical_conversion(self):
        rs = np.linspace(1e-3, 10, 11)
        lats = np.linspace(-np.pi / 2, np.pi / 2, 16)
        lons = np.linspace(0, 2 * np.pi / 2, 16)

        self.assertEqual((0, 0, 0), Vector([0, 0, 0]).sph)

        for r in rs:
            for lat in lats:
                for lon in lons:
                    x, y, z = spherical_to_cartesian(r, lat, lon)
                    vec = Vector([x.value, y.value, z.value])
                    self.assertTrue(
                        np.finfo(float).eps ** 0.5 >
                        max([abs(x - y) for x, y in zip((r, lat, lon), vec.sph)])
                    )

    def test_from_spherical_conversion(self):
        xs = np.linspace(-9, 9, 10)
        ys = np.linspace(-9, 9, 10)
        zs = np.linspace(-9, 9, 10)

        vec = Vector([0, 0, 0])
        vec.sph = (0, 1, 2)
        self.assertEqual((0, 0, 0), vec.cart)

        for x in xs:
            for y in ys:
                for z in zs:
                    r, lat, lon = cartesian_to_spherical(x, y, z)
                    vec = Vector([0, 0, 0])
                    vec.sph = (r.value, lat.value, lon.value)
                    self.assertTrue(
                        np.finfo(float).eps ** 0.5 >
                        max([abs(x - y) for x, y in zip((x, y, z), vec.cart)])
                    )

    def test_dot(self):
        v1 = Vector([1, 2, 3])
        v2 = Vector([0.5, 2, 1])
        self.assertEqual(v1.dot(v2), v2.dot(v1))
        self.assertEqual(v1.dot(v2), 0.5 * 1 + 2 * 2 + 3 * 1)
        self.assertRaisesRegex(
            ValueError,
            'Vector.dot takes another Vector as an argument, got object',
            lambda: v1.dot(object())
        )

    def test_abs(self):
        v = Vector([1, 2, 3])
        self.assertEqual(abs(v), (1 + 4 + 9) ** 0.5)
