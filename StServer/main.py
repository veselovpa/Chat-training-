from dataclasses import dataclass
import socket, struct, time, threading, sys

M_INIT		= 0
M_EXIT		= 1
M_GETDATA	= 2
M_NODATA	= 3
M_DATA      = 4
M_CONFIRM   = 5
M_GETLIST   = 6
M_CONNECT_TO_STORAGE = 7
M_ALLUSER = 8
M_DATASTORAGE = 9

M_BROKER	= 0
M_ALL		= 10
M_USER		= 100



@dataclass
class MsgHeader:
    To: int = 0
    From: int = 0
    Type: int = 0
    Size: int = 0

    def Send(self, s):
        s.send(struct.pack(f'iiii', self.To, self.From, self.Type, self.Size))

    def Receive(self, s):
        try:
            (self.To, self.From, self.Type, self.Size) = struct.unpack('iiii', s.recv(16))
        except:
            self.Size = 0
            self.Type = M_NODATA

class Message:

    ClientID = 0

    def __init__(self, To = 0, From = 0, Type = M_DATA, Data=""):
        self.Header = MsgHeader(To, From, Type, len(Data))
        self.Data = Data

    def Send(self, s):
        self.Header.Send(s)
        if self.Header.Size > 0:
            s.send(struct.pack(f'{self.Header.Size}s', self.Data.encode('cp866')))

    def Receive(self, s):
        self.Header.Receive(s)
        if self.Header.Size > 0:
            self.Data = struct.unpack(f'{self.Header.Size}s', s.recv(self.Header.Size))[0].decode('cp866')

    def SendMessage(To, Type = M_DATA, Data=""):
        HOST = 'localhost'
        PORT = 12345
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            m = Message(To, Message.ClientID, Type, Data)
            m.Send(s)
            m.Receive(s)
            if m.Header.Type == M_INIT:
                Message.ClientID = m.Header.To
            return m

def ProcessMessages():

    buf = []
    storage = ""
    webID = 0
    while True:
        m = Message.SendMessage(M_BROKER, M_GETDATA)
        if m.Header.Type == M_CONNECT_TO_STORAGE:
            webID = m.Header.From
            buf = []
            print("WEB ID: ", webID)
        else:
            if m.Header.Type == M_DATA:
                print(m.Header.From, ": ", m.Data)
                buf.append(str(m.Header.From) + ": " + m.Data + "<br>")
                storage = " ".join(buf)
                f = open('storage1.txt', 'w')
                for index in buf:
                    f.write(index + '\n')
                Message.SendMessage(webID, M_DATA, storage)
                f.close()
            else:
                time.sleep(1)

def Client():
    msg = Message.SendMessage(M_BROKER, M_INIT, "server")
    print("Your ID:", msg.Header.To)
    t = threading.Thread(target=ProcessMessages)
    t.start()



Client()
