#include "server.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <cstring>
int Server::Find(string str)
{
    // printf("len: %lu\n", strlen(str[0].c_str()));
    size_t ravno = 0;
    for (list<string>::iterator i = users.begin();i!=users.end();i++)
    {
        if (strlen(i->c_str()) >= strlen(str.c_str()))
        {
            for (size_t j =0;j<strlen(str.c_str());j++)
            {
                string test = *i;
                if (test[j] == str[j])
                    ravno++;
            }
            if (ravno == strlen(str.c_str()) && strlen(i->c_str()) == strlen(str.c_str()))
                return 1;
            ravno = 0;
        }
    }
    // printf("ravno: %d\n", ravno);
    return 0;
}

int Server::Find(string &str, string str2)
{
    int res = -1;int ravno = 0;
    int len1 = strlen(str.c_str());
    int len2 = strlen(str2.c_str());
    if (len1 >= len2)
    {
        for (int i = 0;i<len2;i++)
        {
            if (str[i] == str2[i])
                ravno++;
            else
                ravno = 0;
        }
        if (ravno == len2)
        {
            res = 0;
            return res;
        }
    }
    return res;
}

int Server::checkClient(string str)
{
    int k = Find(str);
    if (k == 1)
    {
        printf("<%s> :Nickname is already in use\n",str.c_str());//436???
        return 1;
    }
    else
        return 0;
}

int Server::cmdNICK(string &str, int n, struct kevent &event)//доб. замену ника
{
    string nick(str.substr(n));
    string oldnick = "";
    nick = (nick.substr(nick.find_first_of(' ') + 1));
    size_t found = nick.find('\n');
    size_t f = str.find(':') +1;
    if (found > 0)
    {
        for (unsigned long p = 0;p<nick.size();p++)
            if (nick[p] == '\n'|| nick[p] == '\r')
                nick.erase(p);
    }
    if (f != string::npos && f < (size_t)n && str.find("USER") == string::npos)
    {
        for (size_t i =f;str[i] != ' ';i++)
            oldnick += str[i];
        printf("oldnick: %s\n", oldnick.c_str());
        for (list<string>::iterator i = users.begin();i != users.end();i++)
        {
            string buff = *i;
            if (buff == oldnick)
            {
                i->erase(0);
                *i = nick;
                printf("i: %s\n", i->c_str());
            }
        }
        return 0;
    }
    // if (nick.length() < 1)
    //     ERR(":No nickname given", strerror(errno));//431???
    if (checkClient(nick) == 0)
    {
        printf("check: %s\n", nick.c_str());
        users.push_back(nick);
        fds.push_back(event);
        return 0;
    }
    else
        return 1;
}
void Server::cmdQUIT(struct kevent &event)
{
    list<string>::iterator it2 = users.begin();
    for (list<struct kevent>::iterator it = fds.begin();it!=fds.end() || it2!=users.end();it++, it2++)
    {
        if (it->ident == event.ident)
        {
            if (users.size() == 1)
            {
                users.clear();
                fds.clear();
                break;
            }
            else
                users.erase(it2);
            if (fds.size() == 1)
            {
                users.clear();
                fds.clear();
                break;
            }
            else
                fds.erase(it);
            it2 = users.begin();
            it = fds.begin();
        }
    }
    for (list<struct kevent>::iterator i = auth.begin();i != auth.end();i++)
    {
        if (i->ident == event.ident)
        {
            if (auth.size() == 1)
            {
                auth.clear();
                break;
            }
            else
                auth.erase(i);
            i = auth.begin();
        }
    }
    onClientDisconnect(event);
}
void Server::cmdPRIVMSG(string &str, struct kevent &e)
{
    int space = str.find(' ');
    int m = str.find(':');
    string nick = "";
    string nick2 = "";
    string message = "";
    list<struct kevent>::iterator it2 = fds.begin();
    list<string>::iterator it = users.begin();
    for (int i = space + 1;str[i] != ' ';i++)
    {
        nick += str[i];
    }
    for (unsigned long i = m;i<str.size();i++)
        message+=str[i];
    int j = 0;
    for (list<string>::iterator i = users.begin();i != users.end();i++)
    {
        string str = *i;
        size_t check = 0;
        if (strlen(str.c_str()) == strlen(nick.c_str()))
            for (size_t g=0;g<strlen(str.c_str());g++)
                if (str[g] == nick[g])
                    check++;
        if (check == strlen(str.c_str()))
            break;
        j++;
    }
    struct kevent event;
    for (int k = 0;k<=j;it2++,k++)
    {
        if (k==j)
            event = *it2;
    }
    unsigned long a = 0;
    for (list<struct kevent>::iterator i = fds.begin();i != fds.end();i++)
    {
        if (i->ident == e.ident)
            break;
        a++;
    }
    for (unsigned long k = 0;k<users.size();it++,k++)
    {
        string str = *it;
        printf("str2: %s\n", str.c_str());
        if (k==a)
        {
            for (size_t n = 0;n<strlen(str.c_str());n++)
                nick2 += str[n];
        }    
    }
    string *str2 = new string(":"+nick2+"! PRIVMSG "+nick+" "+ message + "\r\n");
    sendAnswer(event, str2[0]);
}
int Server::cmdPASS(string &str, struct kevent &e)
{
    string pass = str.substr(str.find_first_of(':') + 1);
    size_t found = pass.find('\n');
    size_t found2 = pass.find('\r');
    if (found != string::npos)
        pass.erase(found);
    if (found2 != string::npos)
        pass.erase(found2);
    printf("pass: %s\n", pass.c_str());
    printf("pass len: %lu\n", strlen(pass.c_str()));
    if (pass == serverpassword)
        return 0;
    else
    {
        // sendAnswer(e, ERROR);
        onClientDisconnect(e);
        return 1;
    }
}
int Server::parsBuffer(string &str, struct kevent &event)
{
    int in = str.find("NICK");
    int ret = 0;int check = 0;
    if (str.find("PASS") != string::npos)
    {
        printf("ret: %d\n", ret);
        ret = cmdPASS(str, event);
        if (ret == 0)
            auth.push_back(event);
        printf("ret: %d\n", ret);
    }
    else
    {
        for (list<struct kevent>::iterator i = auth.begin();i != auth.end();i++)
            if (i->ident == event.ident)
                check = 1;
        if (check == 0)
        {
            onClientDisconnect(event);
            return ret;
        }
    }
    if (ret == 1)
        return ret;
    // if ((str.find("USER") != string::npos) && (str.find("PASS") == string::npos))
    // {
    //     onClientDisconnect(event);
    //     return 1;
    // }
    if (str.find("NICK") != string::npos)
    {
        ret = cmdNICK(str, in, event);
        if (ret == 0)
        {	
            string name = "";
            for (list<string>::iterator i = users.begin();i!=users.end();i++)
                name = i->c_str();
            sendAnswer(event, SUCCESSCONNECT + name +"\r\n");
            cout << "-------------------------------\n";
            cout << "users online: " << users.size() << endl;
            for (list<string>::iterator i = users.begin();i!= users.end();i++)
                printf("%s\n", i->c_str());
            cout << "-------------------------------\n";
        }
        else
        {
            sendAnswer(event, ERROR);
            sendAnswer(event, ":server 451 :You have not registered\r\n");
            onClientDisconnect(event);
        }
    }
    if (Find(str, "PING") == 0)
        sendAnswer(event,"PONG 10.21.32.116");
    if (Find(str, "QUIT") == 0)
        cmdQUIT(event);
    if (Find(str,"PRIVMSG") == 0)
        cmdPRIVMSG(str, event);
    if (Find(str,"LIST") == 0)
    {
        printf("users: %lu\n", users.size());
        printf("fds: %lu\n", fds.size());
        for (list<string>::iterator i = users.begin(); i!=users.end();i++)
            printf("%s: %lu\n", i->c_str(), strlen(i->c_str()));
        for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++)
            printf("%lu\n", i->ident);
        for (list<struct kevent>::iterator i = auth.begin();i!=auth.end();i++)
            printf("auth: %lu\n", i->ident);
    }
    return ret;
}