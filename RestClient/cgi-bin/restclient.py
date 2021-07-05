from dataclasses import dataclass
import socket, struct, threading, time
import json
from msg import *
from controller import *

HOST = 'localhost'
PORT = 12345

def ProcessReceive():
    while True:
        try:
            msg = GetMsg(Message.ClientID)['messages']
            if (len(msg) > 0):
                print(msg)

        except Exception as e:
            pass
        time.sleep(2)

def Client():
    Message.SendMessage(M_BROKER, M_INIT, "Rest")
    print("Your ID: ", Message.ClientID)
    t = threading.Thread(target=ProcessReceive)
    t.start()
    while True: 
        n = int(input("Отправить - 0 \n"))
        if (n == 0):
            id = int(input("Кому: "))
            s = input("Текст: ")
            SendMsg(Message.ClientID, s, id)
        else:
            print("Повторите ввод \n")
Client()