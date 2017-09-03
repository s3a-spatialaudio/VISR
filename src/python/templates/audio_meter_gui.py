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

from PyQt4 import QtCore, QtGui
import pyqtgraph as pg
import sys

numChannels = 2
audioInterfaceName = 'PortAudio'
audioBackendOptions = '{}'
samplingFrequency = 48000
blockSize = 1024

class LoudnessGui( QtGui.QMainWindow ):
    def __init__( self ):
        super( LoudnessGui, self ).__init__( None )
        self.audioRunning = False
        self.setGeometry( 0, 0, 800, 600 )
        
        self.centralWidget = QtGui.QWidget( self )
        
        self.layout = QtGui.QGridLayout( self.centralWidget )
        
        self.startButton = QtGui.QPushButton( "Start Audio", self.centralWidget )
        self.stopButton = QtGui.QPushButton( "Stop Audio", self.centralWidget )
        self.layout.addWidget( self.startButton, 0, 0, QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter )
        self.layout.addWidget( self.stopButton, 0, 1, QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter )
        
        self.loudnessLabel = QtGui.QLabel( '--- dB', self.centralWidget )
        self.layout.addWidget( self.loudnessLabel, 0, 2, QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter )
        
        self.startButton.clicked.connect( self.startAudio )
        self.stopButton.clicked.connect( self.stopAudio )
        
        context = visr.SignalFlowContext( period = blockSize, 
                                         samplingFrequency = samplingFrequency )
        self.meter = SurroundLoudnessMeter( context, 'meter', None, numChannels, 
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
        
        self.startAudio()
        
    def startAudio( self ):
        if self.audioRunning:
            return
        self.audioInterface.registerCallback( self.flow )
#        self.audioInterface.start()
#        self.audioRunning = True
        # self.readTimer.start()
        return
    def stopAudio( self ):
        if not self.audioRunning:
            return
        self.audioInterface.stop()
        self.audioInterface.unregisterCallback( self.flow )
        self.audioRunning = False
        self.readTimer.stop()
        return
    def getMeterValues( self ):
        while not self.loudnessPort.empty():
            Lk = self.loudnessPort.front()
            self.LoudnessLabel.setText( '%f dB' % Lk.value )
            self.loudnessPort.pop()

app = QtGui.QApplication( sys.argv )
mainWnd = LoudnessGui()

mainWnd.show()
appReturnValue =  app.exec_()
#sys.exit( appReturnValue )
#sys.exit( app.exec_() )
