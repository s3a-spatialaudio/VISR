.. _visr_object_based_audio:

Object-Based Audio with VISR
============================

.. toctree::
   :maxdepth: 2

Overview
--------

Although the VISR framework is deliberately application-agnostic, it is well-suited for working with spatial and object-based audio.
This is due to a number of reasons:

* The focus on multichannel audio makes it suitable for object-based audio, which often features complex sound scenes consisting of many signals, as well as multichannel reproduction systems.
* The ability to handle complex, also structure parameter data allows for the generation and transmission of object metadata within the system.
* The modular, reusable component structure fosters the creation of complex signal flows that are often used in object-bases audio.
* Last but not least, the VISR framework was conceived in the `S3A project <http://www.s3a-spatialaudio.org>`_ a research project on spatial and object-based audio.

Consequently the VISR framework contains different types of functionality to support processing of object-based audio.
These are typically implemened as libraries, for example component libraries.

.. include:: object-model.inc

Predefined object-based rendering primitives and renderers
----------------------------------------------------------

The default component library contains numerous atomic components for object-based audio as well as ready-made rendering signal flows.

These include:

Atomic components
^^^^^^^^^^^^^^^^^
* :py:class:`rcl.SceneDecoder`
* :py:class:`rcl.SceneEncoder`
* :py:class:`rcl.PanningCalculator`
* :py:class:`rcl.ObjectGainEqCalculator`
* :py:class:`rcl.HoaAllradGainCalculator`
* :py:class:`rcl.CAPGainCalculator`
* :py:class:`rcl.DiffusionGainCalculator`
* :py:class:`rcl.ObjectGainEqCalculator`	

Composite components
^^^^^^^^^^^^^^^^^^^^

* :py:class:`signalflows.CoreRenderer`
* :py:class:`signalflows.BaselineRenderer`
* :py:class:`reverbobject.ReverbObjectRenderer`
* :py:class:`visr_bst.DynamicHrirRenderer`
* :py:class:`visr_bst.HoaObjectToBinauralRenderer`
* :py:class:`visr_bst.ObjectToVirtualLoudspeakerRenderer`
  
Standalone renderers
--------------------

The loudspeaker renderers are described in Section :ref:`using_visr_using_standalone_renderers_visr_renderer`.

.. _visr_object_based_audio_reverberation:

Object-Based Reverberation
--------------------------

.. note:: This section will describe the support for object-based reverberation in the VISR renderers.
	  This is based on the reverb object :cite:`coleman2017reverb` using the object representation described in Section :ref:`visr_object_model_reverb_object`.
	  The functionality is contained in the library :code:`reverbobject` and the corresponding Python module :py:mod:`reverbobject`.

.. _loudspeaker_configuration_file_format:

The loudspeaker configuration format
------------------------------------

.. include:: loudspeaker-config-file-format.inc
