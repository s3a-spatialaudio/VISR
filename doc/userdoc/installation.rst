Installing VISR
================================

Binary installation packages are the suggested way to uses the VISR framework.
A binary installers enables all uses of the framework, including

* Running standalone applications
* Using DAW plugins based on the VISR 
* Using the Python interfaces and creating new functionality in Python
* Creating standalone applications and extension libraries in C++

.. hint:: Building the VISR from source is necessary only in these cases:
    - Porting it to a platform where no binary installer exists
    - Fixing or changing the internal workings of the framework.

Windows
-------

The graphical installer is provides as an **.exe** file and provides a dialog-based, component-enables installation.
Figure figure_windows_installer_ shows the component selection dialog of the installer. The choices are detailed below in section :ref:`installation_component_description`.

.. _figure_windows_installer:
.. figure:: ../images/windows_installer.png
   :scale: 100 %
   :align: center   
   
   Graphical Windows installer.

An executable installer (.exe) with a graphical user interface and corresponding uninstall functionality. Supported are 64-bit versions of Windows.
If required, install the "Microsoft Visual C++ Redistributable for Visual Studio 2017", package,
for example from the `Visual C++ downloads page <https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads>`_.

On Windows, it is necessary to add the directory containing the VISR libraries (DLLs) as well as the directory containing third-party libraries shipped with the VISR installer to the **PATH** variable.
To this end, open the environment variable editor (Settings -> System -> Advanced system settings -> Environment variables).
The environment variable on Windows 10 is depicted in figure windows_environment_variables_editor .

.. _windows_environment_variables_editor:
.. figure:: ../images/windows_environment_variables_editor.png
   :scale: 50 %
   :align: center   
   
   Environment variable editor on Windows 10.

Append the value "C:\ Program Files\VISR-X.X.X\lib;C:\ Program Files\VISR-X.X.X\3rd" if the standard installation location was used. (Note: Replace **X.X.X** with the actual version number of VISR.
Depending on your system permissions and whether you VISR shall be used by all users of the computer, you can either set the **PATH** user variable or the **PATH** system variable.

.. note::
   Any applications used to access VISR (for example command line terminals, Python development environments, or DAWs) must be closed and reopened before the changed paths take effect.

Append the path ''<install-directory>/lib'' to the path variable, where ''install_diectory'' is the directory specified during the installation.
For the default path, the setting would be ''c:\Program Files\VISR-N.N.N\lib'', where ''N.N.N is replaced by the actual version number.
If the **PATH** variable is edited as a string, subsequent paths are separated by semicolons.

.. note:: Future versions of the installer might adjust the paths automatically.
   However, as pointed out in
   `NSIS Path manipulation <http://nsis.sourceforge.net/Path_Manipulation>`_,
   this needs an extremely cautious implementation to avoid potential damage to users' systems.

Mac OS X
--------
An installer with a graphical user interface guides through the installation process and allows the selection of optional components.
Figure figure_visr_macos_installer_ shows a screenshot of this installer.
By default, it installs the VISR into the directory **/Applications/VISR-X.X.X/** where **X.X.X** denotes the version number.

.. _figure_visr_macos_installer:
.. figure:: ../images/visr_macos_installer.png
   :scale: 50 %
   :align: center   
   
   Component-based installer for Mac OS X.

Linux
-----

For Linux, installation packages are provided as *.deb* (Debian) packages.
At the moment, this package is monolithic, i.e., it contains all components.
They are installed via the command

.. code-block:: bash

   sudo apt install VISR-<version>.deb

If this command reports missing dependencies, these can be installed subsequently with the command 

.. code-block:: bash

   sudo apt install --fix-broken

After that the framework is ready to use.

.. _installation_component_description:

Installation components
=======================

With the dialog-based, component-enabled installers, parts of the framework can be chosen depending on the intended use of the framework.




Setting up Python
=================

As explained in section :ref:`overview_python_integration`, the Python integration is an optional, albeit central, part of the VISR framework that enables a number of its functionalities, for example:

* Using the framework interactively from a Python interpreter.
* Using application that use Python internally, for instance the Binaural Synthesis Toolkit or metadata adaptation processes using the metadapter. 
* Creating new signal flows or algorithms in Python.

To use these functionalities, a Python 3 distribution must be installed on the computer, and some configuration steps are required.

Basically, t

