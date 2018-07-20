Installing visr!
================================


The installation packages are provided in the S3A wiki http://s3a-spatialaudio.org/wiki/index.php/Software#Installation

@subsection linux_installation Linux Installation
Type the following on a shell:

@code sudo dpkg -i VISR-X.X.X.tar-Linux.deb @endcode

Dependencies should be installed automatically
Binaries are in the system path (/usr/bin/)

@subsection windows_installation Windows Installation
Execute the graphical installer.

If not present already, download and install the Visual Studio 2015 C++ Redistributable packages (64 Bit):

https://www.microsoft.com/en-us/download/details.aspx?id=53587

The Windows installer contains the required third-party libraries (portaudio, libsndfile) in the same directories as the executables.
To execute binaries, open an shell (e.g., cmd, navigate to the bin/, and start the binaries using the command line and options.

@subsection macosx_installation MacOS X Installation
Install Jack, preferably JackOSX http://www.jackosx.com/
Beta version https://github.com/jackaudio/jackaudio.github.com/releases/download/1.9.11/JackOSX.0.92_b3.zip seems to cause less trouble (and has a working qjackctl).

Double-click on <tt>VISR-X.X.X-Darwin.dmg</tt> installer and follow the instructions.

When the image is mounted, a new window should appear with a link to the <tt>Applications</tt> folder and a <tt>VISR-X.X.X-Darwin</tt> floder. Just drag that folder into <tt>Applications</tt> and the installation is completed.

@section installation_instructions_python_setup Using VISR Python components

If you want to use the VISR framework as python packages, follow the instructions in @ref python_support_setting_up_runtime_env to set up the runtime environment variables.
