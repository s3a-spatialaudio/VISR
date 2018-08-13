MacOS Setup Guide
================================

To begin with, you will need to install command-line toold for Xcode installed. On a modern Mac-OS X system you can install these just by running the following command in a terminal window. You could also install the full Xcode application from Apple, if you prefer - but that takes up more space on your mac and isn't necessary.

``$ xcode-select --install``

Installing a Package Manager
############################

The easiest way to start developing with VISR is to use Mac OS X's open-source package manager, Homebrew.

``$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"``

We use Homebrew to install a number of required provisions that VISR needs to build, and you can explore the full documentation about what this powerful package manager does `here <https://brew.sh/>`_.

**Note**: You don't need to, but to support Mac universal build binaries ``BUILD_MACOS_UNIVERSAL_BINARIES``, all of the following installations via brew should be done with the ``-universal`` flag, as:

``$ brew install [package options] -universal [package name]``

Installing CMake
############################

.. _download a more UI-friendly CMake: https://cmake.org/files/v3.12/cmake-3.12.1-Darwin-x86_64.dmg

We use CMake as a way to cross-compile build, test and package our software. CMake uses VISR' source to generate native makefiles and workspaces that can be used in the compile environment of your choice.

**There are two ways to install cmake:** You can `download a more UI-friendly CMake`_, or if you are terminal kind of guy, you can get it via Homebrew, without the UI:

``$ brew install cmake``

Installing Doxygen (Sphinx & Breathe for WebAPI)
################################################

To install doxygen, the documentation generator and VISR's tool for  writing reference documentation, run

``$ brew install doxygen``

.. _download and installation instructions here: http://www.sphinx-doc.org/en/stable/install.html

To contribute to VISR's WebAPI, and to produce static html webpage layout of pages of any new classes or functions you write, you will need to install Sphinx (requires Python at least 2.7) and its reStructuredText-doxygen bridge plugin, Breathe. For Sphinx, see `download and installation instructions here`_. To install the extension to reStructuredText and Sphinx - Breathe, you should run ``$ pip install breathe`` in terminal.

**Note**: For webapi usage, and for building the target BUILD_DOCUMENTATION_SPHINX, first consult the code conduct page.

Installing Boost
################

On Mac OS X, VISR is currently working with version 1.65.1. Run:

``$ brew install boost``

**Note**: Should you run into any issues with compiling VISR after this tutorial, or notice any Boost errors, you can attempt to switch your boost version. Make sure you use brew to do this (it is highly discouraged to do this manually)

``$ brew switch <package> <version>``

Thus, you should attempt:

``$ brew switch boost 1.65.1``

``$ brew switch boost 1.66.0``

``$ brew switch boost 1.67.0_1``

Installing PortAudio
####################

``$ brew install portaudio``

Installing libsndfile
#####################

``$ brew install libsndfile``

Using VISR's Python Bindings
############################

**Note:** MacOS High Sierra ships with Python installed in its /usr/local/

You can install via anaconda, or via Homebrew again, but this will create two working copies of Python in your /usr/local.

``$ brew install python``

``$ brew install boost -universal -with-python``

``$ brew install boost-python -universal``
