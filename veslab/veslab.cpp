// veslab.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "veslab.h"
#include "../MsgServer/Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CheckMessages()
{
    while (1) {
        Sleep(1000);
        Message msg=Message::Send(M_BROKER, M_GETDATA);
        if (msg.m_Header.m_Type == M_DATA)
            cout << endl << msg.m_Header.m_From << ": " << msg.m_Data << endl;
    }
}

//void start()

// Единственный объект приложения

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            AfxSocketInit();
            int id1;
            Message msg = Message::Send( M_BROKER, M_INIT, "C++");
            thread tt(CheckMessages);
            tt.detach();
            cout << msg.m_Header.m_To << endl;
            while (1) {
                cout << "Send - 0; Quit - 1: ";
                bool check;
                cin >> check;
                switch (check)
                {
                case 0:
                {
                    cout << "To: ";
                    cin >> msg.m_Header.m_To;
                    cin.ignore(50, '\n');
                    cout << "Text: ";
                    getline(cin, msg.m_Data, '\n');
                    Message::Send( msg.m_Header.m_To, M_DATA, msg.m_Data);
                    break;
                }
                case 1:
                {
                    Message::Send( M_BROKER,  M_EXIT);
                    return 0;
                }
                }
            }
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
