Setting up a build environment
==============================

Standard build setup
--------------------

Git
---
 * On Linux git can be installed through the distribution's package manager. On Ubuntu, Raspbian, and Debian-based distribtution, the command is ``sudo apt install git``.
 * On Mac OS X 10.9 and above, the git command line tool is installed on its first invocation (source: `GIT SCM book <https://git-scm.com/book/>`_).
 * On Windows, we suggest the `Git for Windows <htps://git-scm.com/download/win>`_ binaries.
 
 Alternatively you can choose from a number of GUIs, for example `SourceTree <https://www.sourcetreeapp.com/>`_ or `GitKraken <https://www.gitkraken.com/>`_, or the support in modern IDEs as Microsoft Visual Studio or Apple XCode.

CMake
---------------------
VISR uses `CMake <http://www.cmake.org/>`_ as a portable build tool
The minimum required version is CMake 3.1.

CMake can be used as a command line tool but also provides a GUI (cmake-gui) for configuring builds.

* On Linux, install throught he distribution's package manager. On Debian-based systems, the packages are named ``cmake`` for the command line tools and ``cmake-qt-gui`` for the graphical user interface.
* On Windows and Mac OS X, download 


* If you plan to use the machine for CI (continuous integration tasks), make sure you add it to the system path (Windows: Select "Add CMake to the system PATH for all users")

Documentation
-----------------------

The VISR documentation is mainly written as ReStructured text documents (using `Sphinx <http://www.sphinx-doc.org/>`_ to create web pages and PDF documents), whereas the code documentation pulled into these documents is generated through `Doxygen <https://www.doxygen.nl>`_.

In order to create the user and API documentation, the following software tools must be installed:

* Doxygen: On Windows and Mac OS X, we recommend downloading binary packages from the `Doxygen download page <https://www.doxygen.nl/download.html>`_ and installing them. On Linux, installing via the distribution-specific package manager (e.g., ``sudo apt install doxygen``. Note that the XML output generation needed for subsequent build stages is quite buggy in older Doxygen releases, it might therefore be advisable to upgrade to a recent Doxygen release or to build from source (especially on Linux, where the version provided by the package manager could be rather dated).

* LaTeX: Both the Doxygen documentation and the PDF generation of Sphinx require a LaTeX system to be installed. We recommend the following LaTeX distributions:

    - Linux: The system-provided LaTeX (installed, e.g., through ``sudo apt install texlive-full`` ``sudo apt install texlive`` on Ubuntu)
	  - Mac OS X: `MacTeX <http://www.tug.org/mactex/>`_
	  - Windows:  `MikTeX <http://www.miktex.org>`_
	
  Note that - unless you install a "full" distribution - LaTeX packages might be missing and would need to be installed as needed. So look out for error messages when running the documentation generation for the first time, and use the distribution's package manager to install the missing packages as needed.
  
  After installing LaTeX and running the CMake configuration stage with the option BUILD_DOCUMENTATION enabled, check for errors and for missing binary paths. In particular, if ``PDFLATEX_COMPILER`` is not found, fix it as follows:
   * Windows: Set the CMake variable ``MIKTEX_BINARY_PATH`` to the directory containing the MikTeX binaries, e.g., ``C:/Program Files/MiKTeX 2.9/miktex/bin/x64``
   * Mac OS X: Set ``PDFLATEX_COMPILER`` to the result of the shell command ``which pdflatex``. On a recent MacTeX installation, this yields ``/Library/TeX/texbin\pdflatex/``.

* Sphinx: Sphinx is typically bundled with the Python distribution you are using and should therefore be installed through the package management system of this distribution.

    - Linux: use the package manager of the Linux distribution, e.g., ``sudo apt install python3-sphinx``.
	- Anaconda (recommended distribution on Windows and Mac OS X): use ``conda install sphinx``.
	
  Alternatively, the generic Python package manager ``pip`` can be used.
  	
	
* Breathe: Breathe is needed by sphinx to render source code generation for languages other than Python. As Breathe is a Python package, the package management system of the Python distribution should be used.
    - Linux: use the package manager of the Linux distribution, e.g., ``sudo apt install python3-breathe``.
    - Anaconda (recommended distribution on Windows and Mac OS X): use ``conda install -c conda-forge breathe``. Note that breathe is not contained in the standard package repository but the additional repository `conda-forge <https://conda-forge.org>`_.
    
* sphinxcontrib-bibtex: Extension to create bibliographies using BibTeX files. Installed in the same way as breathe, that, is using ``conda`` where available, or the ``pip`` package manager. Again, it is not included in the standard conda package repository, therefore it has to be installed through

  .. code-block:: bash

     conda install -c conda-forge sphinxcontrib-bibtex

* sphinx.ext.autodoc: Extract documentation from the docstrings contained in Python code and Python bindings.

* sphinx.ext.napoleon: Enables a simpler syntax for Python docstrings to be extracted via ``autodoc``.

Installer package generation
--------------------------------------

Installation packages are created using `CPack <https://cmake.org/cmake/help/latest/module/CPack.html>`_, which is integrated into the CMake build software. On Linux and Mac OS X, no external build software is needed.

Windows
"""""""
Download and install the NSIS installer system (`<https://sourceforge.net/projects/nsis/>`_). It will be automatically recognized if you create the installer packages.

Setup as Gitlab runner
----------------------

To provide a 
