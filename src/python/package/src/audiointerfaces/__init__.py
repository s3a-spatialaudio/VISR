import os
from contextlib import contextmanager

if hasattr(os, 'add_dll_directory'):
   cookie = os.add_dll_directory(
      os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', 'impl'))
else:
   cookie = None
from ..impl.audiointerfaces import *

if cookie is not None:
   cookie.close()
