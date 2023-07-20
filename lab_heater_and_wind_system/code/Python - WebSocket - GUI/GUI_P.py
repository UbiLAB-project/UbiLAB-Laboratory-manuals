
from PyQt5 import QtCore, QtGui, QtWidgets

import asyncio
import websockets
import pathlib
import ssl
import time
import threading 
import random 
import serial
import os
import sys
import glob
import socket

#Global variables
connected = False
Ts=1
RX_cnt=0
MCUBuffer_RX=0
MCUBuffer_TX=0
ser=0 #Serival variable



def threaded():
        global IPAddr
        # Run server
        print('Server running')
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        start_server = websockets.serve(WebSocket_function, IPAddr, 8800)
        asyncio.get_event_loop().run_until_complete(start_server)
        asyncio.get_event_loop().run_forever()


 # Webscoket Handler function
async def WebSocket_function(websocket, path):
    global MCUBuffer_RX, ser
    t = time.time()
    data = await websocket.recv()
    data1=MCUBuffer_RX.split()
    print("Data IN: " + data)
    if data=='SERVER':
       #print("Time: "+str(elapsed))
        if data1[0]=='DS':
            reply=MCUBuffer_RX
        else:
            reply="ECHO: "+MCUBuffer_RX #Send data from MCU
        await websocket.send(reply)
        elapsed = time.time() - t
    else:
        ser.write(data.encode())


def read_from_port(ser):
    global connected,MCUBuffer_RX,RX_cnt
    while not connected:
        #serin = ser.read()
        connected = True

        while True:
            MCUBuffer_RX= ser.readline().decode() #Read data from ARM
            print('MCU: '+MCUBuffer_RX)
        

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("UbiLAB")
        MainWindow.resize(600, 500)

        font = QtGui.QFont()
        font.setPointSize(12)
        MainWindow.setFont(font)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        self.comboBox = QtWidgets.QComboBox(self.centralwidget)
        self.comboBox.setGeometry(QtCore.QRect(100, 170, 150, 40))
        self.comboBox.setObjectName("comboBox")
        #self.comboBox.addItem("")


        self.STOP = QtWidgets.QPushButton(self.centralwidget)
        self.STOP.setEnabled(True)
        self.STOP.setGeometry(QtCore.QRect(330, 320, 100, 30))
        self.STOP.setObjectName("STOP")
        self.STOP.clicked.connect(lambda: self.close())

        self.RUN = QtWidgets.QPushButton(self.centralwidget)
        self.RUN.setGeometry(QtCore.QRect(170, 320, 100, 30))
        self.RUN.setObjectName("RUN")
        self.RUN.clicked.connect(lambda: self.clickedR())

        self.Text_naslov = QtWidgets.QLabel(self.centralwidget)
        self.Text_naslov.setGeometry(QtCore.QRect(80, 10, 451, 101))

        font = QtGui.QFont()
        font.setPointSize(18)
        self.Text_naslov.setFont(font)
        self.Text_naslov.setMouseTracking(False)
        self.Text_naslov.setAutoFillBackground(False)
        self.Text_naslov.setAlignment(QtCore.Qt.AlignCenter)
        self.Text_naslov.setObjectName("Text_naslov")

        self.text_IP = QtWidgets.QLabel(self.centralwidget)
        self.text_IP.setGeometry(QtCore.QRect(350, 170, 150, 40))
        font = QtGui.QFont()
        font.setPointSize(12)
        self.text_IP.setFont(font)
        self.text_IP.setMouseTracking(False)
        self.text_IP.setAutoFillBackground(False)
        self.text_IP.setAlignment(QtCore.Qt.AlignCenter)
      
        self.text_IP.setObjectName("text_IP")

        self.TextIP = QtWidgets.QLabel(self.centralwidget)
        self.TextIP.setGeometry(QtCore.QRect(360, 135, 101, 31))
        self.TextIP.setObjectName("TextIP")

        self.TextCOM = QtWidgets.QLabel(self.centralwidget)
        self.TextCOM.setGeometry(QtCore.QRect(100, 135, 111, 31))
        self.TextCOM.setObjectName("TextCOM")
        MainWindow.setCentralWidget(self.centralwidget)

        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 600, 34))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)

        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def clickedR(self):
        #print(self.comboBox.currentText())
        #print(self.lineEdit.displayText())
        #def threaded():
        global ser, start_server
        try:
              ser = serial.Serial(self.comboBox.currentText(), 115200)
              thread = threading.Thread(target=read_from_port, args=(ser,))
              thread.start()
              print("Serial-Adapter OK!")
        except:
              print("No Serial-Adapter")

        x=threading.Thread(target=threaded)
        x.start()

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("UbiLAB", "UbiLAB"))
        #self.comboBox.setItemText(0, _translate("UbiLAB", "COM Port"))

        self.STOP.setText(_translate("UbiLAB", "STOP"))
        self.RUN.setText(_translate("UbiLAB", "RUN"))
        self.Text_naslov.setText(_translate("UbiLAB", "UbiLAB - WebSocket python GUI"))
        self.TextIP.setText(_translate("UbiLAB", "IP NASLOV:"))
        self.TextCOM.setText(_translate("UbiLAB", "COM PORT:"))


if __name__ == "__main__":

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()

    serial_p=serial_ports()
    for x in serial_p:
        ui.comboBox.addItem(x)

    IPAddr=socket.gethostbyname(socket.gethostname())
    ui.text_IP.setText(IPAddr)
    print(IPAddr)

    sys.exit(app.exec_())
    
