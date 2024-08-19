import serial
import serial.tools.list_ports
class SimpleSerial():
    def __init__(self):
        self.IS_OPEN = False
        self.get_com_list()
    def get_com_list(self):
        self.coms = []
        port_list = list(serial.tools.list_ports.comports())
        for i in port_list:
            self.coms.append(str(i).split()[0])
        self.coms.sort(key = lambda s:int(s[3:]))
        return self.coms
    def open(self,
            port,#str: COMX 
            baudrate=9600,#int
            parity="N",#str 'N', 'E', 'O'
            timeout=0, #float
            stopbits=1,#float 1,1.5,2
            bytesize=8):#int 5,6,7,8
        self.port = port
        self.baudrate = baudrate
        self.parity = parity
        self.timeout = timeout
        self.stopbits = stopbits
        self.bytesize = bytesize
        try:
            self.ser = serial.Serial(
                port = self.port,
                baudrate = self.baudrate,
                parity = self.parity,
                timeout = self.timeout,
                stopbits = self.stopbits,
                bytesize = self.bytesize)
            print("open scesess !")
            self.IS_OPEN = True
            return True
        except Exception as e:
            print("except:",e)
            self.IS_OPEN = False
            return False
    def close(self):
        try:
            self.IS_OPEN = False
            self.ser.close()
        except Exception as e:
            print("except:",e)
    def send(self,data):#data: bytes
        try:
            if self.IS_OPEN == True:
                self.ser.write(data)
            else:
                print("send error: com is not open! ")

        except Exception as e:
            print("except:",e)
    def read(self):#Non-blocking
        data = ""
        try: 
            if self.IS_OPEN == True:
                data = self.ser.readall()
                return data
            else:
                print("read error: com is not open! ")
                return data
        except Exception as e:
            print("except:",e)
            raise e
            return data
        

if __name__ == "__main__":
    myserial = SimpleSerial()
    coms = myserial.get_com_list()
    print(coms)
    myserial.open('COM2')
    myserial.send(bytes("hello",'ascii'))
    myserial.send(bytes([1,2,3,4,5,6]))
    myserial.close()

    