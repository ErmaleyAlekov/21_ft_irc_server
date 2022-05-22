#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <fcntl.h>
#include <fstream>
#include "log.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <signal.h>
#include <limits.h>
using namespace std;
#define ERROR ":server 433 Nickname is already in use\r\n"
#define SUCCESSCONNECT ":server 376 "

class chatroom
{
  public:
    list<string> users;
    list<uintptr_t> Fds;
    string pass;
    string name;
    string topic;
    chatroom(string Name,string Pass) {name = Name;pass = Pass;topic = "";}
    ~chatroom() {}
    void addUser(string nik) {users.push_back(nik);}
    void printUsers()
    {
      for (list<string>::iterator it = users.begin();it != users.end();it++)
        cout<< *it<< endl;
    }
    string getUsers()
    {
      string res = "";
      list<string>::iterator it = users.begin();
      for (size_t i =0;i <users.size();i++,it++)
        res += *it + " ";
      return res;
    }
    list<string> getListUsers() {return users;}
    void printName()
    {
      cout<< name << endl;
    }
    void printPass()
    {
      cout<< pass << endl;
    }
    void deleteUser(string nik) 
    {
      for (list<string>::iterator it = users.begin();it != users.end();it++)
      {
        string n = *it;
        if (n == nik)
          users.erase(it);
      }
    }
};
class Server
{
  public:
    Server(const char *addr, const int port,string pass, int backlog);
    ~Server();
    int onRead(uintptr_t& event);
    void onEOF();
    int onClientConnect(uintptr_t& event);
    int onClientDisconnect(uintptr_t& event);
    void startServer();
    int parsBuffer(string &str, uintptr_t &event);
    int cmdNICK(string &str, int n, uintptr_t &event);
    int checkClient(string str);
    int Find(string str);
    int Find(string &str, string str2);
    ssize_t sendAnswer(uintptr_t &event, string str);
    void cmdQUIT(uintptr_t &event);
    void cmdPRIVMSG(string &str, uintptr_t &e);
    int cmdPASS(string &str, uintptr_t &e);
    void cmdWHOIS(string &str, uintptr_t &e);
    void cmdISON(string &str, uintptr_t &e);
    void cmdNOTICE(string &str, uintptr_t &e);
    void signals();
    void checkConnects();
    void prePRIVMSG(string &str, uintptr_t &event);
    void nickAnswer(int res,uintptr_t &event);
    void cmdLIST();
    string split(string str, char del);
    void cmdJOIN(string &str, uintptr_t &event);
    int channelNameCheck(string str);
    int spaceCheck(string str);
    void chanPassNum(string chanName, string passName, uintptr_t &event);
    string addUserToChan(uintptr_t &event, string ChanName);
    void userAlreadyInChan(string chanCheck, uintptr_t &event);
    string *findNickByFd(uintptr_t &event);
    chatroom findRoomByName(string Name);
    uintptr_t& findFdByNick(string Nick);
    void marussia(string Nick,string Message,uintptr_t &event);
    int checkUserNick(string &name);
    void cmdPART(string &str, uintptr_t &event);
    vector<string> split2(string &str);
    void cmdKICK(string &str, uintptr_t &event);
    int checkRoomExist(string Name);
    void kickUserByNick(string chanName,string Nick);
    int checkFdInRoom(chatroom &obj,uintptr_t &event);
  private:
    int listen();
    int bind();
    int shutdown();
    int close();
    int initServer();
    struct sockaddr_in m_address;
    int m_sock_reuse;
    uintptr_t m_sock;
    int m_backlog;
    int m_kqueue;
    struct kevent m_event_subs;
    struct kevent m_event_list[USHRT_MAX];
    char m_receive_buf[USHRT_MAX*3];
    list<string> users;
    list<uintptr_t> fds;
    list<uintptr_t> auth;
    vector<chatroom> rooms;
    string sockstr;
    string serverpassword;
    enum SocketState 
    {
      INITIALIZED,
      BOUND,
      LISTENING,
      CLOSED
    };
    SocketState m_sock_state;
};
#endif