// MsgServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "MsgServer.h"
#include "Msg.h"
#include "Session.h"
#include <Windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int gMaxID = M_USER;
map<int, shared_ptr<Session>> gSessions;
unsigned int WebId = 20;


void ProcessClient(SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);
    Message m, vrem;
    string msgStorage;

    int nCode = m.Receive(s);
        switch (nCode)
        {
        case M_INIT:
        {
            if (m.m_Data == "server")
            {
                auto pSession = make_shared<Session>(M_STORAGE, m.m_Data);
                gSessions[pSession->m_ID] = pSession;
                Message::Send(s, pSession->m_ID, M_BROKER, M_INIT);
                cout << "storage on ID: 50" << endl;
            }
            else
            {
                auto pSession = make_shared<Session>(++gMaxID, m.m_Data);
                gSessions[pSession->m_ID] = pSession;
                Message::Send(s, pSession->m_ID, M_BROKER, M_INIT);
                cout << pSession->m_ID << " Hi " << m.m_Data << endl;

                if (m.m_Data == "WebClient")
                {
                    WebId = pSession->m_ID;
                    m.m_Header = {M_STORAGE, WebId, M_CONNECT_TO_STORAGE};
                    gSessions[M_STORAGE]->Add(m);

                }
            }

            break;
        }
        case M_EXIT:
        {
            cout << m.m_Header.m_From << " Bye" << endl;
            gSessions.erase(m.m_Header.m_From);
            Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            return;
        }
        case M_GETDATA:
        {
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                gSessions[m.m_Header.m_From]->m_Time = time(0);
                gSessions[m.m_Header.m_From]->Send(s);
            }
            break;
        }
        case M_ALLUSER:
        {
            string all;
            for (auto& [id, Session] : gSessions) 
            {
                all.push_back(to_string(Session->m_ID)[0]);
                all.push_back(to_string(Session->m_ID)[1]);
                all.push_back(to_string(Session->m_ID)[2]);
                all.push_back(' ');
            }
            Message::Send(s, m.m_Header.m_From, M_BROKER, M_ALLUSER, all);
            break;
        }
        default:
        {
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                gSessions[m.m_Header.m_From]->m_Time = time(0);
                if (m.m_Header.m_From == M_STORAGE && m.m_Header.m_To == WebId)
                {
                    gSessions[WebId]->Add(m);
                }
                if (gSessions.find(m.m_Header.m_To) != gSessions.end())
                {
                    if (m.m_Header.m_To == WebId)
                    {
                        if (m.m_Header.m_From != M_STORAGE)
                        {
                            gSessions[M_STORAGE]->Add(m);
                        }
                    }
                    else
                    {
                        if (m.m_Header.m_From == WebId)
                        {
                            gSessions[M_STORAGE]->Add(m);
                        }
                        gSessions[m.m_Header.m_To]->Add(m);
                    }
                }
                else if (m.m_Header.m_To == M_ALL)
                {
                    for (auto& [id, Session] : gSessions)
                    {
                        if (!(id == m.m_Header.m_From || id == WebId))
                        {
                            Session->Add(m);
                        }

                    }
                }
                Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            }
            break;
        }
        }
}

void Timer()
{
    while (true) {
        Sleep(1000);
        for (auto& [id, Session] : gSessions) {
            time_t tt= time(0) - Session->m_Time;
            if (tt > 1000) {
                cout << id << " time" << endl;
                gSessions.erase(id);
                break;
            }
        }
    }
}


void Server()
{
    AfxSocketInit();

    cout << "Сервер запущен!" << endl;

    CSocket Server;
    Server.Create(12345);
    thread t1(Timer);
    t1.detach();

    while (true)
    {
        if (!Server.Listen())
            break;
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
    }
}


// The one and only application object

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            Server();
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
