import os
from SimpleSerial import *
from PyQt5.QtCore import QThread,pyqtSignal
import threading
class Serial_receive_thread(QThread):
    signal_have_data = pyqtSignal(bytes)
    signal_receive_error = pyqtSignal()
    def __init__(self):
        super().__init__()
        
    def start_receive_data(self,serial_class):
        self.ser = serial_class
        self.start()
    def run(self):
        # print("receive_data start")
        # print("receive: ",threading.current_thread().ident)
        try:
            receive_data = self.ser.read()
            if len(receive_data) != 0:
                self.signal_have_data.emit(receive_data)
        except:
            self.signal_receive_error.emit()