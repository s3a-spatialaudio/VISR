# -*- coding: utf-8 -*-
"""
Created on Sun Sep  3 09:56:18 2017

@author: Andreas Franck a.franck@soton.ac.uk
"""

import visr
import rrl
import pml
import audiointerfaces as ai

from audio_meter import SurroundLoudnessMeter

from PyQt5 import QtCore, QtGui
import pyqtgraph as pg
import numpy as np
import sys

numChannels = 2
audioInterfaceName = 'Jack'
audioBackendOptions = '{}'
samplingFrequency = 48000
blockSize = 1024

numPlotPoints = 640

class LoudnessGui( QtGui.QWidget ): # QtGui.QMainWindow
    def __init__( self ):
        # Set up the UI
        super( LoudnessGui, self ).__init__( None )
        self.audioRunning = False
        self.setGeometry( 0, 0, 800, 600 )
        # self.setWindowLabel( "Loudness meter" ) # Only for QMainWindow
        
#        self.centralWidget = QtGui.QWidget( self )
#        self.layout = QtGui.QGridLayout( self.centralWidget )
        self.layout = QtGui.QGridLayout( self )
        
#        self.layout.setColumnStretch(1, 4)
#        self.layout.setColumnStretch(2, 4)
        
        
        self.startButton = QtGui.QPushButton( "Start Audio" ) # , self.centralWidget )
        #self.startButton.setMinimumSize( 200, 60 )
        self.stopButton = QtGui.QPushButton( "Stop Audio" ) # , self.centralWidget )
        #self.stopButton.setMinimumSize( 200, 60 )
        self.layout.addWidget( self.startButton, 0, 0 ) # , QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter )
        self.layout.addWidget( self.stopButton, 0, 1 ) # , QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter )
        
        self.loudnessLabel = QtGui.QLabel( '--- dB' ) # , self.centralWidget )
        #self.loudnessLabel.setMinimumSize( 200, 60 )
        
        self.layout.addWidget( self.loudnessLabel, 0, 2 ) # , QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter )
        
        self.startButton.clicked.connect( self.startAudio )
        self.stopButton.clicked.connect( self.stopAudio )
        
        self.plotWidget = pg.PlotWidget()
        self.plotWidget.setRange( xRange = (0,numPlotPoints-1), yRange = (-130.0,6.0))
        self.layout.addWidget( self.plotWidget, 1, 0, 1, 4 )
        self.loudnessPlot = pg.PlotCurveItem()
        self.plotWidget.addItem( self.loudnessPlot )
        
        self.loudnessHist = -120.0 * np.ones( numPlotPoints, dtype=np.float32 )
        self.loudnessPlot.setData( self.loudnessHist )
        
        # %% Setup the DSP part
        
        self.context = visr.SignalFlowContext( period = blockSize, 
                                               samplingFrequency = samplingFrequency )
        self.meter = SurroundLoudnessMeter( self.context, 'meter', None, numChannels, 
                                           gatingPeriod = 0.4 )
        
        audioOptions = ai.AudioInterface.Configuration( numChannels,
                                                             numChannels,
                                                             samplingFrequency,
                                                             blockSize )
        self.audioInterface = ai.AudioInterfaceFactory.create( audioInterfaceName,
                                                              audioOptions,
                                                              audioBackendOptions )
        self.flow = rrl.AudioSignalFlow( self.meter )
        
        self.loudnessPort = self.flow.parameterSendPort( 'loudnessOut' )
        
        self.readTimer = QtCore.QTimer( self )
        self.readTimer.timeout.connect( self.getMeterValues )
        self.readTimer.setSingleShot( False )
        self.readTimer.setInterval( 100 ) # ms
        
    def startAudio( self ):
        if self.audioRunning:
            return
        self.audioInterface.registerCallback( self.flow )
        self.audioInterface.start()
        self.audioRunning = True
        self.readTimer.start()
        return
    def stopAudio( self ):
        if not self.audioRunning:
            return
        self.audioInterface.stop()
        self.audioInterface.unregisterCallback()
        self.audioRunning = False
        self.readTimer.stop()
        self.loudnessLabel.setText( '--- dB' )
        return
    def getMeterValues( self ):
        numPoints = self.loudnessPort.size()
        if numPoints > 0:
            self.loudnessHist = np.roll( self.loudnessHist, numPoints )
            for idx in range( 0, numPoints ):
                Lk = self.loudnessPort.front()
                self.loudnessPort.pop()
                self.loudnessHist[idx] = Lk.value
            self.loudnessLabel.setText( '%f dB' % Lk.value )
            self.loudnessPlot.setData( self.loudnessHist )
                
        
#        while not self.loudnessPort.empty():
#            Lk = self.loudnessPort.front()
#            self.loudnessLabel.setText( '%f dB' % Lk.value )
#            self.loudnessPort.pop()

app = QtGui.QApplication( sys.argv )
mainWnd = LoudnessGui()

mainWnd.show()
appReturnValue =  app.exec_()
#sys.exit( appReturnValue )
#sys.exit( app.exec_() )
