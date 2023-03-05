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

cookie = os.add_dll_directory(
   os.path.join(os.path.dirname(os.path.realpath(__file__)), 'impl'))
from .impl.visr import *
# from .impl.audiointerfaces import audiointerfaces

from .impl.efl import efl
# from .impl.efl import efl
# from .impl.objectmodel import objectmodel
# from .impl.panning import panning
# from .impl.panningdsp import panningdsp
# from .impl.pml import pml
# from .impl.rbbl import rbbl
# from .impl.reverbobject import reverbobject
# from .impl.rcl import rcl
# from .impl.rrl import rrl
# from .impl.signalflows import signalflows

cookie.close()
