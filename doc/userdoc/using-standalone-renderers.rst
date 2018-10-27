.. _using_visr_using_standalone_renderers:

Using standalone applications
-----------------------------

The VISR framework provides a number of standalone real-time rendering applications for some of its audio processing functionality.

If a component-aware installer is used (see Section :ref:`installation_component_description`), then the component "Standalone applications" has to be selected during installation.

The standalone applications are started as command line applications, and configured through a number of command line options or a configuration file.

.. _using_standalone_renderers_common_options:

Common options
^^^^^^^^^^^^^^

All standalone applications provided with the VISR provide a common set of command line options:

**--version** or **-v**
  Returns a short description of the tool and its version information.
**--help** or **-h**
  Returns a list of supported command line options with brief descriptions.
**--option-file <filename>** or **@<filename>**
  Pass a configuration file containing a set of command line options to the applications.
  This options allows to store and share complex sets of command line options, and to overcome potential command line length limitations.
  
  A typical option file has the format
  
  .. code-block:: bash
  
     -i 2
     -o 2
     -f 48000
     -c "/usr/share/visr/config/generic/stereo.xml"
  
  where,by convention, one option is stored per line.
**--sampling-frequency** or **-f**
  The sampling frequency to be used for rendering, as an integer value in Hz.
  Typically optional. If not given, a default value (e.g., 48000 Hz) will be used.
**--period** or **-p**
  The period, or blocksize, or buffersize to be used by the audio interface. 
  
  In most cases, the period should be a power of 2, e.g., 64, 128, 256, 512, ..., 4096.
  Lower values mean lower audio latency, but typically higher system load and higher susceptibility to audio underruns.
  
  Typically an optional argument. If not given, a default value (e.g., 1024) is used.
**--audio-backend** or **-D**
  Specify the audio interface library to be used.
  
  This option is mandatory.
  
  The audio interfaces depend on the operating system and the configuration of the user's system. The most common options are **"PortAudio"** (all platforms) and **"Jack"** (Linux and Mac OS X). Note that additional libraries (or backends) can be available for a specific platform, and new backends might be added in the future.
**-audio-ifc-options**
  A string to provide additional options to the audio interface.
  
  This is an optional argument, and its content is interface-specific.
  
  By convention, the existing audio interfaces expect JSON (`JavaScript Object Notation <https://www.json.org/>`_) strings for the backend-configuration.
  
  To pass JSON strings, the whole string should be enclosed in single or double quotes, and the quotes required by JSON must be escaped with a backslash. For example, the option might be used in this way:
  
  .. code-block:: bash
  
     visr_renderer ... -audio-ifc-options='{ \"hostapi\": \"WASAPI\" }'

  Section :ref:`using_visr_using_standalone_renderers_specific_audio_options` below explains the options for the currently supported audio interfaces.
     
**-audio-ifc-option-file**
  Provide a interface-specific option string within a file.
  
  This can be used to avoid re-specifying complex options strings, to author them in a structured way, and to store and share them.
  
  In addition, it avoids the quoting and escaping tricks needed on the command line.
  For example, the option shown above could be specified in a file **portaudio_options.cfg** as
  
  .. code-block:: json
  
     {
       "hostapi": "WASAPI"
     }
  
  and passed as 
  
  .. code-block:: bash
  
     visr_renderer ... -audio-ifc-option-file=portaudio_options.cfg

.. note:: The options **--audio-ifc-options** and **--audio-ifc-option-file** are mutually
     exclusive, that means other none or one of them can be provided.

.. _using_visr_using_standalone_renderers_visr_renderer:

VISR object-based loudspeaker renderer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These renderers facilitate object-based rendering to arbitrary loudspeaker setups.
They use the VISR audio object model and the corresponding JSON format described in Section :ref:`the_visr_object_model`.

Note that there are two binaries for loudspeaker rendering: **visr_renderer** and **baseline_renderer**.
The provision of these separate binaries has technical reasons - mainly their dependency on a compatible and configured Python installation, as explained below.

The two binaries provided are:

*visr_renderer*
  This is the full object-based renderer, including a powerful metadata adaptation engine for intelligent object-based rendering - the Metadapter - implemented in Python.
  This metadapter is integrated into the rendering binary as an optional part, and is used if the option **--metadapter-config** is specified.
  The binary itself, however, needs a Python istallation to start at all, irrespective whether this option is set.
*baseline_renderer*
  This is the legacy object-based loudspeaker renderer. At the time being, it provides the same functionality as the **visr_renderer**, 
  but without the optional integrated metadapter component.
  In this way, the binary is independent of a Python distribution on the user's computer.

In general, we recommend to use **visr_renderer** if possible, and to use **baseline_renderer** on systems where the Python features 
of the VISR framework are not available.

The command line arguments supported by the **visr_renderer** application are:

.. code-block:: bash

   $> visr_renderer.exe --help
   -h [ --help ]                   Show help and usage information.
   -v [ --version ]                Display version information.
   --option-file arg               Load options from a file. Can also be used
                                   with syntax "@<filename>".
   -D [ --audio-backend ] arg      The audio backend.
   -f [ --sampling-frequency ] arg Sampling frequency [Hz]
   -p [ --period ] arg             Period (blocklength) [Number of samples per
                                   audio block]
   -c [ --array-config ] arg       Loudspeaker array configuration file
   -i [ --input-channels ] arg     Number of input channels for audio object
                                   signal
   -o [ --output-channels ] arg    Number of audio output channels
   -e [ --object-eq-sections ] arg Number of eq (biquad) section processed for
                                   each object signal.
   --reverb-config arg             JSON string to configure the object-based
                                   reverberation part, empty string (default) to
                                   disable reverb.
   --tracking arg                  Enable adaptation of the panning using visual
                                   tracking. Accepts the position of the tracker
                                   in JSON format"{ "port": <UDP port number>,
                                   "position": {"x": <x in m>, "y": <y im m>,
                                   "z": <z in m> }, "rotation": { "rotX": rX,
                                   "rotY": rY, "rotZ": rZ } }" .
   -r [ --scene-port ] arg         UDP port for receiving object metadata
   -m [ --metadapter-config ] arg  Metadapter configuration file. Requires a
                                   build with Python support. If empty, no
                                   metadata adaptation is performed.
   --low-frequency-panning         Activates frequency-dependent panning gains
                                   and normalisation
   --audio-ifc-options arg         Audio interface optional configuration
   --audio-ifc-option-file arg     Audio interface optional configuration file

The arguments for the **baseline_renderer** application are identical, except that the :code:`--metadapter-config` option is not supported as explained above.

:code:`--audio-backend` or :code:`-D`
   The audio interface library to be used. See section :ref:`using_standalone_renderers_common_options`.
:code:`--audio-ifc-options`:
   Audio-interface specific options, section :ref:`using_standalone_renderers_common_options`.
:code:`--audio-ifc-option-file`:
   Audio-interface specific options, section :ref:`using_standalone_renderers_common_options`.
:code:`--sampling-frequency` or :code:`-f`:
  Sampling frequency in Hz. Default: 48000 Hz. See section :ref:`using_standalone_renderers_common_options`.
:code:`--period` or :code:`-p`:
  The number of samples processed in one iteration of the renderer. Should be a power of 2 (64,128,...,4096,...) . Default: 1024 samples. See section :ref:`using_standalone_renderers_common_options`.
:code:`--array-config` or :code:`-c`:
  File path to the loudspeaker configuration file. Path might be relative to the current working directory. Mandatory argument.
  The XML file format is described below in Section :ref:`loudspeaker_configuration_file_format`.
:code:`--input-channels` or :code:`-i`:
  The number of audio input channels. This corresponds to the number of single-waveform objects the renderer will process. Mandatory argument. A (case-insensitive) file extension of :code:`.xml` triggers the use of the XML format for parsing.
:code:`--output-channels` or :code:`-o`:
  The number of output channels the renderer will put write to. If not given, the number of output channels is determined from the largest logical channel number in the array configuration.
:code:`--object-eq-sections`:
  The number of EQs (biquad sections) that can be specified for each object audio signal.

  Default value: o, which deactivate EQ filtering for objects.
:code:`--low-frequency-panning`:
      Switches the loudspeaker panning between standard VBAP and a dual-frequency approach with separate low- and high-frequency panning rules.

      Admissible values are :code:`true` and :code:`false`. The default value is :code:`false`, corresponding
      to the standard VBAP algorithm.
  
:code:`--reverb-config`:
  A set of options for the integrated reverberation engine for the RSAO (:code:`PointsourceWithReverb`) object (see section :ref:`visr_object_based_audio__reverberation`).
  To be passed as a JSON string. The supported options are:

  :code:`numReverbObjects`:
    The number of RSAO objects that can be rendered simultaneously.
    These objects may have arbitrary object ids, and they are automatically allocated to the computational resources avalable.

    To be provided as a nonnegative integer number The default value is 0, which means that the reverberation rendering is effectively disabled.

  :code:`lateReverbFilterLength`:
    Specify the length of the late reverberation filters, in seconds.

    Provided as a floating-point value, in seconds. Default value is zero, which results in the shortest reverb filter length that can be processed by the renderer, typically one sample.

  :code:`lateReverbDecorrelationFilters`:
    Specifies a multichannel WAV file containing a set of decorrelation filters, one per loudspeaker output. The number of channels must be equal or greater than the number of loudspeakers, channels that exceed the number of loudspeakers are not used.

    To be provided as a full file path. The default value is empty, which means that zero-valued filters are used, which effectively disables the late reverb.

  :code:`discreteReflectionsPerObject`:
    The maximum number of discrete reflections that can be rendered for a single RSAO object.

    Given as a nonnegative integer number. The default value is 0, which means that no discrete reflections are supported.

  :code:`maxDiscreteReflectionDelay`:
    The maximum discrete reflection delay supported. This allows a for tradeoff between the computational resources, i.e., memory required by the renderer and a realistic upper limit for discrete reflection delays.

    To be provided as a floating-point number in seconds. Default value is 1.0, i.e., one second.

  :code:`lateReverbFilterUpdatesPerPeriod`
    Optional argument for limiting the number of filter updates in realtime rendering.
    This is to avoid processing load peaks, which might lead to audio underruns, if multiple RSAO objects are changed simultaneously.
    The argument specifies the maximum number of objects for whom the late reverb filter is calculated withon one period (audio buffer). If there are more pending changes than thix number, the updates are spread over multiple periods.
    This is a tradeoff between peak load and the timing accuracy and synchronity of late reverb updates.

    Optional value, default value is 1, meaning at most one update per period

    .. todo: Decide wehether the default value should mean that all changes should be applied instantaneously.

  An example configuration is:

  .. code:: bash

     --reverb-config='{ \"numReverbObjects\": 5, \"lateReverbFilterLength\": 4.0,
                \"lateReverbDecorrelationFilters\": "/home/af5u13/tmp/decorr.wav\",
                \"discreteReflectionsPerObject\": 10 }'	    
  
:code:`--tracking`
  Activates the listener-tracked VBAP reproduction, which adjust both the VBAP gains as well as the final loudspeaker gains and delays according to the listener position. It takes a non-empty string argument containing a JSON message of the format: :code:`{ "port": &lt;UDP port number&gt;, "position": {"x": &lt;x in m&gt;, "y": &lt;y im m&gt;, "z": &lt;z in m&gt; }, "rotation": { "rotX": rX, "rotY": rY, "rotZ": rZ } }"`. The values are defined as follows:

  =============  ======================================================  ============  ====
  ID             Description                                             Unit          Default 
  =============  ======================================================  ============  ====
  port           UDP port number                                         unsigned int  8888 
  position.x     x position of the tracker                               m             2.08 
  position.y     y position of the tracker                               m             0.0 
  position.z     z position of the tracker                               m             0.0 
  rotation.rotX  rotation the tracker about the x axis, i.e., y-z plane  degree        0.0 
  rotation.rotY  rotation the tracker about the y axis, i.e., z-x plane  degree        0.0 
  rotation.rotZ  rotation the tracker about the z axis, i.e., x-y plane  degree        180 
  =============  ======================================================  ============  ====

.. note:: The option parsing for :code:`--tracking` not supported yet, default values are used invariably. To activate tracking, you need to specify the  :code:`--tracking` option with an arbitrary parameter (even  :code:`--tracking=false` would activate the tracking.
	  
:code:`--scene-port`
  The UDP network port which receives the scene data in the VISR JSON object format.
:code:`--metadapter-config`
  An optional Metadapter configuration file in XML format, provided as a full path to the file. If specified, the received metadata are passed through a sequence of metadata adaptation steps that are specified in the configuration file. If not given., metadata adaptation is not performed, and objects are directly passed to the audio renderer.

  This option is not supported by the **baseline_renderer** application.

.. _loudspeaker_configuration_file_format:
  
Loudspeaker configuration file format
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The loudspeaker configuration has to be specified in an XML file.
It is used primarily for the loudspeaker renderers.

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

Format description		
''''''''''''''''''
The root node of the XML file is :code:`<panningConfiguration>`.
This root element supports the folloring optional attributes:

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

.. _using_visr_using_standalone_renderers_matrix_convolver:

The matrix convolver renderer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The matrix convolver renderer is a multiple-input multiple-output convolution engine to be run as a command line application.

It implements uniformly partitioned fast convolution for arbitrary routing points between input and output files.

Basic usage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


.. code-block:: bash

   $> matrix_convolver --help
   -h [ --help ]                   Show help and usage information.
   -v [ --version ]                Display version information.
   --option-file arg               Load options from a file. Can also be used
                                   with syntax "@<filename>".
   -D [ --audio-backend ] arg      The audio backend. JACK_NATIVE activates the
                                   native Jack driver insteat of the PortAudio
                                   implementation.
   --audio-ifc-options arg         Audio interface optional configuration
   --audio-ifc-option-file arg     Audio interface optional configuration file
   --list-audio-backends           List the supported audio backends that can be
                                   passed to the the "--audio-backend" ("-D")
                                   option.
   --list-fft-libraries            List the supported FFT implementations that
                                   can be selected using the "--fftLibrary"
                                   option.
   -f [ --sampling-frequency ] arg Sampling frequency [Hz]
   -p [ --period ] arg             Period (block length): The number of samples
                                   per audio block, also the block size of the
                                   partitioned convolution.
   -i [ --input-channels ] arg     Number of input channels for audio object
                                   signal.
   -o [ --output-channels ] arg    Number of audio output channels.
   --filters arg                   Initial impulse responses, specified as
                                   comma-separated list of one or multiple WAV
                                   files.
   --filter-file-index-offsets arg Index offsets to address the impulses in the
                                   provided multichannel filter files. If
                                   specified, the number of values must match
                                   the number of filter files.
   -r [ --routings ] arg           Initial routing entries, expects a JSON array
                                   consisting of objects "{"inputs": nn,
                                   "outputs":nn, "filters":nn ("gain":XX)
   -l [ --max-filter-length ] arg  Maximum length of the impulse responses, in
                                   samples. If not given, it defaults to the
                                   longest provided filter,
   --max-routings arg              Maximum number of filter routings.
   --max-filters arg               Maximum number of impulse responses that can
                                   be stored.
   --fft-library arg               Specify the FFT implementation to be used.
                                   Defaults to the default implementation for
                                   the platform.


Operation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The matrix convolver consists of the following elements:

* A number of **input channels**.
* A set of **FIR filter**, which can be reused multiple times.
* A set of **output channels**.  
* A set of **routings**, which defines that a given input is filtered through a specific filter (with an optional gain), and the result is routed to a given output channels. All filtering results that are routed to a given output are summed together.

This interface allows for several different operation modes, for example:

* Multi-channel filtering where each input is filtered with one filter to give produce the same number of output channels.
* Filtering to produce multiple, different copies of the same input signal.
* Filtering multiple signals and adding them together, as, for example, in filter-and-sum beamforming.
* MIMO filtering with complete matrices, where a filter is defined for each input-output combination.
* MIMO filtering with sparse matrices, corresponding to sophisticated routings between inputs and outputs.
  

Detailed option description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:code:`--help` or :code:`-h`:

:code:`--version` or :code:`-v`  :
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--option-file`:
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--audio-backend` or :code:`-D`:
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--audio-ifc-options`:
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--audio-ifc-option-file`:
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--sampling-frequency` or :code:`-f`
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--period` or :code:`-p`:
  Standard options, described in :ref:`using_standalone_renderers_common_options`
:code:`--input-channels` or :code:`-i`:
  The number of input channels. Must not exceed the number of capture channels of the sound card.
:code:`-o` or :code:`--output-channels`:
  The number of output channels. Must be less or equal than the number of sound card output channels.
:code:`--filters`
  The filters, specified as a comma-separated list of WAV files. WAV files can be multichannel, in this case, every channel is handled as a separate filter.

  All filters are combined into a single array, where each filter is associated to a unique index (starting from zero if not specified otherwise.)

  This argument is optional. If not provided, all filters are zero-initialised. Note that if the :code:`filters` argument is not provided, then the option :code:`max-routings` must be provided.
:code:`--filter-file-index-offsets`
  Specify the start filter index for each WAV file specified by the :code:`--filters` argument.
  To be provided as a comma-separated list of nonnegative filter entries, one for each file in the :code:`filters` argument.
  This argument is optional. If not provided, the start index of the first file is 0, and the start offset af all subsequent filter files follows the end index of the previous filter file.
  This facility can be used to decouple the number of filters in the WAV files from the indexing scheme used to define the routings.

  Example:

  .. code-block:: bash

     --filters ="filters_2ch.wav, filters_6ch.wav, filters_4ch.wav"   
     --filter-file-index-offsets="2, 8, 16"

  Here, three WAV files are provided: :code:`filters_2ch.wav`, :code:`filters_6ch.wav`, and :code:`filters_4ch.wav`, with 2, 6, and 4 channels respectively.
  The filter offsets "2, 8, 16" mean that the filters of :code:`filters_2ch.wav` will be associated to the indices 2 and 3, that of :code:`filters_6ch.wav` by indices 8-13, and that of :code:`filters_4ch.wav` by the indices 16-19.

  Any filters below, between, or above the initialized filter channels (here, indices 0-1, 4-7, 14-15, and >=20) will be zero-initialised. 

  If the :code:`--filter-file-index-offsets` hadn't been provided in this example, the start offsets for the filter sets from the three files would have been 0,2,8.

:code:`--routings` or :code:`-r`
  Provide a list of routings points. This is to be specified as a JSON string.
  A routing defines a filter being applied between a specific input channel and a specific output channels.
  The JSON representation for a single entry is

  .. code-block:: json

     { "input": "<i>", "output": "<o>", "filter": "<f>", "gain": "<g>" }		  

  Here, :code:`<i>` is the index of the input channel, :code:`<o>` is the channel index of the output, and :code:`<f>` is the index of the filter (see above).
  All indices are zero-offset.
  The gain specification :code:`,"gain": <g>` is optional, with :code:`<g>` representing a linear-scale gain value.

  A routing list is a JSON array of routing entries, for example

  .. code-block:: json

     [{"input":"0", "output":"0", "filter":"2" },
      {"input":"0", "output":"1", "filter":"1" },
      {"input":"0", "output":"2", "filter":"0" }]
  
  A routing entry can define multiple multiple routings using a Matlab-like stride syntax for :code:`<i>`, :code:`<o>`, :code:`<f>`, or several of them.
  If an index is a stride sequence, then the routing entry is duplicated over all values of the stride sequence. If more than one index in the routing entry are strides, then all of them must have the same length, and each of the duplicated routing entries contains the respective value of the respective stride sequence.
  For example, the strided routing entry

  .. code-block:: json

     {"input":"3", "output":"0:3:9", "filter":"1" }

  routes input 3 to the outputs 0, 3, 6, and 9, using the filter indexed by 1 for each routing.
  In contrast.
  
  .. code-block:: json

     {"input":"0", "output":"0:2", "filter":"2:-1:0" }

  is equivalent to the routing list shown above.
  
  .. code-block:: json

     [{"input":"0", "output":"0", "filter":"2" },
      {"input":"0", "output":"1", "filter":"1" },
      {"input":"0", "output":"2", "filter":"0" }]
  
:code:`--max-filter-length` or :code:`-l`:
  Define the maximum length of the FIR filters.
  If the :code:`--filters` option is provided, this argument is optional. In this case, admissible filter length is set to the largest length of all specified filter.
  an error is reported if any specified filter exceeds the admissible length.
  If :code:`--filters` and :code:`--max-filter-length` are both provided, then an error is generated if the length of any specified filter exceeds the value of :code:`--max-filter-length`.

:code:`--max-routings` :
  Define the maximum number of routings.
  If the :code:`--routings` options is present, this argument is optional, and the maximum number of permissible routings is set to the number
  of routing entries in the :code:`--routing` argument.
  If :code:`routings` and :code:`--max-routings` are both specified, the number of entries in :code:`--routings` must not exceed the value of :code:`--max-routings`.
:code:`--max-filters`:
  Define the maximum number of filter entries.
  This parameter is optional if the argument :code:`--filters` is provided. In this case, the maximum filter number is set to the number of filters generated by the  :code:`--filters` argument.

  .. note:: If combined with :code:`--filter-file-index-offsets`, this automatically computed number of filters includes any gaps in the generated filter set.

  If :code:`--filters` and :code:`max-filters` are both provided, then the number of filter entries created by :code:`--filters` must not exceed
  the value of :code:`--max-filters`. 	    
:code:`--fft-library`:
  Select a FFT implementation from the set of available FFT libraries.
  The admissible values (strings) can be obtained through the :code:`--list-fft-libraries` option.
  
.. note:: The current implementation accepts only a static configuration.

   Future versions, however, will provide runtime control through a network command interface.

   Some arguments or argument combinations do not make sense at the moment, but will do when combined with runtime control.
   Examples include the ability to provide empty routings, zero-valued filters, or to specify values for :code:`--max-routings` or :code:`--max-filters` that are larger than the currently set values. 


Examples   
~~~~~~~~

A channel-wise multichannel convolution can be performed as

.. code-block:: bash

   $> matrix_convolver -i 2 -o 2 -p 512 -D PortAudio -f 48000 --filters="filters.wav"
     -r '[ {\"input\": \"0:1\", \"output\":\"0:1\", \"filter\":\"0:1\"}]'

.. note:: The quoting is necessary when started from the command line. 

The following example shows a convolution with binaural room impulse responses, where a 9-loudspeaker multichannel signal is routed to 9x2 BRIRs
that are summed to form two ear signals.

.. code-block:: bash

   $> matrix_convolver -i 9 -o 2 --max-filters=18 --max-routings=18
      -r "[{\"input\":\"0:8\", \"output\":\"0\", \"filter\":\"0:2:16\"},
           {\"input\":\"0:8\", \"output\":\"1\", \"filter\":\"1:2:17\"}]"
      --filters="bbcrdlr9ch_brirs.wav"
      -D Jack -f 48000 -p 512

Here, the file :code:`bbcrdlr9ch_brirs.wav` contains the 18 BRIRs, with the first nine channels for the left and the remaining channels for the right ear filters.

.. _using_visr_using_standalone_renderers_python_runner:

The python_runner application
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This standalone application is an alternative way to run arbitrary VISR components in real-time.

Compared to instantiating the processing from a Python interpreter, this can be easier to control, for example within  a script or when running a device in 'headless mode'.

For obvious reasons, this application requires an installed and correctly configured Python distribution, as described in Section :ref:`installation_python_setup_configuration`. 

Usage
~~~~~
The supported options are displayed when started with the :code:`--help` or :code:`-h` option:

.. code-block:: bash

   $> python_runner --help
     -h [ --help ]                     Show help and usage information.
     -v [ --version ]                  Display version information.
     --option-file arg                 Load options from a file. Can also be used 
                                       with syntax "@<filename>".
     -D [ --audio-backend ] arg        The audio backend.
     -f [ --sampling-frequency ] arg   Sampling frequency [Hz]
     -p [ --period ] arg               Period (blocklength) [Number of samples per
                                       audio block]
     -m [ --module-name ] arg          Name of the Python module to be loaded 
                                       (without path or extension).
     -c [ --python-class-name ] arg    Name of the Python class (must be a 
                                       subclass of visr.Component).
     -n [ --object-name ] arg          Name of the Python class (must be a 
                                       subclass of visr.Component).
     -a [ --positional-arguments ] arg Comma-separated list of positional options 
                                       passed to the class constructor.
     -k [ --keyword-arguments ] arg    Comma-separated list of named (keyword) 
                                       options passed to the class constructor.
     -d [ --module-search-path ] arg   Optional path to search for the Python 
                                       module (in addition to the default search 
                                       path (sys.path incl. $PYTHONPATH). Provided as a
                                       comma-separated list of directories.
     --audio-ifc-options arg           Audio interface optional configuration.
     --audio-ifc-option-file arg       Audio interface optional configuration file.

Detailed option description
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The standard options :code:`--help`, :code:`--version`, :code:`--audio-backend`, :code:`sampling-frequency`, `:code:`--period`, `:code:`--audio-ifc-options`, and `:code:`--audio-ifc-option-file` are described in Section :ref:`using_standalone_renderers_common_options`.

The remaining options are:

:code:`--module-name` or :code:`-m`:
  Specify the name of a Python module that contains the VISR component to be executed.
  That is, use the module name that would need to be imported in an interactive Python session.
  The module name must be provided without the file extension.
  It can be specified either with a full file path, or as a pure module name.
  In the latter case, the directory containing the module must be on the Python module search
  path or included in the :code:`--module-search-path` option.

  The module can be in one of several forms:

  * A Python file (normally with extension :code:`.py`) that contains the component class.
    The module name must be specified without the extension.
  * A directory containing a multi-file package.
  * Compiled extension modules implemented in C++. Typical file extesnions are :code:`.so`
    (Linux and Mac OS X) or :code:`.pyc` (Windows). The module name must be specified without the extension.

  This is a mandatory argument.
:code:`--python-class-name` or :code:`-c`:
  The name of the Python class to be instantiated, without the leading namespace name.
  This class must be derived from :code:`visr.Component`
  and must be defined in the module :code:`module-name`.

  .. note:: At the moment, only classes in the top-level namespace are supported.
	    That is, classes of the form :code:`moduleName.submodule.className` cannot be used. 

  This argument is mandatory.	    
:code:`--object-name` or :code:`-n`:
  Set a name for the top-level component. This name is used, for example, in error messages
  and warnings emitted from the component.
  
  This argument is optional.
  If not provided, a default name is used.
:code:`-a` :code:`--positional-arguments`:
  Provide a sequence of parameters to the component's constructor as positional arguments.

  The fixed first three arguments to a component constructor, i.e., :code:`context`,
  :code:`name` and :code:`parent`, do not need top be specified.
  That means the first value of the sequence is passed to the fourth argument, the second value
  to the fifth argument, and so on.

  The parameters are passed as a Python tuple.
  See, e.g., the `Python documentation on tuples <https://docs.python.org/3.7/library/stdtypes.html#typesseq-tuple>`_.
  Following these conventions, the arguments can be specified as follows:

  * A comma-separated list of values, for example

    .. code-block:: bash
		    
       -a "3, 2.7,'foobar'"

    Note that the enclosing double quotes are required to separate the argument to :code:`-a`
    from other options on the command line.
    They are strictly necessary only if the parameter sequence contains spaces, but we
    recommend to use double quotes for consistency.

    If the parameter sequence consists of a single value, a training comma is required.
    That is, a single positional argument is specified as
    
    .. code-block:: bash
		    
       -a "3,"

    If two or more arguments are provided, the trailing comma is optional.

  * A comma-separated list of values, enclosed in parentheses.
    Apart from the additional parentheses, the syntax is identical to the
    comma-separated lists above.
    That is, the argument list above would be specified as
    
    .. code-block:: bash
		    
       -a "(3, 2.7,'foobar' )"

    As above, single arguments require a trailing comma.
       
    .. code-block:: bash
		    
       -a "(3,)"

  * A tuple constructed using the :code:`tuple()` keyword, that is

    .. code-block:: bash

       -a "tuple(3, 2.7,'foobar' )"

    and in the single-parameter case
       
    .. code-block:: bash
		    
       -a "tuple(3)"

    That is, no trailing comma is required in this case.

  The :code:`--positional-arguments` option is optional.
  If it is not provided, no positional arguments are passed to the component's constructor.

:code:`--keyword-arguments` or :code:`-k`:
  A set of keyword arguments to be passed to the component's constructor.
  To be provided as a Python dictionary, for example:

  .. code-block:: bash
		    
     -k "{ 'argument1': value1, 'argument2': value2, ..., 'argumentN': valueN }"

  .. hint:: As in case of positional arguments, we suggest to enclose the complete argument
            in double quotes.
            When following this convention, single quotes can be used for the keywords as
            :code:`'argument1'` and string parameters without the need for escaping quotes.

  Following Python conventions, keyword arguments must not be provided for arguments already
  handled by the :code:`--positional-arguments` option.
  Likewise, keyword arguments must not be provided for the fixed first three constructor
  arguments of a component: :code:`context`, :code:`name` and :code:`parent`.

  This argument is optional; no keyword arguments are passed to the component if it is not given.
:code:`--module-search-path` or :code:`-d`:
  Specifies additional search paths for Python modules.

  To be specified as a comma-separeted list of directory path.
  
  These search paths can be used to locate the module containing the component to be run, unless
  a directory path is passed to the :code:`--module-name` option.
  In addition, the search paths are evaluated to locate transitive dependencies of the module to be loaded.
  For example, the path to VISR Python externals can be specidied in this way, thus avoiding the use of
  the :code:`PYTHONPATH` environment variable, as described in section
  :ref:`installation_python_setup_configuration`.
  The additional search paths are added to the Python search path :code:`sys.path` before the main
  module specified by the :code:`-m` option is loaded. 

  .. todo:: Decide whether the additional paths shall be appended or prepended to the system path.
	    In the latter case, this could avoid loading another module of the same name by prioritizing the explicitly added paths.

  This argument is optional, no additional search paths are added if the option is not provided.
  
Examples
~~~~~~~~

In this example we use a simple Python-based VISR component :code:`PythonAdder`.

.. code-block:: python

    class PythonAdder( visr.AtomicComponent ):
      """ General-purpose add block for an arbitrary number of inputs"""
      def __init__( self, context, name, parent, numInputs, width ):
      ...

that implements generic addition with :code:`numInputs` signals to be added with
:code:`width` signals each.
Here, the component class :code:`PythonAdder` is contained in a source file :code:`pythonAtoms.py`. 
      
The :code:`python_runner` can be invoked using positional arguments through

.. code-block:: bash

   $> python_runner -D PortAudio -f 48000 -p 512
       -m $HOME/VISR/src/python/scripts/pythonAtoms -c PythonAdder -a "3,2"

which creates a :code:`PythonAdder` component with three inputs and a width of two.

The same component is constructed with the keyword argument option as 

.. code-block:: bash

   $> python_runner -D PortAudio -f 48000 -p 512
       -m $HOME/VISR/src/python/scripts/pythonAtoms -c PythonAdder -k "{'width':2, 'numInputs':3}"

Positional and keyword arguments can also be mixed, as long as the corresponding Python rules are observed:

.. code-block:: bash

   $> python_runner -D PortAudio -f 48000 -p 512
       -m $HOME/VISR/src/python/scripts/pythonAtoms -c PythonAdder -a "3," -k "{'width':2}"

Note the trailing comma for the positional option.

So far, the examples specified the path to the module explicitly.
If this path (:code:`$HOME/VISR/src/python/scripts` in the example) is contained in the default Python search path, i.e., :code:`sys.path`, then the pure module name suffices
	
.. code-block:: bash

   $> python_runner -D PortAudio -f 48000 -p 512
       -m pythonAtoms -c PythonAdder -a "3," -k "{'width':2}"

Another way to locate the module is to provide the path through the :code:`module-search-path` option.

.. code-block:: bash

   $> python_runner -D PortAudio -f 48000 -p 512
       -m pythonAtoms -c PythonAdder -a "3," -k "{'width':2}"
       --module-search-path $HOME/VISR/src/python/scripts

Finally, the option :code:`--module-search-path` can also be used to locate modules needed by the
main module. For example, the path to the core VISR modules can be specified in this way, thus
eradicating the need to add them to the default Python search path, for example by adding them to
the :code:`PYTHONPATH` variable.

.. code-block:: bash

   $> python_runner -D PortAudio -f 48000 -p 512
       -m pythonAtoms -c PythonAdder -a "3," -k "{'width':2}"
       --module-search-path
       $HOME/VISR/src/python/scripts,/usr/share/visr/python 


.. _using_visr_using_standalone_renderers_specific_audio_options:

Interface-specific audio options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section described the audio-interface-specific options that can be passed through the 
:code:`--audio-ifc-options` or :code:`--audio-ifc-option-file` arguments.

PortAudio interface
~~~~~~~~~~~~~~~~~~~

The interface-specific options for the PortAudio interface are to be provided as a JSON file,
for example:

.. code-block:: json
   
   {
    "sampleformat": "...",
    "interleaved": "...",
    "hostapi" : "..."
   }

.. note:: When used on the command line using the :code:`--audio-ifc-options` argument, apply the quotation and escaping as described in Section :ref:`using_standalone_renderers_common_options`.

The following options are supported for the PortAudio interface:

**sampleformat** 
  Specifies the PortAudio sample format. Possible values are:
  
   * :code:`signedInt8Bit`
   * :code:`unsignedInt8Bit`
   * :code:`signedInt16Bit`
   * :code:`unsignedInt16Bit`
   * :code:`signedInt24Bit`
   * :code:`unsignedInt24Bit`
   * :code:`signedInt32Bit`
   * :code:`unsignedInt32Bit`
   * :code:`float32Bit` .

**interleaved**:
  Enable/disable interleaved mode, possible values are :code:`true, false`.
 
**hostapi**:
  Used to specify PortAudio backend audio interface. Possible values are:

   - :code:`default`: This activates the default backend
   - :code:`WASAPI` : Supported OS: Windows.
   - :code:`ASIO` : Supported OS: Windows.
   - :code:`WDMKS`: Supported OS: Windows.
   - :code:`DirectSound` : Supported OS: Windows.
   - :code:`CoreAudio` : Supported OS: MacOs.
   - :code:`ALSA` : Supported OS: Linux.
   - :code:`JACK` : Supported OSs: MacOs, Linux. 

  PortAudio aupports a number of other APIs. However, they are outdated or refer to obsolete platforms and therefore should not be used:
  - :code:`SoundManager` (MacOs)
  - :code:`OSS` (Linux)
  - :code:`AL`
  - :code:`BeOS`
  - :code:`AudioScienceHPI` (Linux)

This configuration is an example of usage of PortAudio, with Jack audio interface as backend.

.. code-block:: json

   {
     "sampleformat": "float32Bit", 
     "interleaved": "false", 
     "hostapi" : "JACK"
   }

Jack audio interface
~~~~~~~~~~~~~~~~~~~~

The following options can be provided when using Jack as our top level component’s Audio Interface:

:code:`clientname`:
  Jack Client name for our top level component.
:code:`servername`:
  Jack Server name. If not provided, the default Jack server is used.
:code:`autoconnect`:
   Globally enable/disable the automatic connection of ports.
   Admissible values are :code:`true` and :code:`false`.
   This setting can be overridden specifically for capture and playback ports in the port configuration section described below.
   
:code:`portconfig`: Subset of options regarding the configuration and connection of Jack Ports, see following section.
  
Port Configuration
''''''''''''''''''
The port configuration section allows to individually set properties for the capture, i.e., input, and the playback, i.e., output, ports of an application.

:code:`capture`: Specifies that the following options regard the top level component’s capture ports only
      
   - :code:`autoconnect` : Enable/disable auto connection to an external jack client’s input ports, possible values are :code:`true, false`
   - :code:`port`: Jack ports specification
     
       - :code:`basename`: Common name for all top level component’s capture ports
       - :code:`indices`:  list of port numbers to append to top level component’s capture port name. It is possible to use Matlab’s colon operator to express a list of numbers in a compact fashion (es."0:4" means appending numbers 0 to 3 to port names)
       - :code:`externalport`: Specification of an external jack client to connect to if :code:`autoconnect` is enabled.
             - :code:`client`: Name of an external jack client to use as input for our top level component (es. “system")
             - :code:`portname`: Common name for all external jack client input ports
             - :code:`indices`: List of port numbers that together with :code:` portname` describe existing external jack client input ports. It is possible to use Matlab’s colon operator to express a list of numbers.

:code:`playback`: Specifies that the following options regard the top level component’s playback ports only.
      
   - :code:`autoconnect` : Enable/disable auto connection to an external jack client’s output ports, possible values are :code:`true, false`
   - :code:`port`: Jack ports specification
       - :code:`basename`: Common name for all top level component’s playback ports
       - :code:`indices`:  list of port numbers to append to top level component’s playback port name. It is possible to use Matlab’s colon operator to express a list of numbers in a compact fashion (es."0:4" means appending numbers 0 to 4 to port names)
       - :code:`externalport`: Specification of an external jack client to connect to if :code:`autoconnect` is enabled.
             - :code:`client`: Name of an external jack client to use as output for our top level component (es. “system")
             - :code:`portname`: Common name for all external jack client output ports
             - :code:`indices`: List of port numbers that together with :code:` portname` describe existing external jack client output ports. It is possible to use Matlab’s colon operator to express a list of numbers.

Simple Example
''''''''''''''

This configuration example shows how to auto-connect the Jack input and output ports of an application to the default jack client (:code:`system`), specifying which range of ports to connect.

.. code-block:: json

   {
     "clientname": "BaseRenderer",
     "autoconnect" : "true",
     "portconfig":
     {
       "capture":
       {
         "port":
         [{ "externalport" : {"indices": "1:4"} }]
       },
       "playback":
       {
         "port":
         [{ "externalport" : {"indices": "5:8"} }]
       }
     }
   }
   
.. _figure_jack_config_simple_example:
.. figure:: ../images/jacksimpleexample.jpeg
   :scale: 100 %
   :align: center

   Jack audio complex configuration example.


Complex Example 
'''''''''''''''

Follow a more complex example where auto-connection of ports is performed specifying different jack clients and the ranges of ports to be connected are described both for the top level component and for external clients.

.. code-block:: json

   {
     "clientname": "VisrRenderer",
     "servername": "",
     "autoconnect" : "true",
     "portconfig":
     {
       "capture":
       {
         "autoconnect" : "true",
         "port":
         [
           {
            "basename" : "Baseinput_" ,
            "indices": "0:1",
            "externalport" :
             {
               "client" : "REAPER",
               "portname": "out",
               "indices": "1:2"
             }
           },
           {
            "basename" : "Baseinput_" ,
            "indices": "2:3",
            "externalport" :
             {
               "indices": "4:5"
             }
            }
         ]
       },
       "playback":
       {
         "autoconnect" : "true",
         "port":
         [{
           "basename" : "Baseoutput_" ,
           "indices": "0:1",
           "externalport" :
            {
             "client" : "system",
             "portname": "playback_",
             "indices": "4:5"
            }
          }]
       }
     }
   }

.. _figure_jack_config_complex_example:
.. figure:: ../images/jackexample.jpeg
   :scale: 100 %
   :align: center

   Jack audio complex configuration example.


