.. visr documentation master file, created by
   sphinx-quickstart on Wed Jul 18 14:06:39 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Front matter
   
.. toctree::
   :maxdepth: 1

   about
   people

Download
==================

.. toctree::
   :maxdepth: 2

   download


Development and Contribution
============================

.. toctree::
  :maxdepth: 2

  source
  git-workflow
  
  create-build-environment
  
  macos-setup-guide
  
VISR API
========

Libraries
=========

.. toctree::
  :maxdepth: 2


  builtin_component_library
  runtime_library
  objectmodel_library

..  elementary_function_library
  
Documentation
==================

.. * :ref:`genindex`
.. * :ref:`modindex`
.. * :ref:`search`

.. Andreas ! This is commented out, comment this in so that it can scan through the /doc/src/xml/index.xml and produce an index of classes.
.. doxygenindex::
..  :project: visr
..  :outline:
	    
   
.. This is an example of what would be for a single source . You can also do classes too.
.. .. doxygenfile:: point_source.hpp
..   :project: visr

Learning
==================

.. toctree::
   :maxdepth: 1

   tutorials
   examples
