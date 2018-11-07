  
Loudspeaker configurations are used to tell a renderer about the loudspeaker positions
and other properties of the setup.
It is used primarily in the loudspeaker renderers, including binaural renderering that use a virtual loudspeaker setup internally.

This section describes the format of a loudspeaker configuration and explains the helper functions provided with a VISR installation to create configuration files.

Configuration file example		
^^^^^^^^^^^^^^^^^^^^^^^^^^

A loudspeaker configuration has to be specified in an XML file.

An example is given below.

.. code-block:: xml
		
   <panningConfiguration>
     <loudspeaker id="M+000" channel="1" eq="highpass">
       <cart x="1.0" y="0.0" z="0"/>
     </loudspeaker>
     <loudspeaker id="M-030" channel="2" eq="highpass">
       <polar az="-30.0" el="0.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="M+030" channel="3" eq="highpass">
       <polar az="30.0" el="0.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="M-110" channel="4" eq="highpass">
       <polar az="-110.0" el="0.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="M+110" channel="5" eq="highpass">
       <polar az="110.0" el="0.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="U-030" channel="6" eq="highpass">
       <polar az="-30.0" el="30.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="U+030" channel="7" eq="highpass">
       <polar az="30.0" el="30.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="U-110" channel="8" eq="highpass">
       <polar az="-110.0" el="30.0" r="1.0"/>
     </loudspeaker>
     <loudspeaker id="U+110" channel="9" eq="highpass">
       <polar az="110.0" el="30.0" r="1.0"/>
     </loudspeaker>
     <virtualspeaker id="VoS">
       <polar az="0.0" el="-90.0" r="1.0"/>
       <route lspId="M+000" gainDB="-13.9794"/>
       <route lspId="M+030" gainDB="-13.9794"/>
       <route lspId="M-030" gainDB="-13.9794"/>
       <route lspId="M+110" gainDB="-13.9794"/>
       <route lspId="M-110" gainDB="-13.9794"/>
     </virtualspeaker>
     <triplet l1="VoS" l2="M+110" l3="M-110"/>
     <triplet l1="M-030" l2="VoS" l3="M-110"/>
     <triplet l1="M-030" l2="VoS" l3="M+000"/>
     <triplet l1="M-030" l2="U-030" l3="M+000"/>
     <triplet l1="M+030" l2="VoS" l3="M+000"/>
     <triplet l1="M+030" l2="VoS" l3="M+110"/>
     <triplet l1="U+030" l2="U-030" l3="M+000"/>
     <triplet l1="U+030" l2="M+030" l3="M+000"/>
     <triplet l1="U-110" l2="M-030" l3="U-030"/>
     <triplet l1="U-110" l2="M-030" l3="M-110"/>
     <triplet l1="U+110" l2="U-110" l3="M-110"/>
     <triplet l1="U+110" l2="M+110" l3="M-110"/>
     <triplet l1="U+030" l2="U-110" l3="U-030"/>
     <triplet l1="U+030" l2="U+110" l3="U-110"/>
     <triplet l1="U+030" l2="U+110" l3="M+110"/>
     <triplet l1="U+030" l2="M+030" l3="M+110"/>
     <subwoofer assignedLoudspeakers="M+000, M-030, M+030, M-110, M+110, U-030, U+030, U-110, U+110"
   	     channel="10" delay="0" eq="lowpass" gainDB="0"
   	     weights="1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0"
   	     />
     <outputEqConfiguration numberOfBiquads="1" type="iir">
         <filterSpec name="lowpass">
            <biquad a1="-1.9688283" a2="0.96907117" b0="6.0729856e-05" b1="0.00012145971" b2="6.0729856e-05"/>
         </filterSpec>
         <filterSpec name="highpass">
            <biquad a1="-1.9688283" a2="0.96907117" b0="-0.98447486" b1="1.9689497" b2="-0.98447486"/>
         </filterSpec>
     </outputEqConfiguration>
   </panningConfiguration>

Predefined configuration files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VISR package comes with a number of preconfigured loudspeaker configurations.
They are contained in the directory :code:`$VISR_ROOT/config/`.

The subdirectory :code:`config/generic` contains standard configurations, mainly from the `ITU-R BS2051 standard <https://www.itu.int/rec/R-REC-BS.2051/en>`_.

The supported configurations are:

=========== =============== ========================== ========= ==================== ==========
Name        File name       Number of loudspeakers     Dimension Virtual loudspeakers Comment
                            (upper, horizontal, lower)           (azimuth, elevation)
=========== =============== ========================== ========= ==================== ==========
System A    bs2051-0+2+0    0, 2, 0                    2D        (180,0)              Stereo
System B    bs2051-0+5+0    0, 5, 0                    2D         ---                 5.1
System C    bs2051-2+5+0    2, 5, 0                    3D        (0,90), (0,-90)
System D    bs2051-3+7+0    3, 7, 0                    3D        (0,90), (0,-90)
System E    bs2051-4+5+0    4, 5, 0                    3D        (0,90), (0,-90)
System F    bs2051-4+5+1    4, 5, 1                    3D        (0,90), (0,-90)
System G    bs2051-4+9+0    4, 9, 0                    3D        (0,90), (0,-90)
System H    bs2051-9+10+3   9, 10, 3                   3D        (0,-90)              22.2 (NHK)
=========== =============== ========================== ========= ==================== ==========

For all configurations, versions with and without subwoofer channels are provided. The version with subwoofer has
no suffix, the version without has :code:`-no-subwoofer` appended to the file name.
In general, the version with subwoofer should be preferred and the generated loudspeaker outputs are identical.
The output channel mapping is identical for both cases. In some cases the subwoofer channels are embedded
into the block of loudspeaker output channels. In the corresponding configurations without subwoofer, these
channels are not used.

For the stereo configuration, an additional configuration :code:`bs2051-0+2+0-rear-fading.xml` is provided in
which sound sources are faded out as they approach :math:`180^{\circ}`. In all other cases, the energy from virtual loudspeakers (as denoted in the table above) is distributed to neighboring real loudspeakers.

The subdirectories :code:`config/isvr`, :code:`config/surrey`, and :code:`config/bbc` contain examples of actual listening rooms.
The functions to generated these configurations are contained in the subdirectories :code:`scripts/` within these folders.

   
Generation functions
^^^^^^^^^^^^^^^^^^^^

To ease the creation of generation functions, the VISR framework provides several Python functions
to create the XML configuration files from a number of loudspeaker coordinates and additional optional parameters.
These functions are contained in the Python module :code:`loudspeakerconfig`.
If the VISR framework was installed through a binary installer and Python was configured as described in :ref:`installation_python_setup_configuration`, then the package can be directly imported, e.g.,

.. code-block:: python

   import loudspeakerconfig
   loudspeakerconfig.createArrayConfigFile( ... )

or

.. code-block:: python

   from loudspeakerconfig import createArrayConfigFile( ... )

The main function in this module is :code:`createArrayConfigFile()`.
It takes a set of loudspeaker coordinates, an output file name, and a large set of additional options:

.. autofunction:: loudspeakerconfig.createArrayConfigFile

The function `createArrayConfigFromSofa()` can be used to create configuration files from a SOFA file to be used, for example in a virtual loudspeaker renderer (:py:class:`visr_bst.VirtualLoudspeakerRenderer`):

.. autofunction:: loudspeakerconfig.createArrayConfigFromSofa

		  
Format description		
^^^^^^^^^^^^^^^^^^
The root node of the XML file is :code:`<panningConfiguration>`.
This root element supports the following optional attributes:

:code:`isInfinite`
  Whether the loudspeakers are regarded as point sources located on the unit sphere (:code:`false`) or as plane waves, corresponding to an infinite distance (:code:`true`). The default value is :code:`false`.
:code:`dimension`
  Whether the setup is considered as a 2-dimensional configuration (value :code:`2`) or as three-dimensional (:code:`3`, thedefault). In the 2D case, the array is considered in the x-y plane , and the :code:`z` or :code:`el` attributes of the loudspeaker positions are not evaluated. In this case, the triplet specifications consist of two indices only (technically they are pairs, not triplets).

Within the :code:`<panningConfiguration>` root element, the following elements are supported:

:code:`<loudspeaker>`
  Represents a reproduction loudspeaker.
  The position  is encoded either in a :code:`<cart>` node representing the cartesian coordinates in the :code:`x`, :code:`y` and :code:`z` attributes (floating point values in meter), or a :code:`<polar>` node with the attributes :code:`az` and :code:`el` (azimuth and elevation, both in degree) and :code:`r` (radius, in meter).

  The :code:`<loudspeaker>` nodes supports for a number of attributes:

  * :code:`id` A mandatory, non-empty string identification for the loudspeaker, which must be unique across all :code:`<loudspeaker>` and :code:`<virtualspeaker>` (see below) elements.
    Permitted are alpha-numeric characters, numbers, and the characters "@&()+/:_-".
    ID strings are case-sensitive.
  * :code:`channel` The output channel number (sound card channel) for this loudspeaker. Logical channel indices start from 1. Each channel must be assigned at most once over the set of all loudspeaker and subwoofers of the setup..
  * :code:`gainDB` or :code:`gain` Additional gain adjustment for this loudspeaker, either in linear scale or in dB (floating-point values. The  default value is 1.0 or 0 dB.  :code:`gainDB` or :code:`gain` are mutually exclusive.
  * :code:`delay` Delay adjustment to be applied to this loudspeaker as a floating-point value in seconds. The default value is  0.0).
  * :code:`eq` An optional output equalisation filter to be applied for this loudspeaker.
    Specified as a non-empty string that needs to match an :code:`filterSpec` element in the :code:`outputEqConfiguration`
    element (see below). If not given, no EQ is applied to for this loudspeaker.
:code:`<virtualspeaker>`
   An additional vertex added to the triangulation that does not correspond to a physical loudspeaker. Consist of a numerical :code:`id` attribute and a position specified either as a :code:`<cart>` or a :code:`<polar>` node (see :code:`<loudspeaker>` specification).

   The :code:`<virtualspeaker>` node provides the following configuration options:

   * A mandatory, nonempty and unique attribute :code:`id` that follows the same rules as for the :code:`<loudspeaker>` elements.
   * A number of :code:`route` sub-elements that specify how the energy from this virtual loudspeaker is routed to real loudspeakers.
     The :code:`route` element has the following attributes:
     * :code:`lspId`: The ID of an existing real loudspeaker.
     * :code:`gainDB`: A scaling factor with which the gain of the virtual loudspeaker is distributed to the real loudspeaker.
     
     In the above example, the routing specification is given by

     .. code-block:: xml

        <virtualspeaker id="VoS">
         <polar az="0.0" el="-90.0" r="1.0"/>
         <route lspId="M+000" gainDB="-13.9794"/>
         <route lspId="M+030" gainDB="-13.9794"/>
         <route lspId="M-030" gainDB="-13.9794"/>
         <route lspId="M+110" gainDB="-13.9794"/>
         <route lspId="M-110" gainDB="-13.9794"/>
       </virtualspeaker>

     That means that the energy of the virtual speaker :code:`"vos"` is routed to five surrounding speakers, with a scaling factor of 13.97 dB each.
     
:code:`<subwoofer>` Specify a subwoofer channel. In the current implementation, the loudspeaker are weighted and mixed into an arbitray number of subwoofer channels. The attributes are:
      
  * :code:`assignedLoudspeakers` The loudspeaker signals (given as a sequence of logical loudspeaker IDs) that contribute to the subwoofer signals. Given as comma-separated list of loudspeaker index or loudspeaker ranges. Index sequences are similar to Matlab array definitions, except that thes commas separating the parts of the sequence are compulsory.

    Complex example:

    .. code-block:: xml

       assignedLoudspeakers = "1, 3,4,5:7, 2, 8:-3:1"
    
  * :code:`weights` Optional weights (linear scale) that scale the contributions of the assigned speakers to the subwoofer signal.
    Given as a sequence of comma-separated linear-scale gain values, Matlab ranges are also allowed. The number of elements must match the :code:`assignedLoudspeakers` index list. Optional value, the default option assigns 1.0 for all assigned loudspeakers.
    Example: "0:0.2:1.0, 1, 1, 1:-0.2:0".
  * :code:`gainDB` or :code:`gain` Additional gain adjustment for this subwoofer, either in linear scale or in dB (floating-point valus, default 1.0 / 0 dB ). Applied on top of the :code:`weight` attributes to the summed subwoofer signal. See the :code:`<loudspeaker>` specification.
  * :code:`delay` Delay adjustment for this (floating-point value in seconds, default 0.0). See the :code:`<loudspeaker>` specification.
    
:code:`<triplet>`
  Loudspeaker triplet specified by the attributes :code:`l1`, :code:`l2`, and :code:`l3`.
  The values of :code:`l1`, :code:`l2`, and :code:`l3` must correspond to IDs of existing real or virtual loudspeakers.
  In case of a 2D setup, only :code:`l1` and :code:`l2` are evaluated.

  .. note:: At the time being, triplet specifications must be generated externally and placed in the configuration file.
	    This is typically done by creating a Delaunay triangulation on the sphere, which can be done in Matlab or Python.

	    Future versions of the loudspeaker renderer might perform the triangulation internally, or might not require a
	    conventional triangulation at all. In these cases, is it possible that the renderer ignores or internally
	    adapts the specified triplets.
  
:code:`outputEqConfiguration`
  This optional element must occur at most once.
  It provides a global specification for equalisation filters for loudspeakers and subwoofers.

  .. code-block:: xml

     <outputEqConfiguration  type="iir" numberOfBiquads="1">
       <filterSpec name="lowpass">
         <biquad a1="-1.9688283" a2="0.96907117" b0="6.0729856e-05" b1="0.00012145971" b2="6.0729856e-05"/>
       </filterSpec>
       <filterSpec name="highpass">
         <biquad a1="-1.9688283" a2="0.96907117" b0="-0.98447486" b1="1.9689497" b2="-0.98447486"/>
       </filterSpec>
     </outputEqConfiguration>

  The  attributes are:

  * :code:`type`: The type of the output filters. At the moment, only IIR filters provide as second-order sections (biquads) are supported. Thus, the value :code:`"iir"` must be set.
  * :code:`numberOfBiquads`: This value is specific to the :code:`"iir"` filter type.

  The filters are described in :code:`filterSpec` elements.
  These are identifed by a :code:`name` attribute, which must be an non-empty string unique across all :code:`filterSpec` elements.
  For the type :code:`iir`, a :code:`filterSpec` element consists of at most :code:`numberOfBiquad` nodes of type :code:`biquad`, which represent the coefficients of one second-order IIR (biquad) section.
  This is done through the attributes :code:`a1`,  :code:`a2`, :code:`b0`, :code:`b1`, :code:`b2` that represent the coefficients of the normalised transfer function
  
  .. math::

     H(z) = \frac{ b_0 + b_1 z^{-1} + b_{2}z^{-2} }{1 + a_1 z^{-1} + a_{2}z^{-2}}
