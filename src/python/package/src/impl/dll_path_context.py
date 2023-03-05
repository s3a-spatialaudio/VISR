import os
from contextlib import contextmanager

@contextmanager
def dll_path_context(path):
    """
    Create a context where DLLs within the directory "path" can be loaded.
    This is necessary with Python >= 3.8 on Windows, where DLL loading is
    prohibited by default.
    """
    if hasattr( os, "add_dll_directory"):
        cookie = os.add_dll_directory(os.path.abspath(path))
        try:
            yield
        finally:
            cookie.close()
