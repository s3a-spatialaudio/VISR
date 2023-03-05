# Original source https://stackoverflow.com/questions/51939257/how-do-you-get-the-filename-of-a-python-wheel-when-running-setup-py

from setuptools.dist import Distribution
from distutils.core import Extension

def wheel_name(**kwargs):
    """
    Return the computed function name for a binary platform wheel.

    This is used by the build system where we need to know the name of the
    output file beforehand.
    """
    # create a fake distribution from arguments
    dist = Distribution(attrs=kwargs)
    # finalize bdist_wheel command
    bdist_wheel_cmd = dist.get_command_obj('bdist_wheel')
    bdist_wheel_cmd.ensure_finalized()
    # assemble wheel file name
    distname = bdist_wheel_cmd.wheel_dist_name
    tag = '-'.join(bdist_wheel_cmd.get_tag())
    return f'{distname}-{tag}.whl'

# Allow invocation as a script.
if __name__ == "__main__":
    
    from argparse import ArgumentParser
    
    parser = ArgumentParser(
                    prog = "getPythonBinaryWheelName" )
    
    parser.add_argument('dist')
    parser.add_argument('-v', '--version')
    args = parser.parse_args()

    # The ext_modules parameter with an empty extension module ensures 
    # that the generated name represents a platform wheel.
    res = wheel_name(name=args.dist, version=args.version,  
                     ext_modules=[Extension("foo", [])])
    print(res)
