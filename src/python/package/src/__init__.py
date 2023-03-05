import os
from contextlib import contextmanager

# @contextmanager
# def dll_path_context(path):
#     """
#     Create a context where DLLs within the directory "path" can be loaded.
#     This is necessary with Python >= 3.8 on Windows, where DLL loading is
#     prohibited by default.
#     """
#     if hasattr( os, "add_dll_directory"):
#         cookie = os.add_dll_directory(os.abspath(path))
#         try:
#             yield
#         finally:
#             cookie.close()
#
# with dll_path_context(os.path.join(os.getcwd(), 'impl')):
#     from .impl.visr import *

if hasattr(os, 'add_dll_directory'):
   cookie = os.add_dll_directory(
      os.path.join(os.path.dirname(os.path.realpath(__file__)), 'impl'))
else:
   cookie = None
from .impl.visr import *

if cookie is not None:
   cookie.close()
