Copyright (C) 2017-2018 Andreas Franck and Giacomo Costantini
Copyright (C) 2017-2018 University of Southampton

VISR Binaural Synthesis Toolkit (BST)
Authors: Andreas Franck and Giacomo Costantini
Project page: http://cvssp.org/data/s3a/public/BinauralSynthesisToolkit/

Contact:
a.franck@soton.ac.uk
g.costantini@soton.ac.uk

License:
https://www.isc.org/downloads/software-support-policy/isc-license/ :

The BST is a Python package for binaural synthesis.
It is based on the VISR rendering framework
http://cvssp.org/data/s3a/public/VISR .

Installation
============

Install the VISR framework following the instructions linked from
http://cvssp.org/data/s3a/public/VISR .


Python 3 must also be installed on the system. Note that the VISR installer
must match the Python major + minor version number, for instance “Python3.6”.
We recommend the following Python distributions:

* Windows: Anaconda Download
* Mac OS X <= 10.11: Anaconda Download
* Mac OS X >= 10.12: System-provided Python 3.6 installation
* Linux (Ubuntu 16.04): System-provided Python 3 packages

The following Python packages must be installed:

* numpy
* h5py (for reading SOFA files)
* pyserial (for using serial trackers)

Configuration
=============

* Unpack the visr_bst archive to a directory of your choice (we call that base_directory).
* Add the base_directory to the Python search path. This can be done in several ways:
* Append base_directory to the PYTHONPATH environment variable.
* Add the directory to the Python variable sys.path (contained in the module sys)
* See the Python documentation  for additional information.
* The configuration instructions for the VISR framework will contain more detailed
  information on setting this path.
* The configuration can be checked by starting a Python interpreter and typing
  ‘import visr_bst’. If this command does not return an error, the configuration
  was successful.

Contents
========

Directories:

* "examples/"
 - Contains Python scripts with ready-made examples for the different renderers.
   Different forms exist:
   * "simulate_" scripts are offline executions of a binaural synthesis method.
   * "run_" scripts start a realtime rendering. They require a sound card and,
     depending on the method and the options chosen, realtime input for tracking
     data and object metadata.
   * "test_" scripts demonstrate the working of a specific component, e.g., a
     controller, in an offline execution.

* "renderers/"
 - Complete signal flows for various binaural rendering methods.

* "realtime_renderers/"
 - Complete signal flows for various binaural rendering methods with added
   facilities for receiving real-time tracking and (where applicable) object metadata.

* "controllers/"
 - Application-specific components to calculate control commands for the DSP components.
   These components embody the logic of a binaural synthsis approach (e.g., dynamic HRIR
   rendering or virtual loudspeaker synthesis)

* "hoa_components/"
 - Generic components for HOA (Higher order Ambisonics) rendering needed for HOA
   binaural synthesis.

* "util/"
 - Utility functions, e.g., for spherical geometry, spherical harmonics, or
   SOFA files.

* "data/"
 - Directory to hold data used in the example scripts.
   - Where possible, the example scripts attempt to download SOFA files and store
     them in subdirectories of data/
   - The subdirectory data/config/ contains loudspeaker configuration files used
     by the virtual loudspeaker renderers.
