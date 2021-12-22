#include "utils.h"
#include <math.h>

// DOESN'T WORK WITH __FASTMATH__ !!!
bool isnan(double val) {
    return val != val;
}

Py_hash_t arr_hash(double v[], Py_ssize_t len) {
    PyObject *tup = Py_BuildValue("(ddd)", v[0], v[1], v[2]);
    return PyObject_Hash(tup);
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
