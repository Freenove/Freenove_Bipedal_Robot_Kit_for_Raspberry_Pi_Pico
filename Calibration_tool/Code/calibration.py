from calibration_tool_ui import *
from SimpleSerial import *
import time
import sys
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

class MyMainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.myserial = SimpleSerial()
        self.setWindowIcon(QIcon('image/logo_Mini.png'))
        self.label_Video.setPixmap(QPixmap('image/Robot.png'))
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")
        self.update_coms()
        self.point = self.Read_from_txt('point')
        self.commandFlag = 1
        self.cmdArray = ''
        self.endChar = '\n'
        self.robotendChar = '\r\n'
        self.intervalChar = '#'
        self.calibrationCommand = 'G'
        self.Servo_MoveCommand = 'A'
        self.Get_valueCommand = 'F'
        #  Right_Leg
        self.servo1 = self.point[0][0]
        self.Right_Leg_Angle.setText(str(self.servo1))
        self.Right_Leg_Min.setText('50')
        self.Right_Leg_Max.setText('130')
        self.Right_Leg_draw_slip.setMinimum(50)
        self.Right_Leg_draw_slip.setMaximum(130)
        self.Right_Leg_draw_slip.setSingleStep(1)
        self.Right_Leg_draw_slip.setValue(self.servo1)
        #  Right_Foot
        self.servo2 = self.point[0][1]
        self.Right_Foot_Angle.setText(str(self.servo2))
        self.Right_Foot_Min.setText('50')
        self.Right_Foot_Max.setText('130')
        self.Right_Foot_draw_slip.setMinimum(50)
        self.Right_Foot_draw_slip.setMaximum(130)
        self.Right_Foot_draw_slip.setSingleStep(1)
        self.Right_Foot_draw_slip.setValue(self.servo2)
        #  Left_Leg
        self.servo3 = self.point[0][2]
        self.Left_Leg_Angle.setText(str(self.servo3))
        self.Left_Leg_Min.setText('50')
        self.Left_Leg_Max.setText('130')
        self.Left_Leg_draw_slip.setMinimum(50)
        self.Left_Leg_draw_slip.setMaximum(130)
        self.Left_Leg_draw_slip.setSingleStep(1)
        self.Left_Leg_draw_slip.setValue(int(self.servo3))
        #  Left_Foot
        self.servo4 = self.point[0][3]
        self.Left_Foot_Angle.setText(str(self.servo4))
        self.Left_Foot_Min.setText('50')
        self.Left_Foot_Max.setText('130')
        self.Left_Foot_draw_slip.setMinimum(50)
        self.Left_Foot_draw_slip.setMaximum(130)
        self.Left_Foot_draw_slip.setSingleStep(1)
        self.Left_Foot_draw_slip.setValue(self.servo4)
        self.open_pushButton.clicked.connect(self.open_serial)
        self.com_comboBox.clicked.connect(self.update_coms)
        self.Right_Leg_draw_slip.valueChanged.connect(self.Change_Left_Right)
        self.Right_Leg_Add.clicked.connect(self.on_btn_Right)
        self.Right_Leg_Home.clicked.connect(self.on_btn_Home)
        self.Right_Leg_Sub.clicked.connect(self.on_btn_Left)
        self.Right_Foot_draw_slip.valueChanged.connect(self.Change_Left_Right2)
        self.Right_Foot_Add.clicked.connect(self.on_btn_Right2)
        self.Right_Foot_Home.clicked.connect(self.on_btn_Home2)
        self.Right_Foot_Sub.clicked.connect(self.on_btn_Left2)
        self.Left_Leg_draw_slip.valueChanged.connect(self.Change_Left_Right3)
        self.Left_Leg_Add.clicked.connect(self.on_btn_Right3)
        self.Left_Leg_Home.clicked.connect(self.on_btn_Home3)
        self.Left_Leg_Sub.clicked.connect(self.on_btn_Left3)
        self.Left_Foot_draw_slip.valueChanged.connect(self.Change_Left_Right4)
        self.Left_Foot_Add.clicked.connect(self.on_btn_Right4)
        self.Left_Foot_Home.clicked.connect(self.on_btn_Home4)
        self.Left_Foot_Sub.clicked.connect(self.on_btn_Left4)
        self.Servo_Move_Forward.clicked.connect(self.Servo_Move_test_Forward)
        self.Servo_Move_Back.clicked.connect(self.Servo_Move_test_Back)
        self.Servo_Move_Left.clicked.connect(self.Servo_Move_test_Left)
        self.Servo_Move_Light.clicked.connect(self.Servo_Move_test_Light)
        self.Servo_Move_Home.clicked.connect(self.Servo_Move_test_Home)
        self.Save_Calibration.clicked.connect(self.Save_Calibration_value)
        self.Get_value.clicked.connect(self.Get_servo_value)

    def mousePressEvent(self, event):
        pass

    def on_btn_Left(self):
        self.servo1 = self.servo1 + 1
        if self.servo1 >= 130:
            self.servo1 = 130
        self.Right_Leg_draw_slip.setValue(self.servo1)

    def on_btn_Right(self):
        self.servo1 = self.servo1 - 1
        if self.servo1 <= 50:
            self.servo1 = 50
        self.Right_Leg_draw_slip.setValue(self.servo1)

    def on_btn_Home(self):
        self.servo1 = 90
        self.Right_Leg_draw_slip.setValue(self.servo1)

    def Change_Left_Right(self):
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: red;")
        self.data = self.calibrationCommand + self.intervalChar + str(self.servo3) + self.intervalChar + str(
            self.servo1) + self.intervalChar + str(self.servo4) + self.intervalChar + str(
            self.servo2) + self.intervalChar + str(0) + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        print(self.data)
        self.statusBar().showMessage(self.data, 1000)
        self.servo1 = self.Right_Leg_draw_slip.value()
        self.Right_Leg_Angle.setText("%d" % self.servo1)

    def on_btn_Left2(self):
        self.servo2 = self.servo2 + 1
        if self.servo2 >= 130:
            self.servo2 = 130
        self.Right_Foot_draw_slip.setValue(self.servo2)

    def on_btn_Right2(self):
        self.servo2 = self.servo2 - 1
        if self.servo2 <= 50:
            self.servo2 = 50
        self.Right_Foot_draw_slip.setValue(self.servo2)

    def on_btn_Home2(self):
        self.servo2 = 90
        self.Right_Foot_draw_slip.setValue(self.servo2)

    def Change_Left_Right2(self):
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: red;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")
        self.data = self.calibrationCommand + self.intervalChar + str(self.servo3) + self.intervalChar + str(
            self.servo1) + self.intervalChar + str(self.servo4) + self.intervalChar + str(
            self.servo2) + self.intervalChar + str(0) + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        print(self.data)
        self.statusBar().showMessage(self.data, 1000)
        self.servo2 = self.Right_Foot_draw_slip.value()
        self.Right_Foot_Angle.setText("%d" % self.servo2)

    def on_btn_Left3(self):
        self.servo3 = self.servo3 + 1
        if self.servo3 >= 130:
            self.servo3 = 130
        self.Left_Leg_draw_slip.setValue(self.servo3)

    def on_btn_Right3(self):
        self.servo3 = self.servo3 - 1
        if self.servo3 <= 50:
            self.servo3 = 50
        self.Left_Leg_draw_slip.setValue(self.servo3)

    def on_btn_Home3(self):
        self.servo3 = 90
        self.Left_Leg_draw_slip.setValue(self.servo3)

    def Change_Left_Right3(self):
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: red;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")
        self.data = self.calibrationCommand + self.intervalChar + str(self.servo3) + self.intervalChar + str(
            self.servo1) + self.intervalChar + str(self.servo4) + self.intervalChar + str(
            self.servo2) + self.intervalChar + str(0) + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        print(self.data)
        self.statusBar().showMessage(self.data, 1000)
        self.servo3 = self.Left_Leg_draw_slip.value()
        self.Left_Leg_Angle.setText("%d" % self.servo3)

    def on_btn_Left4(self):
        self.servo4 = self.servo4 + 1
        if self.servo4 >= 130:
            self.servo4 = 130
        self.Left_Foot_draw_slip.setValue(self.servo4)

    def on_btn_Right4(self):
        self.servo4 = self.servo4 - 1
        if self.servo4 <= 50:
            self.servo4 = 50
        self.Left_Foot_draw_slip.setValue(self.servo4)

    def on_btn_Home4(self):
        self.servo4 = 90
        self.Left_Foot_draw_slip.setValue(self.servo4)

    def Change_Left_Right4(self):
        self.Left_Foot_Image.setStyleSheet("background-color: red;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")
        self.data = self.calibrationCommand + self.intervalChar + str(self.servo3) + self.intervalChar + str(
            self.servo1) + self.intervalChar + str(self.servo4) + self.intervalChar + str(
            self.servo2) + self.intervalChar + str(0) + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        print(self.data)
        self.statusBar().showMessage(self.data, 1000)
        self.servo4 = self.Left_Foot_draw_slip.value()
        self.Left_Foot_Angle.setText("%d" % self.servo4)
    def Servo_Move_test_Forward(self):  # Servo_Move
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")

        self.data = self.Servo_MoveCommand + self.intervalChar + str(1) + self.intervalChar + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        self.statusBar().showMessage(self.data, 1000)
        print('Forward')

    def Servo_Move_test_Back(self):  # Servo_Move
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")

        self.data = self.Servo_MoveCommand + self.intervalChar + str(2) + self.intervalChar + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        self.statusBar().showMessage(self.data, 1000)
        print('Back')

    def Servo_Move_test_Left(self):  # Servo_Move
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")

        self.data = self.Servo_MoveCommand + self.intervalChar + str(3) + self.intervalChar + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        self.statusBar().showMessage(self.data, 1000)
        print('Left')

    def Servo_Move_test_Light(self):  # Servo_Move
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")

        self.data = self.Servo_MoveCommand + self.intervalChar + str(4) + self.intervalChar + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        self.statusBar().showMessage(self.data, 1000)
        print('Right')

    def Servo_Move_test_Home(self):  # Servo_home
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")

        self.data = self.Servo_MoveCommand + self.intervalChar + str(0) + self.intervalChar + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        self.myserial.send(data_bytes2)
        self.statusBar().showMessage(self.data, 1000)
        print('Stop')

    def Save_Calibration_value(self):  # Save_Calibration_value
        self.Left_Foot_Image.setStyleSheet("background-color: blue;")
        self.Right_Foot_Image.setStyleSheet("background-color: blue;")
        self.Left_Leg_Image.setStyleSheet("background-color: blue;")
        self.Right_Leg_Image.setStyleSheet("background-color: blue;")
        self.data = self.calibrationCommand + self.intervalChar + str(self.servo3) + self.intervalChar + str(
            self.servo1) + self.intervalChar + str(self.servo4) + self.intervalChar + str(
            self.servo2) + self.intervalChar + str(1) + self.endChar
        data_bytes2 = bytes(self.data, encoding="gbk")
        print('Save the current steering gear calibration value.')
        print(self.data)
        self.myserial.send(data_bytes2)
        self.statusBar().showMessage(self.data, 1000)
        self.point[0][0] = self.servo1
        self.point[0][1] = self.servo2
        self.point[0][2] = self.servo3
        self.point[0][3] = self.servo4
        self.Save_to_txt(self.point, 'point')

    def update_coms(self):
        self.coms = self.myserial.get_com_list()
        self.com_comboBox.clear()
        self.com_comboBox.addItems(self.coms)
        # print(self.coms)

    def update_com_paras(self):
        self.port = self.com_comboBox.currentText()
        self.baudrate = 9600
        self.parity = "N"
        self.stopbits = 1
        self.bytesize = 8
        self.com_paras_dic = {
            "port": self.port,
            "baudrate": self.baudrate,
            "parity": self.parity,
            "stopbits": self.stopbits,
            "bytesize": self.bytesize
        }

    def open_serial(self):
        if self.open_pushButton.text() == "Open the serial port":
            self.update_com_paras()
            self.myserial.open(**self.com_paras_dic)
            if(self.myserial.IS_OPEN == True):
                self.open_pushButton.setText("Close the serial port")
            else:
                self.open_pushButton.setText("Open the serial port")
        else:
            self.close_serial()

    def close_serial(self):
        self.myserial.close()
        self.open_pushButton.setText("Open the serial port")

    def Read_from_txt(self,filename):
        file1 = open(filename + ".txt", "r")
        list_row = file1.readlines()
        list_source = []
        for i in range(len(list_row)):
            column_list = list_row[i].strip().split("\t")
            list_source.append(column_list)
        for i in range(len(list_source)):
            for j in range(len(list_source[i])):
                list_source[i][j] = int(list_source[i][j])
        file1.close()
        return list_source

    def Save_to_txt(self,list, filename):
        file2 = open(filename + '.txt', 'w')
        for i in range(len(list)):
            for j in range(len(list[i])):
                file2.write(str(list[i][j]))
                file2.write('\t')
            file2.write('\n')
        file2.close()

    def Get_servo_value(self):  # Get_servo_value
        if self.open_pushButton.text() != "Open the serial port":
            try:
                self.Left_Foot_Image.setStyleSheet("background-color: blue;")
                self.Right_Foot_Image.setStyleSheet("background-color: blue;")
                self.Left_Leg_Image.setStyleSheet("background-color: blue;")
                self.Right_Leg_Image.setStyleSheet("background-color: blue;")
                self.data = self.Get_valueCommand + self.robotendChar
                data_bytes2 = bytes(self.data, encoding="gbk")
                self.myserial.send(data_bytes2)
                self.statusBar().showMessage(self.data, 1000)
                time.sleep(0.3)
                read_data_bytes=self.myserial.read()
                time.sleep(0.3)
                AllData = str(read_data_bytes, encoding="gbk")

                if AllData == '':
                    pass
                else:
                    self.cmdArray = AllData.split("\n")
                for oneCmd in self.cmdArray:
                    receiveddata = oneCmd.split("#")
                    if self.Get_valueCommand in receiveddata:
                        self.servo3 = int(receiveddata[1])
                        self.servo1 = int(receiveddata[2])
                        self.servo4 = int(receiveddata[3])
                        self.servo2 = int(receiveddata[4])
                    # print(receiveddata)
                self.Right_Leg_draw_slip.setValue(self.servo1)
                self.Right_Foot_draw_slip.setValue(self.servo2)
                self.Left_Leg_draw_slip.setValue(self.servo3)
                self.Left_Foot_draw_slip.setValue(self.servo4)

                self.servo4 = self.Left_Foot_draw_slip.value()
                self.servo3 = self.Left_Leg_draw_slip.value()
                self.servo2 = self.Right_Foot_draw_slip.value()
                self.servo1 = self.Right_Leg_draw_slip.value()

                self.Left_Foot_Angle.setText(str(self.servo4))
                self.Left_Leg_Angle.setText(str(self.servo3))
                self.Right_Leg_Angle.setText(str(self.servo1))
                self.Right_Foot_Angle.setText(str(self.servo2))
            except Exception as e:
                print("Please check that the serial port is correctly selected.")
        else :
            print("Please open a serial port! ")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    my_window = MyMainWindow()
    my_window.show()
    sys.exit(app.exec_())
