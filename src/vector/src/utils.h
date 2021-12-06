#ifndef UTILS_H
#define UTILS_H
#include <Python.h>

typedef enum { false, true } bool;

// DOESN'T WORK WITH __FASTMATH__ !!!
bool isnan(double);
Py_hash_t arr_hash(double [], Py_ssize_t);
bool arr_cmp(double [], double[], Py_ssize_t);

bool check_float(PyObject *, double *);
int check_array(PyObject *arr, double target[], const char *value_name);
bool is_subclass(PyObject *, PyTypeObject *, const char *);

void spherical_to_cartesian_3(double sph[], double cart[]);

double r_from_cartesian(double[]);
double lat_from_cartesian(double[], double);
double lon_from_cartesian(double[]);

#endif