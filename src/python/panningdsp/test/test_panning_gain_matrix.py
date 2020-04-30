
import visr
import panningdsp
import rrl

import numpy as np
import matplotlib.pyplot as plt


def test_constantGain():
    fs=48000
    bs=64
    numIns=1
    numOuts=1

    activeIn=0
    activeOut=0

    numBlocks=8
    signalLength=numBlocks*bs
    inSig=np.asarray(2.0*np.random.rand( numIns, signalLength )-1,
                     dtype=np.float32)
    inSig[activeIn,:]=0.75*np.sin( 2*np.pi*880/fs*np.arange(signalLength))
    outSig = np.zeros( (numOuts, signalLength), dtype=np.float32)

    gains = np.zeros((numOuts, numIns), dtype=np.float32)
    gains[activeOut, activeIn] = 0.75


    refOutput = gains @ inSig

    ctxt=visr.SignalFlowContext(period=bs, samplingFrequency=fs)

    comp=panningdsp.PanningGainMatrix(ctxt, "GainMatrix", None,
      numberOfObjects=numIns, numberOfLoudspeakers=numOuts,
      initialGains=gains )

    flow = rrl.AudioSignalFlow( comp )
    gainInput = flow.parameterReceivePort("gainInput")

    for bIdx in range(numBlocks):
        outSig[:,bIdx*bs:(bIdx+1)*bs] = flow.process(inSig[:,bIdx*bs:(bIdx+1)*bs])

    fig, ax = plt.subplots(1,1)
    ax.plot( inSig[activeIn,:], 'g-' )
    ax.plot( refOutput, 'bo-' )
    ax.plot( outSig[activeOut,:], 'm.-')

    fig2, ax2 = plt.subplots(1,1)
    ax2.plot( outSig[activeOut,:]-refOutput, 'r.-')
    fig2.suptitle('Error (output-reference)')

    fig3, ax3 = plt.subplots(1,1)
    ax3.plot( np.diff(refOutput), 'bo-', label='ref' )
    ax3.plot( np.diff(outSig[activeOut,:]), 'm.-', label='output')
    fig3.suptitle( 'Finite differences' )
    ax3.legend()
    plt.show(block=True)

    assert np.all(np.abs(outSig[activeOut,:] - refOutput )
                  <= 5*np.finfo(np.float32).eps)

# Enable to run the unit test as a script.
if __name__ == "__main__":
    test_constantGain()
