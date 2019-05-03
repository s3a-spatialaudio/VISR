# -*- coding: utf-8 -*-
"""
Created on Sun Sep  3 09:56:18 2017

@author: Andreas Franck a.franck@soton.ac.uk
"""


from PyQt4 import QtCore, QtGui, QtNetwork
import sys

class CalibrateButton( QtGui.QMainWindow ): # QtGui.QMainWindow QWidget
    def __init__( self ):
        # Set up the UI
        super( CalibrateButton, self ).__init__( None )
        self.audioRunning = False
        self.setGeometry( 0, 0, 140, 140 )

        self.layout = QtGui.QGridLayout( self )

        self.calibrateButton = QtGui.QPushButton( "Calibrate", self ) # , self.centralWidget )
        self.calibrateButton.setMinimumSize( 80, 80 )
        self.layout.addWidget( self.calibrateButton, 0, 0 , QtCore.Qt.AlignHCenter | QtCore.Qt.AlignVCenter )


        self.nwSocket = QtNetwork.QUdpSocket( self )
        self.nwSocket.bind( QtNetwork.QHostAddress.LocalHost, 8889 )

        self.calibrateButton.clicked.connect( self.sendMessage )


    def sendMessage( self ):
        self.nwSocket.writeDatagram( b'bang', QtNetwork.QHostAddress.LocalHost, 8889 )
        return

app = QtGui.QApplication( sys.argv )
mainWnd = CalibrateButton()

mainWnd.show()
appReturnValue =  app.exec_()

