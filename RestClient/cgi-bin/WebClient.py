# -*- coding: utf-8 -*-

import os, sys, re, codecs, binascii, cgi, cgitb, datetime, pickle, json
from msg import *

cgitb.enable()
sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())

class Messenger:
	def __init__(self, q):
		self.q = q
		Message.ClientID = int(q.getvalue('ClientID', 0))
		if Message.ClientID == 0:
			Message.SendMessage(M_BROKER, M_INIT, "WebClient")
		self.MessageText = ''
		self.msg = ''
		self.users = ''

	def PrintPage(self):
		print(f"""Content-type: text/html; charset=utf-8

<html><head><title>Messages</title></head>
<body>
<center>
<form action=/cgi-bin/WebClient.py name=msgform>
Ваш ID = {Message.ClientID} <br>
<input type=hidden name=type value="send">
<input type=hidden name=msg>
<input type=hidden name=ClientID value="{Message.ClientID}">
<input type=text name=message value="{self.MessageText}" placeholder = "Введите сообщение"> <br>
<input type = text name = id placeholder = "Введите ID">
<br>
<input type=submit value="Send">
<input type=button value="Get" onclick="document.forms.msgform.type.value='get'; document.forms.msgform.submit();">
</form>

<span>Входящие: <br><p align="left"> {self.msg}</span></div>
</center></body></html>
	""")


	def MsgSend(self):
		id = ''
		try:
			id = int(self.q.getvalue('id'))
		except:
			pass

		message = self.q.getvalue('message')
		Message.SendMessage(id, M_DATA, message)

		
	def MsgGet(self):
		m = Message.SendMessage(M_BROKER, M_GETDATA)
		if m.Header.Type == M_DATA:
			if (m.Header.From != M_STORAGE):
				self.msg = str(m.Header.From) + ": " + m.Data
			else:
				self.msg = m.Data

def main():
	q = cgi.FieldStorage()
	m = Messenger(q)
	if (q.getvalue('Rest') == '1'):
		if (q.getvalue('Post', '') != '0'):
			print("Content-type: application-json; charset=utf-8\n\n")
			r = {'messages': ''}
			print(json.dumps(r))
			m.MsgSend()	
		else:
			m.MsgGet()
			print("Content-type: application-json; charset=utf-8\n\n")
			r = {'messages': m.msg}
			print(json.dumps(r))
	else: 
		MENU = {
			'send':	m.MsgSend,
			'get':  m.MsgGet,
		}
		try:
			MENU[q.getvalue('type')]()
		except Exception as e:
			pass
		m.PrintPage()
        
main()
