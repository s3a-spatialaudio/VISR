import os
from ..impl.dll_path_context import dll_path_context


with dll_path_context(
  os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', 'impl')):
    from ..impl.objectmodel import *
