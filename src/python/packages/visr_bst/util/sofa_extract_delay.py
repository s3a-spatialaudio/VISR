# -*- coding: utf-8 -*-
"""
Created on Sat Oct  7 17:15:03 2017

@author: af5u13
"""

import os
import shutil
import numpy as np
import h5py

def extractDelayInSofaFile( inFileName, outFileName, dtype = np.float32, fdAdjust = False ):
    if not os.path.exists( inFileName ):
        raise ValueError( "SOFA file does not exist." )
    try:
        shutil.copyfile( inFileName, outFileName )
        h = h5py.File( outFileName, 'r+' )

        # hrir = np.asarray( h.get('Data.IR'), dtype=dtype )
        hrir = h['Data.IR']
 
        HRTF = np.fft.rfft( hrir, axis = -1 )

        wrappedPhase = np.angle( HRTF )
        unwrappedPhase = np.unwrap( wrappedPhase, axis = -1 )

        hrirLen = hrir.shape[-1]
        dftPoints = hrirLen
        fGridSize = HRTF.shape[-1]

        freqGridRad = 2.0*np.pi*np.arange(0.0,float(fGridSize))/dftPoints

        # The (X^T*X)^-1*X^T part of a linear regression
        XTXinvXT = 1.0/np.dot(freqGridRad,freqGridRad)*freqGridRad
                             
        # Calculate the phase slope (i.e, the first-order approximation of the 
        # negative time delay) simultaneously for all HRTFS
        beta = np.dot( unwrappedPhase, XTXinvXT )

        tdSamples = - beta

        minDelay = np.min( tdSamples )

        # Truncate to the integer part to avoid fractional delay interpolation.            
        tdAdjusted = np.floor(tdSamples - minDelay)

        tdRep = tdAdjusted.reshape( tdAdjusted.shape + (1,) ).repeat( fGridSize, axis=-1)

        # Whether to adjust
        if fdAdjust:
            # Adjust the phases 
            adjustedPhase = unwrappedPhase + tdRep * freqGridRad

            # FD adjust causes a circular shift, which might create artifacts at
            # the end of the IR.
            HRTFadjust = np.abs(HRTF) * np.exp( -1j*adjustedPhase )

            hrirAdjust = np.fft.irfft( HRTFadjust )
        else:
            
            # We don't use windowing here, because the end of the IR remains the same as in the original hrir
            # (only the end of the active filter is no longer aligned with the taps array.)
            # We might consider filtering both the onset and the end, but we don't now which treatment the IRs 
            # have already undergone.

            hrirAdjust = np.zeros( hrir.shape, dtype=hrir.dtype )
            for irIndex in np.ndindex( hrir.shape[:-1] ):
                shift = int(tdAdjusted[irIndex])
                hrirAdjust[irIndex][0:hrirLen-shift] = hrir[irIndex][shift:]

        # We need to remove an existing Delay field.
        if 'Data.Delay' in h.keys():
            del h['Data.Delay']

        # Create the new dataset/
        h.create_dataset( 'Data.Delay', tdAdjusted.shape,
                         tdAdjusted.dtype, data=tdAdjusted, compression="gzip" )

        # Alter the IRs
        hrirRef = h['Data.IR'] # We need to get an explicit reference 
        hrirRef[...] = hrirAdjust # to avoid a "Name already exists" error.

        h.close()
    except Exception as ex:
        if not os.path.exists( outFileName ):
            os.remove( outFileName )
        raise ex
    return

# Example code:
if __name__ == '__main__':
    currDir = os.getcwd()
    inFilePath = os.path.join( currDir, 'data/dtf_b_nh169.sofa' )
    outFilePath =  os.path.join( currDir, 'data/dtf_b_nh169_timedelay.sofa' )

    extractDelayInSofaFile( inFilePath, outFilePath )
