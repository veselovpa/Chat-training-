#pragma once

enum Messages
{
	M_INIT,//0
	M_EXIT,//1
	M_GETDATA,//2
	M_NODATA,//3
	M_DATA,//4
	M_CONFIRM,//5
	M_ALLUSER,//6
	M_CONNECT_TO_STORAGE,//7

};

enum Members
{
	M_BROKER = 0,
	M_ALL = 10,
	M_STORAGE = 50,
	M_USER = 100
};

struct MsgHeader
{
	unsigned int m_To;
	unsigned int m_From;
	unsigned int m_Type;
	unsigned int m_Size;
};

struct Message
{
	MsgHeader m_Header;
	string m_Data;
	static int m_ClientID;

	Message()
	{
		m_Header = { 0 };
	}

	Message(unsigned int To, unsigned int From, unsigned int Type = M_DATA, const string& Data = "")
		:m_Data(Data)
	{
		m_Header = { static_cast<unsigned int>(To), From, Type, unsigned int(Data.length()) };
	}

	void Send(CSocket& s)
	{
		s.Send(&m_Header, sizeof(MsgHeader));
		if (m_Header.m_Size)
		{
			s.Send(m_Data.c_str(), m_Header.m_Size);
		}
	}

	int Receive(CSocket& s)
	{
		if (!s.Receive(&m_Header, sizeof(MsgHeader)))
		{
			return M_NODATA;
		}
		if (m_Header.m_Size)
		{
			vector <char> v(m_Header.m_Size);
			s.Receive(&v[0], m_Header.m_Size);
			m_Data = string(&v[0], m_Header.m_Size);
		}
		return m_Header.m_Type;
	}

	static void Send(CSocket& s, unsigned int To, unsigned int From, unsigned int Type = M_DATA, const string& Data = "");
	static Message Send(unsigned int To, unsigned int Type = M_DATA, const string& Data = "");
};


