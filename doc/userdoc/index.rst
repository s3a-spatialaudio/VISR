.. visr documentation master file, created by
   sphinx-quickstart on Wed Jul 18 14:06:39 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. toctree::
   :caption: Contents
   :name: mastertoc
   :maxdepth: 3

================   
VISR User Manual
================

.. toctree::
   :maxdepth: 1

   about
   getting-started
   visr-python-tutorial
   people

.. _getting_visr:
   
Getting VISR
=========================

.. toctree::
   :maxdepth: 2

   download
   installation
   sourcecode
   support

VISR principles
===============

.. toctree::
   :maxdepth: 2

Component-Based Audio processing
--------------------------------

VISR as a Rendering Framework
-----------------------------

Realtime and Offline Processing
-------------------------------

Prototyping versus mature signal processing code
------------------------------------------------

Using VISR
==================

.. toctree::
   :maxdepth: 2

Using VISR standalone renderers
------------------------------- 

.. toctree::
   :maxdepth: 2

   using-standalone-renderers


Using VISR with Python
----------------------

Using VISR audio workstation plugins
------------------------------------

Using Max/MSP externals
-----------------------

Extending VISR
==============

In this part we describe how to use the VISR framework to implement new functionality, i.e., functionality that is not contained in the existing components or standalone renderers.
This part is basically an extended version of the tutorial presented in 


Creating signal flows from existing components in Python
--------------------------------------------------------

Writing atomic functionality in Python
--------------------------------------

Implementing atomic components in C++
-------------------------------------

Creating composite components in C++
------------------------------------

Object-Based Audio with VISR
============================

.. toctree::
   :maxdepth: 2


Overview
--------

Although the VISR framework is deliberately application-agnostic, it is well-suited for working with spatial and object-based audio.

.. _the_visr_object_model:

The VISR object model
---------------------

.. include:: object-model.rst

Predefined object-based rendering primitives and renderers
----------------------------------------------------------

.. _visr_object_based_audio__reverberation:

Object-Based Reverberation
--------------------------

.. _loudspeaker_configuration_file_format:

The loudspeaker configuration format
------------------------------------

.. toctree::

   loudspeaker-config-file-format

VISR component reference
========================

Standard rendering component library (:code:`rcl`)
--------------------------------------------------

.. toctree::

   rcl_component_documentation

The Binaural Synthesis Toolkit (VISR-BST)
-----------------------------------------

.. toctree::

  binaural-synthesis-toolkit

Dynamic range control library
-----------------------------

Old contents
========================================

.. toctree::

  examples
  tutorials

The contents of these files will be removed or moved to other parts of the documentation.



