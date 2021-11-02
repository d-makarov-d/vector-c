#include "utils.h"

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
