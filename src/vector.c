#define PY_SSIZE_T_CLEAN
#include <Python.h>
// #include <stdio.h>
#include "structmember.h"
#include "utils.h"

typedef struct {
    PyObject_HEAD
    double cart[3];
} VectorObject;


static void
Vector_dealloc(VectorObject *self) {
    Py_XDECREF(self->cart);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Vector_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    VectorObject *self;
    self = (VectorObject *) type->tp_alloc(type, 0);
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

static PyObject* get_cart(VectorObject *self, void * closure) {
    return Py_BuildValue("(ddd)", self->cart[0], self->cart[1], self->cart[2]);
}

static PyGetSetDef Vector_get_sets[] = {
    {"cart", (getter) get_cart, NULL, "Cartesian components", NULL},
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

