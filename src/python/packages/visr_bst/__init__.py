# -*- coding: utf-8 -*-

# Copyright (C) 2017-2018 Andreas Franck and Giacomo Costantini
# Copyright (C) 2017-2018 University of Southampton

# VISR Binaural Synthesis Toolkit (BST)
# Authors: Andreas Franck and Giacomo Costantini
# Project page: http://cvssp.org/data/s3a/public/BinauralSynthesisToolkit/


# The Binaural Synthesis Toolkit is provided under the ISC (Internet Systems Consortium) license
# https://www.isc.org/downloads/software-support-policy/isc-license/ :

# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


# We kindly ask to acknowledge the use of this software in publications or software.
# Paper citation:
# Andreas Franck, Giacomo Costantini, Chris Pike, and Filippo Maria Fazi,
# “An Open Realtime Binaural Synthesis Toolkit for Audio Research,” in Proc. Audio Eng.
# Soc. 144th Conv., Milano, Italy, 2018, Engineering Brief.
# http://www.aes.org/e-lib/browse.cfm?elib=19525

# The Binaural Synthesis Toolkit is based on the VISR framework. Information about the VISR,
# including download, setup and usage instructions, can be found on the VISR project page
# http://cvssp.org/data/s3a/public/VISR .

# Controllers
from .controllers.dynamic_hrir_controller import DynamicHrirController
from .controllers.virtual_loudspeaker_controller import VirtualLoudspeakerController

# Full renderers
from .renderers.dynamic_hrir_renderer import DynamicHrirRenderer
from .renderers.hoa_object_to_binaural_renderer import HoaObjectToBinauralRenderer
from .renderers.hoa_binaural_renderer import HoaBinauralRenderer
from .renderers.virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer
from .renderers.object_to_virtual_loudspeaker_renderer import ObjectToVirtualLoudspeakerRenderer

from .realtime_renderers.realtime_dynamic_hrir_renderer import RealtimeDynamicHrirRenderer
from .realtime_renderers.realtime_hoa_object_to_binaural_renderer import RealtimeHoaObjectToBinauralRenderer
from .realtime_renderers.realtime_hoa_binaural_renderer import RealtimeHoaBinauralRenderer
from .realtime_renderers.realtime_virtual_loudspeaker_renderer import RealtimeVirtualLoudspeakerRenderer

# Import HOA components
from . import hoa_components

# Import utility function subdirectory.
from . import util

# Import library of tracking receivers
from . import tracker
