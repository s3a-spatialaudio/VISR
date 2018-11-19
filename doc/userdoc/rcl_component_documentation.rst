.. Copyright Andreas Franck 2018 - All rights reserved.
.. Copyright University of Southampton 2018 - All rights reserved.

.. _visr_default_component_library_reference:
   
The default component library
-----------------------------

.. toctree::
   
.. note:: The component documentation is extracted from the Python classes (or Python bindings in case of C++ classes).
	  For the exact C++ interfaces, see the VISR API documentation or the header documentation in the C++ code.

	  This information is also avalaible through Python's interactive documentation, e.g., through

	  .. code-block:: python

	     >>> import rcl
	     >>> help( rcl.Add )
   
.. _visr_component_library_reference_rcl:

Module :code:`rcl` overview
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :code:`rcl` library is the default library for atomic components.
It is contained in the default distribution of the VISR.

.. note:: The separation into a library for atomic components (:code:`rcl`) and composite components
	  (:ref:`visr_component_library_reference_composite`) is largely for historical reasons.
	  In the future, the component library/libraries will be restructured into topical units.

.. automodule:: rcl

Component :code:`rcl.Add`
'''''''''''''''''''''''''

.. autoclass:: rcl.Add
   :members:

Component :code:`rcl.BiquadIirFilter`
'''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.BiquadIirFilter
   :members:

Component :code:`rcl.CAPGainCalculator`
'''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.CAPGainCalculator
   :members:

Component :code:`rcl.ChannelObjectRoutingCalculator`
''''''''''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.ChannelObjectRoutingCalculator
   :members:

Component :code:`rcl.CrossfadingFirFilterMatrix`
''''''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.CrossfadingFirFilterMatrix
   :members:
      
Component :code:`rcl.DelayMatrix`
'''''''''''''''''''''''''''''''''

.. autoclass:: rcl.DelayMatrix
   :members:

Component :code:`rcl.DelayVector`
'''''''''''''''''''''''''''''''''

.. autoclass:: rcl.DelayVector
   :members:

Component :code:`rcl.DiffusionGainCalculator`
'''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.DiffusionGainCalculator
   :members:

      
Component :code:`rcl.FirFilterMatrix`
'''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.FirFilterMatrix
   :members:

            
Component :code:`rcl.GainMatrix`
''''''''''''''''''''''''''''''''

.. autoclass:: rcl.GainMatrix
   :members:

      
Component :code:`rcl.GainVector`
''''''''''''''''''''''''''''''''

.. autoclass:: rcl.GainVector
   :members:

Component :code:`rcl.HoaAllradGainCalculator`
'''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.HoaAllradGainCalculator
   :members:

Component :code:`rcl.InterpolatingFirFilterMatrix`
''''''''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.InterpolatingFirFilterMatrix
   :members:

Component :code:`rcl.ListenerCompensation`
''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.ListenerCompensation
   :members:

Component :code:`rcl.Nullsource`
''''''''''''''''''''''''''''''''

.. autoclass:: rcl.NullSource
   :members:

      
Component :code:`rcl.ObjectGainEqCalculator`
''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.ObjectGainEqCalculator
   :members:
      
      
Component :code:`rcl.PanningCalculator`
'''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.PanningCalculator
   :members:

Component :code:`rcl.PositionDecoder`
'''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.PositionDecoder
   :members:

Component :code:`rcl.ScalarOscDecoder`
''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.ScalarOscDecoder
   :members:


Component :code:`rcl.SceneDecoder`
''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.SceneDecoder
   :members:

Component :code:`rcl.SceneEncoder`
''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.SceneEncoder
   :members:

Component :code:`rcl.SignalRouting`
'''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.SignalRouting
   :members:

Component :code:`rcl.SparseGainMatrix`
''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.SparseGainMatrix
   :members:

Component :code:`rcl.TimeFrequencyTransform`
''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.TimeFrequencyTransform
   :members:
      
Component :code:`rcl.TimeFrequencyInverseTransform`
'''''''''''''''''''''''''''''''''''''''''''''''''''

.. autoclass:: rcl.TimeFrequencyInverseTransform
   :members:

Component :code:`rcl.UdpReceiver`
'''''''''''''''''''''''''''''''''

.. autoclass:: rcl.UdpReceiver
   :members:
      
Component :code:`rcl.UdpSender`
'''''''''''''''''''''''''''''''

.. autoclass:: rcl.UdpSender
   :members:

.. _visr_component_library_reference_composite:

Module :code:`signalflows` overview
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :code:`signalflows` contains composite components that are part of the standard distribution of the VISR.

Component :code:`signalflows.CoreRenderer`
''''''''''''''''''''''''''''''''''''''''''
.. autoclass:: signalflows.CoreRenderer
   :members:

Component :code:`signalflows.BaselineRenderer`
''''''''''''''''''''''''''''''''''''''''''''''
.. autoclass:: signalflows.BaselineRenderer
   :members:
