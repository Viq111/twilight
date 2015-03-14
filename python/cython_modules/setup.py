from distutils.core import setup
from Cython.Build import cythonize

setup(
  name = 'twilight_client_api',
  ext_modules = cythonize("client_api_cython.pyx"),
)
