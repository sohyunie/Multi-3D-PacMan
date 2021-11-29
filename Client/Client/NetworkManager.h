#pragma once
#include "Standard.h"

#define SERVERIP   "127.0.0.1"
#define BUFSIZE 2048
#define BUFSIZE2 128

class NetworkManager
{
private:
    static NetworkManager* instance;

    Socket s_socket;
    Message m_sendMsg;
    
    bool isConnected = false;

    MsgType msgtype;

    char myID;

    mutex inputLock;
    char last_input;

public:
    static NetworkManager& GetInstance() {
        if (instance == NULL) {
            instance = new NetworkManager();
        }
        return *instance;
    }

    void Network();
    void Update();
    bool GetIsConnected();
    int GetMyID();
    void SendPlayerInput();

    void SetLastInput(char input);
};