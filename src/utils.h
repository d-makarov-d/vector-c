#ifndef UTILS_H
#define UTILS_H
#include <Python.h>

typedef enum { false, true } bool;

bool check_float(PyObject *, double *);
int check_array(PyObject *arr, double target[], const char *value_name);

#endif