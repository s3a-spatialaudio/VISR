Getting started with the VISR framework
=======================================

Welcome to the user guide of the VISR user documentation.

.. important:: This document is work in progress. Please check the online documentation regularly for updates.

To get an overview of the VISR framework, we suggest reading Chapter :ref:`visr_python_tutorial`, which provides a tutorial-style overview based on the paper :cite:`franck_et_al2018_an_open_software_rendering_framework_for_audio_reproduction_and_reproducible_research`.

Detailed installation instructions are provided in Chapter :ref:`getting_visr`.

The standalone command-line applications contained in the VISR distribution are detailed in Chapter :ref:`using_visr_using_standalone_renderers`.

The support for object-based audio in VISR is outlined in Chapter :ref:`visr_object_based_audio`. This includes the audio object representation, but also the object-based renderers contained in the default VISR distribution.

The processing components contained in the default component libraries are documented in Chapter :ref:`visr_default_component_library_reference`.

The Binaural synthesis toolkit is introduced in Chapter :ref:`visr_bst`, consisting of a :ref:`tutorial-style description <visr_bst_tutorial>` based on :cite:`franck_et_al2018_an_open_realtime_binaural_synthesis_toolkit_for_audio_research` and a :ref:`component reference <visr_bst_component_reference>`.


.. _overview_python_integration:

Python integration
------------------

Although the core of the VISR is written in C++ and there are uses of the VISR framework that do not require Python,
the Python integration plays a central role in this user documentation.
The main reason for that is accessibility. With Python, the VISR framework can be explored and extended interactively, and most interfaces are more intuitive than their C++ counterparts.

The main uses of the VISR framework without Python are the standalone applications explained in  :ref:`using_visr_using_standalone_renderers`, and the integration of VISR components and infrastructure in user-defined applications, plugins, or externals.

Because of this strong emphasis on the Python integration, it is important to install a VISR distribution built for thecorrect Python version, and to perform the Python-specific configuration steps. This is detailed in Section :ref:`visr_installation_setting_up_python`.
