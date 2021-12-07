from setuptools import setup, Extension

module = Extension('vector', sources=['src/vector/src/vector.c', 'src/vector/src/utils.c'])

setup(
    name='vector-c',
    version='1.1.0',
    description='Cpp implementation for 3-dimensional vector',
    ext_modules=[module]
)
