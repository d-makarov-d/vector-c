#include "utils.h"
#include <math.h>

// DOESN'T WORK WITH __FASTMATH__ !!!
bool isnan(double val) {
    return val != val;
}

/* Hash for double arrays.
From https://github.com/python/cpython/blob/main/Objects/tupleobject.c
CPython tuple hash implementation
*/
#if SIZEOF_PY_UHASH_T > 4
#define _PyHASH_XXPRIME_1 ((Py_uhash_t)11400714785074694791ULL)
#define _PyHASH_XXPRIME_2 ((Py_uhash_t)14029467366897019727ULL)
#define _PyHASH_XXPRIME_5 ((Py_uhash_t)2870177450012600261ULL)
#define _PyHASH_XXROTATE(x) ((x << 31) | (x >> 33))  /* Rotate left 31 bits */
#else
#define _PyHASH_XXPRIME_1 ((Py_uhash_t)2654435761UL)
#define _PyHASH_XXPRIME_2 ((Py_uhash_t)2246822519UL)
#define _PyHASH_XXPRIME_5 ((Py_uhash_t)374761393UL)
#define _PyHASH_XXROTATE(x) ((x << 13) | (x >> 19))  /* Rotate left 13 bits */
#endif

/* Tests have shown that it's not worth to cache the hash value, see
   https://bugs.python.org/issue9685 */
Py_hash_t arr_hash(double v[], Py_ssize_t len) {
    Py_uhash_t acc = _PyHASH_XXPRIME_5;
    for (Py_ssize_t i = 0; i < len; i++) {
        Py_uhash_t lane = _Py_HashDouble(v[i]);
        if (lane == (Py_uhash_t)-1) {
            return -1;
        }
        acc += lane * _PyHASH_XXPRIME_2;
        acc = _PyHASH_XXROTATE(acc);
        acc *= _PyHASH_XXPRIME_1;
    }

    /* Add input length, mangled to keep the historical value of hash(()). */
    acc += len ^ (_PyHASH_XXPRIME_5 ^ 3527539UL);

    if (acc == (Py_uhash_t)-1) {
        return 1546275796;
    }
    return acc;
}

bool arr_cmp(double a[], double b[], Py_ssize_t n) {
    for (Py_ssize_t i=0; i<n; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

bool check_float(PyObject *query, double *target) {
    if (PyFloat_Check(query)) {
        *target = PyFloat_AsDouble(query);
        return true;
    }
    if (PyLong_Check(query)) {
        *target = PyLong_AsDouble(query);
        return true;
    }
    return false;
}

int check_array(PyObject *arr, double target[], const char *value_name) {
    if (Py_TYPE(arr)->tp_iter != NULL) {
        PyObject *iter = PyObject_GetIter(arr);
        PyObject *itm;
        for (int i=0; i<3; i++) {
            itm = PyIter_Next(iter);
            if (itm == NULL) {
                char *msg;
                asprintf(
                        &msg,
                        "%s must contain 3 elements, got %i",
                        value_name, i
                );
                PyErr_SetString(PyExc_ValueError, msg);
                Py_DECREF(msg);
                Py_DECREF(iter);
                return -1;
            }
            if (check_float(itm, target + i))
                continue;
            char *msg;
            asprintf(
                    &msg,
                    "%s must contain numeric values, got \"%s\" at %i",
                    value_name, Py_TYPE(itm)->tp_name, i
            );
            PyErr_SetString(PyExc_TypeError, msg);
            Py_DECREF(msg);
            Py_DECREF(itm);
            Py_DECREF(iter);
            return -1;
        }
        Py_DECREF(itm);
        if (PyIter_Next(iter) != NULL) {
            char *msg;
            asprintf(
                    &msg,
                    "%s must contain 3 elements, got more",
                    value_name
            );
            PyErr_SetString(PyExc_ValueError, msg);
            Py_DECREF(msg);
            Py_DECREF(iter);
            return -1;
        }
    } else if(PySequence_Check(arr)) {
        if (PySequence_Size(arr) != 3) {
            char *msg;
            asprintf(
                    &msg,
                    "%s must contain 3 elements, got %ld",
                    value_name, PySequence_Size(arr)
            );
            PyErr_SetString(PyExc_ValueError, msg);
            Py_DECREF(msg);
            return -1;
        }
        PyObject *itm;
        for (int i=0; i<3; i++) {
            itm = PySequence_GetItem(arr, i);
            if (check_float(itm, target + i))
                continue;
            char *msg;
            asprintf(
                    &msg,
                    "%s must contain numeric values, got \"%s\" at %i",
                    value_name, Py_TYPE(itm)->tp_name, i
            );
            PyErr_SetString(PyExc_TypeError, msg);
            Py_DECREF(msg);
            Py_DECREF(itm);
            return -1;
        }
        Py_DECREF(itm);
    } else {
        char *msg;
        asprintf(
                &msg,
                "%s must be an Sequence or Iterable, got \"%s\"",
                value_name, Py_TYPE(arr)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }

    return 0;
}

bool is_subclass(PyObject *query, PyTypeObject *cls, const char *operand) {
    if (PyObject_TypeCheck(query, cls) == 0) {
        char *msg;
        asprintf(
                &msg,
                "unsupported operand type(s) for %s: '%s' and '%s'",
                operand, cls->tp_name, Py_TYPE(query)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return false;
    }
    return true;
};

void spherical_to_cartesian_3(double sph[], double cart[]) {
    double r = sph[0];
    double lat = sph[1];
    double lon = sph[2];

    cart[0] = cos(lon) * cos(lat) * r;   // X
    cart[1] = sin(lon) * cos(lat) * r;   // Y
    cart[2] = sin(lat) * r;              // Z
}

double r_from_cartesian(double cart[]) {
    double x = cart[0];
    double y = cart[1];
    double z = cart[2];

    return sqrt(x*x + y*y + z *z);
}

double lat_from_cartesian(double cart[], double r) {
    // r passed to not recalculate it
    double z = cart[2];

    if (r == 0.d)
        return 0.d;

    return asin(z / r);
}

double lon_from_cartesian(double cart[]) {
    double x = cart[0];
    double y = cart[1];

    return atan2(y, x);
}
