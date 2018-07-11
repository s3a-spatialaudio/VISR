# -*- coding: utf-8 -*-
"""
Created on Tue Dec 26 16:27:36 2017

@author: andi
"""


import sys
from PyQt4 import QtGui

#class Example(QtGui.QWidget):
#
#    def __init__(self):
#        super(Example, self).__init__()
#
#        self.initUI()
#
#    def initUI(self):
#
#        lbl1 = QtGui.QLabel('ZetCode', self)
#        lbl1.move(15, 10)
#
#        lbl2 = QtGui.QLabel('tutorials', self)
#        lbl2.move(35, 40)
#
#        lbl3 = QtGui.QLabel('for programmers', self)
#        lbl3.move(55, 70)
#
#        self.setGeometry(300, 300, 250, 150)
#        self.setWindowTitle('Absolute')
#        self.show()
#
#def main():
#
#    app = QtGui.QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())
#
#
#if __name__ == '__main__':
#    main()

import sys
from PyQt4 import QtGui

class Example(QtGui.QWidget):

    def __init__(self):
        super(Example, self).__init__()

        self.initUI()

    def initUI(self):

        okButton = QtGui.QPushButton("OK")
        cancelButton = QtGui.QPushButton("Cancel")

        hbox = QtGui.QHBoxLayout()
        hbox.addStretch(1)
        hbox.addWidget(okButton)
        hbox.addWidget(cancelButton)

        vbox = QtGui.QVBoxLayout()
        vbox.addStretch(1)
        vbox.addLayout(hbox)

        self.setLayout(vbox)

        self.setGeometry(300, 300, 300, 150)
        self.setWindowTitle('Buttons')
        self.show()

def main():

    app = QtGui.QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()