#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include <stdio.h>
#include "structmember.h"
#include "utils.h"

typedef struct {
    PyObject_HEAD
    double cart[3];
    double sph[3];
} VectorObject;
static PyTypeObject VectorType;

void clear_arr(double arr[], int n) {
    for (int i=0; i<n; i++)
        arr[i] = NAN;
}

static void
Vector_dealloc(VectorObject *self) {
    Py_XDECREF(self->cart);
    Py_XDECREF(self->sph);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Vector_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    VectorObject *self;
    self = (VectorObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        clear_arr(self->sph, 3);
    }
    return (PyObject *) self;
}

static int
Vector_init(VectorObject *self, PyObject *args, PyObject *kwds) {
    PyObject* cart = NULL;
    if (!PyArg_ParseTuple(args, "|O", &cart)) return -1;
    int res = check_array(cart, self->cart, "Vector constructor first argument");
    Py_DECREF(cart);
    return res;
}
// Cartesian -----------------------------------------------------------------------------------------------------------
static PyObject* get_cart(VectorObject *self, void * closure) {
    return Py_BuildValue("(ddd)", self->cart[0], self->cart[1], self->cart[2]);
}

static int
set_cart(VectorObject *self, PyObject *cart, void* closure) {
    int res = check_array(cart, self->cart, "Vector cartesian component");
    // clear spherical coordinates, since they are no more valid
    if (res == 0)
        clear_arr(self->sph, 3);

    return res;
}

static PyObject* get_x(VectorObject *self, void * closure) {
    return Py_BuildValue("d", self->cart[0]);
}

static int
set_x(VectorObject *self, PyObject *x, void* closure) {
    bool res = check_float(x, self->cart);
    // clear spherical coordinates, since they are no more valid
    if (res) {
        clear_arr(self->sph, 3);
        return 0;
    } else {
        char *msg;
        asprintf(
                &msg,
                "Vector.x must be numeric, got \"%s\"",
                Py_TYPE(x)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }
}

static PyObject* get_y(VectorObject *self, void * closure) {
    return Py_BuildValue("d", self->cart[1]);
}

static int
set_y(VectorObject *self, PyObject *y, void* closure) {
    bool res = check_float(y, self->cart + 1);
    // clear spherical coordinates, since they are no more valid
    if (res) {
        clear_arr(self->sph, 3);
        return 0;
    } else {
        char *msg;
        asprintf(
                &msg,
                "Vector.y must be numeric, got \"%s\"",
                Py_TYPE(y)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }
}

static PyObject* get_z(VectorObject *self, void * closure) {
    return Py_BuildValue("d", self->cart[2]);
}

static int
set_z(VectorObject *self, PyObject *z, void* closure) {
    bool res = check_float(z, self->cart + 2);
    // clear spherical coordinates, since they are no more valid
    if (res) {
        clear_arr(self->sph, 3);
        return 0;
    } else {
        char *msg;
        asprintf(
                &msg,
                "Vector.z must be numeric, got \"%s\"",
                Py_TYPE(z)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }
}

// Spherical -----------------------------------------------------------------------------------------------------------
static PyObject* get_sph(VectorObject *self, void * closure) {
    // R
    if (isnan(self->sph[0]))
        self->sph[0] = r_from_cartesian(self->cart);
    // lat
    if (isnan(self->sph[1]))
        self->sph[1] = lat_from_cartesian(self->cart, self->sph[0]);
    // lon
    if (isnan(self->sph[2]))
        self->sph[2] = lon_from_cartesian(self->cart);
    return Py_BuildValue("(ddd)", self->sph[0], self->sph[1], self->sph[2]);
}

static int
set_sph(VectorObject *self, PyObject *sph, void* closure) {
    int res = check_array(sph, self->sph, "Vector spherical component");
    if (res != 0) {
        clear_arr(self->sph, 3);
        return res;
    }
    spherical_to_cartesian_3(self->sph, self->cart);
    return 0;
}

static PyObject* get_r(VectorObject *self, void * closure) {
    if (isnan(self->sph[0]))
        self->sph[0] = r_from_cartesian(self->cart);

    return Py_BuildValue("d", self->sph[0]);
}

static int
set_r(VectorObject *self, PyObject *r, void* closure) {
    // R
    if (isnan(self->sph[0]))
        self->sph[0] = r_from_cartesian(self->cart);
    // lat
    if (isnan(self->sph[1]))
        self->sph[1] = lat_from_cartesian(self->cart, self->sph[0]);
    // lon
    if (isnan(self->sph[2]))
        self->sph[2] = lon_from_cartesian(self->cart);

    bool res = check_float(r, self->sph);
    if (res) {
        // Update cartesian coordinates
        spherical_to_cartesian_3(self->sph, self->cart);
        return 0;
    } else {
        char *msg;
        asprintf(
                &msg,
                "Vector.r must be numeric, got \"%s\"",
                Py_TYPE(r)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }
}

static PyObject* get_lat(VectorObject *self, void * closure) {
    if (isnan(self->sph[0]))
        self->sph[0] = r_from_cartesian(self->cart);
    if (isnan(self->sph[1]))
        self->sph[1] = lat_from_cartesian(self->cart, self->sph[0]);

    return Py_BuildValue("d", self->sph[1]);
}

static int
set_lat(VectorObject *self, PyObject *lat, void* closure) {
    // R
    if (isnan(self->sph[0]))
        self->sph[0] = r_from_cartesian(self->cart);
    // lat
    if (isnan(self->sph[1]))
        self->sph[1] = lat_from_cartesian(self->cart, self->sph[0]);
    // lon
    if (isnan(self->sph[2]))
        self->sph[2] = lon_from_cartesian(self->cart);

    bool res = check_float(lat, self->sph + 1);
    if (res) {
        // Update cartesian coordinates
        spherical_to_cartesian_3(self->sph, self->cart);
        return 0;
    } else {
        char *msg;
        asprintf(
                &msg,
                "Vector.lat must be numeric, got \"%s\"",
                Py_TYPE(lat)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }
}

static PyObject* get_lon(VectorObject *self, void * closure) {
    if (isnan(self->sph[2]))
        self->sph[2] = lon_from_cartesian(self->cart);

    return Py_BuildValue("d", self->sph[2]);
}

static int
set_lon(VectorObject *self, PyObject *lon, void* closure) {
    // R
    if (isnan(self->sph[0]))
        self->sph[0] = r_from_cartesian(self->cart);
    // lat
    if (isnan(self->sph[1]))
        self->sph[1] = lat_from_cartesian(self->cart, self->sph[0]);
    // lon
    if (isnan(self->sph[2]))
        self->sph[2] = lon_from_cartesian(self->cart);

    bool res = check_float(lon, self->sph + 2);
    if (res) {
        // Update cartesian coordinates
        spherical_to_cartesian_3(self->sph, self->cart);
        return 0;
    } else {
        char *msg;
        asprintf(
                &msg,
                "Vector.lon must be numeric, got \"%s\"",
                Py_TYPE(lon)->tp_name
        );
        PyErr_SetString(PyExc_TypeError, msg);
        Py_DECREF(msg);
        return -1;
    }
}

// Algebra -------------------------------------------------------------------------------------------------------------
static PyObject *
Vector_add(PyObject *self, PyObject *other) {
    if (!is_subclass(other, &VectorType, "+"))
        return NULL;

    PyObject *args = Py_BuildValue(
        "((ddd))",
         ((VectorObject *)self)->cart[0] + ((VectorObject *)other)->cart[0],
         ((VectorObject *)self)->cart[1] + ((VectorObject *)other)->cart[1],
         ((VectorObject *)self)->cart[2] + ((VectorObject *)other)->cart[2]
    );

    PyObject *obj = PyObject_CallObject((PyObject *) &VectorType, args);

    Py_DECREF(args);

    return obj;
}

static PyObject *
Vector_sub(PyObject *self, PyObject *other) {
    if (!is_subclass(other, &VectorType, "-"))
        return NULL;

    PyObject *args = Py_BuildValue(
        "((ddd))",
         ((VectorObject *)self)->cart[0] - ((VectorObject *)other)->cart[0],
         ((VectorObject *)self)->cart[1] - ((VectorObject *)other)->cart[1],
         ((VectorObject *)self)->cart[2] - ((VectorObject *)other)->cart[2]
    );

    PyObject *obj = PyObject_CallObject((PyObject *) &VectorType, args);

    Py_DECREF(args);

    return obj;
}

static PyObject *
Vector_mul(PyObject *self, PyObject *other) {
    double d;
    if (check_float(other, &d)) {
        PyObject *args = Py_BuildValue(
            "((ddd))",
             d * ((VectorObject *)self)->cart[0],
             d * ((VectorObject *)self)->cart[1],
             d * ((VectorObject *)self)->cart[2]
        );

        PyObject *obj = PyObject_CallObject((PyObject *) &VectorType, args);

        Py_DECREF(args);

        return obj;
    } else if (is_subclass(other, &VectorType, "*")) {
        // Py_INCREF(((VectorObject *)self)->cart);
        // Py_INCREF(((VectorObject *)other)->cart);

        double *a = ((VectorObject *)self)->cart;
        double *b = ((VectorObject *)other)->cart;

        PyObject *args = Py_BuildValue(
            "((ddd))",
             a[1] * b[2] - a[2] * b[1],
             a[2] * b[0] - a[0] * b[2],
             a[0] * b[1] - a[1] * b[0]
        );

        PyObject *obj = PyObject_CallObject((PyObject *) &VectorType, args);

        Py_DECREF(args);

        // Py_DECREF(((VectorObject *)self)->cart);
        // Py_DECREF(((VectorObject *)other)->cart);
        return obj;
    } else {
        return NULL;
    }
}

static PyObject *
Vector_neg(VectorObject *self) {
    PyObject *args = Py_BuildValue(
        "((ddd))",
         - self->cart[0],
         - self->cart[1],
         - self->cart[2]
    );

    PyObject *obj = PyObject_CallObject((PyObject *) &VectorType, args);

    Py_DECREF(args);

    return obj;
}

static PyObject *
Vector_abs(VectorObject *self) {
    return get_r(self, NULL);
}

static PyObject *
Vector_dot(VectorObject *self, PyObject *args) {
    PyObject* other = NULL;
    if (!PyArg_ParseTuple(args, "|O", &other)) return NULL;
    if (!is_subclass(other, &VectorType, "dot")) {
        PyErr_Clear();
        char *msg;
        asprintf(
                &msg,
                "Vector.dot takes another Vector as an argument, got %s",
                Py_TYPE(other)->tp_name
        );
        PyErr_SetString(PyExc_ValueError, msg);
        Py_DECREF(msg);
        return NULL;
    }
    float res = 0;
    for (int i = 0; i< 3; i++) {
        res += self->cart[i] * ((VectorObject *) other)->cart[i];
    }
    return Py_BuildValue("d", res);
}

static PyNumberMethods Vector_as_number = {
    .nb_add = Vector_add,
    .nb_subtract = Vector_sub,
    .nb_multiply = Vector_mul,
    .nb_negative = (unaryfunc) Vector_neg,
    .nb_absolute = (unaryfunc) Vector_abs,
};

static PyGetSetDef Vector_get_sets[] = {
    {"cart", (getter) get_cart, (setter) set_cart, "Cartesian components", NULL},
    {"x", (getter) get_x, (setter) set_x, "Cartesian \"X\" component", NULL},
    {"y", (getter) get_y, (setter) set_y, "Cartesian \"Y\" component", NULL},
    {"z", (getter) get_z, (setter) set_z, "Cartesian \"Z\" component", NULL},
    {"sph", (getter) get_sph, (setter) set_sph, "Spherical components", NULL},
    {"r", (getter) get_r, (setter) set_r, "Spherical \"R\" component", NULL},
    {"lat", (getter) get_lat, (setter) set_lat, "Spherical \"LAT\" component", NULL},
    {"lon", (getter) get_lon, (setter) set_lon, "Spherical \"LON\" component", NULL},
    {NULL}
};

static Py_hash_t
Vector_hash(VectorObject *self) {
    return arr_hash(self->cart, 3);
}

static PyObject *
Vector_richcompare(PyObject *self, PyObject *other, int op) {
    if (PyObject_TypeCheck(other, &VectorType) == 0)
        return Py_NotImplemented;

    if (op == Py_EQ) {
        bool res = arr_cmp(((VectorObject *)self)->cart, ((VectorObject *)other)->cart, 3);
        if (res)
            return Py_True;
        return Py_False;
    } else if (op == Py_NE) {
        bool res = arr_cmp(((VectorObject *)self)->cart, ((VectorObject *)other)->cart, 3);
        if (!res)
            return Py_True;
        return Py_False;
    } else {
        return Py_NotImplemented;
    }
}

static PyObject *Vector_repr(VectorObject *self) {
    char *msg;
    asprintf(
        &msg,
        "%s([%f, %f, %f])",
        Py_TYPE(self)->tp_name, self->cart[0], self->cart[1], self->cart[2]
    );
    PyObject *str = Py_BuildValue("s", msg);
    Py_DECREF(msg);

    return str;
}

static PyObject *Vector_str(VectorObject *self) {
    char *msg;
    asprintf(
        &msg,
        "[%f, %f, %f>",
        self->cart[0], self->cart[1], self->cart[2]
    );
    PyObject *str = Py_BuildValue("s", msg);
    Py_DECREF(msg);

    return str;
}

static PyObject *
Vector___getstate__(VectorObject *self, PyObject *Py_UNUSED(ignored)) {
    return Py_BuildValue(
        "{s(ddd)s(ddd)}",
        "cart", self->cart[0], self->cart[1], self->cart[2],
        "sph", self->sph[0], self->sph[1], self->sph[2]
    );
}

static PyObject *
Vector___setstate__(VectorObject *self, PyObject *state) {
    if (!PyDict_CheckExact(state)) {
        PyErr_SetString(PyExc_ValueError, "Pickled object is not a dict.");
        return NULL;
    }

    PyObject* cart = PyDict_GetItemString(state, "cart");
    if (cart == NULL) {
        PyErr_SetString(PyExc_KeyError, "No \"cart\" in pickled dict.");
        return NULL;
    }
    for (int i=0; i < 3; i++)
        self->cart[i] = PyFloat_AsDouble(PyTuple_GetItem(cart, i));

    PyObject* sph = PyDict_GetItemString(state, "sph");
    if (sph == NULL) {
        PyErr_SetString(PyExc_KeyError, "No \"sph\" in pickled dict.");
        return NULL;
    }
    for (int i=0; i < 3; i++)
        self->sph[i] = PyFloat_AsDouble(PyTuple_GetItem(sph, i));

    Py_RETURN_NONE;
}

static PyMethodDef Vector_methods[] = {
    {"dot", (PyCFunction) Vector_dot, METH_VARARGS, "Vectors dot product"},
    {"__getstate__", (PyCFunction) Vector___getstate__, METH_NOARGS, "Pickle"},
    {"__setstate__", (PyCFunction) Vector___setstate__, METH_O, "UnPickle"},
    {NULL}
};

static PyTypeObject VectorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "vector.Vector",
    .tp_doc = "Vector object",
    .tp_basicsize = sizeof(VectorObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_init = (initproc) Vector_init,
    .tp_dealloc = (destructor) Vector_dealloc,
    .tp_new = Vector_new,
    .tp_getset = Vector_get_sets,
    .tp_methods = Vector_methods,
    .tp_as_number = &Vector_as_number,
    .tp_hash = (hashfunc) Vector_hash,
    .tp_richcompare = Vector_richcompare,
    .tp_repr = (reprfunc)Vector_repr,
    .tp_str = (reprfunc)Vector_str,
};

static PyModuleDef vectormodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "vector",
    .m_doc = "Vector algebra module.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_vector(void) {
    PyObject *m;
    if (PyType_Ready(&VectorType) < 0)
        return NULL;

    m = PyModule_Create(&vectormodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&VectorType);
    if (PyModule_AddObject(m, "Vector", (PyObject *) &VectorType) < 0) {
        Py_DECREF(&VectorType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

