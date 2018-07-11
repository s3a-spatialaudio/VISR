# -*- coding: utf-8 -*-
"""
Created on Sun Sep  3 09:56:18 2017

@author: Andreas Franck a.franck@soton.ac.uk
"""


from PyQt4 import QtCore, QtGui, QtNetwork
import sys

sys.path.append( '/home/andi/dev/metadapter/thirdparty' )

import OSC

class ABComparison( QtGui.QWidget ): # QtGui.QMainWindow QWidget
    def __init__( self ):
        super( ABComparison, self ).__init__( None )
#        okButton = QtGui.QPushButton("OK")
#        cancelButton = QtGui.QPushButton("Cancel")
#
#        hbox = QtGui.QHBoxLayout()
#        hbox.addStretch(1)
#        hbox.addWidget(okButton)
#        hbox.addWidget(cancelButton)
#
#        vbox = QtGui.QVBoxLayout()
#        vbox.addStretch(1)
#        vbox.addLayout(hbox)
#
#        self.setLayout(vbox)
#
#        self.setGeometry(300, 300, 300, 150)
#        self.setWindowTitle('Buttons')
#        self.show()
#        # Set up the UI
        self.setGeometry( 0, 0, 200, 100 )
        self.setWindowTitle('A/B Comparison')

        self.layout = QtGui.QHBoxLayout()

        self.aButton = QtGui.QPushButton( "A" )
        self.aButton.setMinimumSize( 60, 60 )
        self.layout.addWidget( self.aButton )

        self.bButton = QtGui.QPushButton( "B" )
        self.bButton.setMinimumSize( 60, 60 )
        self.layout.addWidget( self.bButton )


        self.nwSocket = QtNetwork.QUdpSocket( self )
        self.nwSocket.bind( QtNetwork.QHostAddress.LocalHost )

        self.aButton.clicked.connect( self.sendA )
        self.bButton.clicked.connect( self.sendB )

        self.setLayout( self.layout )

    def sendA( self ):
        self.sendMessage( 0 )

    def sendB( self ):
        self.sendMessage( 1 )

    def sendMessage( self, value ):
        # self.nwSocket.writeDatagram( b'bang', QtNetwork.QHostAddress.LocalHost, 8889 )
        msg = OSC.OSCMessage('/switch' )
        msg.append( float(value))
        nwMsg = msg.getBinary()
        self.nwSocket.writeDatagram( nwMsg, QtNetwork.QHostAddress.LocalHost, 12345 )
        return

app = QtGui.QApplication( sys.argv )
mainWnd = ABComparison()

mainWnd.show()
appReturnValue =  app.exec_()
