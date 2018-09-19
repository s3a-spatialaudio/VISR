Installing binary versions
================================


Windows
-------

An executable installer (.exe) with a graphical user interface and corresponding uninstall functionality. Supported are 64-bit versions of Windows.
If required, install the "Microsoft Visual C++ Redistributable for Visual Studio 2017", package,
for example from the `Visual C++ downloads page <https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads>`_.

On Windows, it is necessary to add the directory containing the VISR libraries (DLLs) to the **PATH** variable.
To this end, open the environment variable editor (Settings -> System -> Advanced system settings -> Environment variables).
The environment variable on Windows 10 is depicted in this figure:

.. figure::
   :scale: 50 %
   :align: center   
   
   Environment variable editor on Windows 10.

Depending on your system permissions and whether you VISR shall be used by all users of the computer, you can either set the **PATH** user variable or the **PATH** system variable.
Append the path ''<install-directory>/lib'' to the path variable, where ''install_diectory'' is the directory specified during the installation.
For the default path, the setting would be ''c:\Program Files\VISR-N.N.N\lib'', where ''N.N.N is replaced by the actual version number.
If the **PATH** variable is edited as a string, subsequent paths are separated by semicoli.

.. note:: Future versions of the installer might adjust the paths automatically.
	  However, as pointed out in
	  `NSIS Path manipulation <http://nsis.sourceforge.net/Path_Manipulation>`_,
	  this needs an extremely cautious implementation to avoid damage to users' systems.

Mac OS X
--------
An installer with a graphical user interface guides through the installation process and allows the selection of optional components.

.. todo:: Add screenshot of Mac OS X installer

Linux
-----

For Linux, installation packages are provided as *.deb* (Debian) packages. They are installed via the command

.. code-block:: bash
		
   sudo apt install VISR-<version>.deb

If this command reports missing dependencies, these can be installed subsequently with the command 

.. code-block:: bash
		
   sudo apt install --fix-broken

After that, the framework is ready to use.
