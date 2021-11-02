from setuptools import setup, Extension

module = Extension('vector', sources=['src/vector.c', 'src/utils.c'])

setup(
    name='Vector',
    version='1.0',
    description='Cpp implementation for vector class',
    ext_modules=[module]
)
