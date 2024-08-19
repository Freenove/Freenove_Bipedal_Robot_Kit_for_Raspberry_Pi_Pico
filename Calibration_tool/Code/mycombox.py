from PyQt5.QtWidgets import *
from PyQt5.QtCore import pyqtSignal
class MyComBox(QComboBox):
    clicked = pyqtSignal()
    def mousePressEvent(self,event):
        super().mousePressEvent(event)
        self.clicked.emit()
        
        
