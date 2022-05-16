#include "server.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <cstring>

void Server::cmdJOIN(string &str, struct kevent &event) //а если несколько имен каналов в одном сообщении? - сделано
{//добавить проверку, был ли этот канал создан ранее, если да - юзер просто юзер, если нет и это новый - юзер - оператор
// - будем просто проверять при применении команды оператора - юзер в листе по итератору 0 - и есть оператор
//добавить - у юзера не может быть больше 10 каналов
    if(str.size() < 8) //нужно было проверить - проверено
        sendAnswer(event, ":server 461 JOIN :Not enough parameters\r\n");
    else
    {
        string chanName = "";
        string chanNameLast = "";
        string passName = "";
        string passNameLast = "";
        string wherefind = str.substr((str.find("JOIN") + 4), (str.size() - 5));
        string userInChan = findNickByFd(event);
        if ((wherefind.find('#') == 1) || (wherefind.find('&') == 1))
        {
            wherefind = wherefind.substr(wherefind.find_first_of(' ') + 1);
            chanName = split(wherefind, ' ');
            if (spaceCheck(wherefind) > 2)
            {
                sendAnswer(event, ":server 403 "+chanName+ ":No such channel\r\n");
                return ;
            } 
            if (spaceCheck(wherefind) > 0 && spaceCheck(wherefind) < 2)
                passName = wherefind.substr(wherefind.find(' ') + 1);//исправлено - не работает корректно если ввести только одно слово(имя канала) 
            cout << wherefind << " - wherefind name" << endl;
            cout << chanName << " - channel name" << endl;
            cout << passName << " - pass name" << endl;
            chanPassNum(chanName, passName, event);//проверка чтобы паролей было не больше каналов
            size_t i = 0;
            while(i < chanName.size())//проверка имен каналов
            {
                if (chanName[i] == ',')
                {
                    string chanCheck = split(chanName, ',');
                    // userAlreadyInChan(chanCheck, event);
                    for(vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
                    {
                        if(iter2->name == chanCheck)//проверка, если уже есть такой канал
                        {
                            for(list<string>::iterator iter3 = iter2->users.begin(); iter3 != iter2->users.end(); iter3++)
                            {//проверка, есть ли пользователь в списке данного канала
                                if(*iter3 == userInChan)
                                {
                                    string nicks = rooms[rooms.size()-1].getUsers();
                                    sendAnswer(event, ":"+userInChan+"! JOIN :"+chanCheck+ "\r\n");
                                    sendAnswer(event, ":server 443 "+userInChan+" "+chanCheck+"  :is already on channel\r\n");
                                    sendAnswer(event, ":server 353 "+userInChan+" = "+chanCheck+ " :" + nicks+"\r\n");
                                    sendAnswer(event, ":server 366 "+userInChan+" "+chanCheck+ " :End of /NAMES list\r\n");
                                    cout << "11111" << endl;
                                    return ;
                                }
                                else
                                    iter2->users.push_back(userInChan);
                                struct kevent Fd = findFdByNick(*iter3);
                                if (Fd.ident != event.ident)
                                    sendAnswer(Fd, ":"+userInChan+"! JOIN :"+chanNameLast+ "\r\n");
                            }
                            cout << userInChan << "21" << endl;
                        }
                    }
                    if(chanCheck.size() > 200 || (chanCheck[0] != '#' && chanCheck[0] != '&') || channelNameCheck(chanCheck) < 0)
                    {//некорректно обрабатывает # & в начале - сделано
                        sendAnswer(event, ":server 403 "+chanCheck+ ":No such channel\r\n");
                        return ;
                    }
                    chatroom a(chanCheck, "");
                    rooms.push_back(a);
                    string userToChan = addUserToChan(event, chanCheck);
                    if(rooms[rooms.size()-1].topic == "")//дописать ф-ю вывода списка юзеров в данном канале
                    {
                        string nicks = rooms[rooms.size()-1].getUsers();
                        rooms[rooms.size()-1].printUsers();
                        sendAnswer(event, ":"+userToChan+"! JOIN :"+chanCheck+ "\r\n");
                        sendAnswer(event, ":server 331 "+userToChan+" "+chanCheck+ " :No topic is set\r\n");
                        sendAnswer(event, ":server 353 "+userToChan+" = "+chanCheck+ " :" + nicks+"\r\n");
                        sendAnswer(event, ":server 366 "+userToChan+" "+chanCheck+ " :End of /NAMES list\r\n");
                    }
                    else
                    {
                        string nicks = rooms[rooms.size()-1].getUsers();
                        rooms[rooms.size()-1].printUsers();
                        sendAnswer(event, ":"+userToChan+"! JOIN :"+chanCheck+ "\r\n");
                        sendAnswer(event, ":server 332 "+userToChan+" "+chanCheck+" :"+rooms[rooms.size()-1].topic+ "\r\n");
                        sendAnswer(event, ":server 353 "+userToChan+" = "+chanCheck+ " :" + nicks+"\r\n");
                        sendAnswer(event, ":server 366 "+userToChan+" "+chanCheck+ " :End of /NAMES list\r\n");
                    }
                    chanName = chanName.substr(chanName.find(','));
                    i = 0;
                }   
                i++;    
            }
            chanNameLast = chanName.substr(chanName.find(',') + 1);//проверка и добавление последнего имени канала
            chanNameLast = split(chanNameLast, '\r');
            for(vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
            {
                if(iter2->name == chanNameLast)//проверка, если уже есть такой канал
                {
                    for(list<string>::iterator iter3 = iter2->users.begin(); iter3 != iter2->users.end(); iter3++)
                    {//проверка, есть ли пользователь в списке данного канала
                        if(*iter3 == userInChan)
                        {
                            string nicks = iter2->getUsers();
                            sendAnswer(event, ":"+userInChan+"! JOIN :"+chanNameLast+ "\r\n");
                            sendAnswer(event, ":server 443 "+userInChan+" "+chanNameLast+"  :is already on channel\r\n");
                            sendAnswer(event, ":server 353 "+userInChan+" = "+chanNameLast+ " :" + nicks+"\r\n");
                            sendAnswer(event, ":server 366 "+userInChan+" "+chanNameLast+ " :End of /NAMES list\r\n");
                            cout << "22222" << endl;
                            return ;
                        }
                        else
                            iter2->users.push_back(userInChan);
                        struct kevent Fd = findFdByNick(*iter3);
                        if (Fd.ident != event.ident)
                            sendAnswer(Fd, ":"+userInChan+"! JOIN :"+chanNameLast+ "\r\n");
                    }
                }
            }
            if(chanNameLast.size() > 200 || (chanNameLast[0] != '#' && chanNameLast[0] != '&') || channelNameCheck(chanNameLast) < 0)
            {
                sendAnswer(event, ":server 403 "+chanNameLast+ " :No such channel\r\n");
                return ;
            }
            chatroom b(chanNameLast, "");
            rooms.push_back(b);
            string userToChan = addUserToChan(event, chanNameLast);
            if(rooms[rooms.size()-1].topic == "")
            {
                string nicks = rooms[rooms.size()-1].getUsers();
                rooms[rooms.size()-1].printUsers();
                sendAnswer(event, ":"+userToChan+"! JOIN :"+chanNameLast+ "\r\n");
                sendAnswer(event, ":server 331 "+userToChan+" "+chanNameLast+ " :No topic is set\r\n");
                sendAnswer(event, ":server 353 "+userToChan+" = "+chanNameLast+ " :" + nicks+"\r\n");
                sendAnswer(event, ":server 366 "+userToChan+" "+chanNameLast+ " :End of /NAMES list\r\n");
            }
            else
            {
                string nicks = rooms[rooms.size()-1].getUsers();
                rooms[rooms.size()-1].printUsers();
                sendAnswer(event, ":"+userToChan+"! JOIN :"+chanNameLast+ "\r\n");
                sendAnswer(event, ":server 332 "+userToChan+" "+chanNameLast+" :"+rooms[rooms.size()-1].topic+ "\r\n");//что тут тоже надо было проверить??
                sendAnswer(event, ":server 353 "+userToChan+" = "+chanNameLast+ " :" + nicks+"\r\n");
                sendAnswer(event, ":server 366 "+userToChan+" "+chanNameLast+ " :End of /NAMES list\r\n");
            }
            size_t l = 0;//добавить добавление паролей - добавлено
            size_t m = 0;
            while (l < passName.size())//проверка и добавление паролей
            {
                if (passName[l] == ',')
                {
                    string passCheck = split(passName, ',');
                    rooms[m].pass = passCheck;
                    m++;
                    passName = passName.substr(passName.find(','));
                    l = 0;
                }
                l++;
            }
            passNameLast = passName.substr(passName.find(',') + 1);//проверка и добавление последнего пароля
            rooms[m].pass = passNameLast;
            for(vector<chatroom>::iterator it = rooms.begin(); it !=rooms.end(); it++)
            {
                it->printName();
                it->printPass();
                for(list<string>::iterator iterat = it->users.begin(); iterat != it->users.end(); iterat++)
                    cout << *iterat << endl;
                cout << "1" << endl;
            }   
        }
        else
           sendAnswer(event, ":server 403 "+userInChan+" :No such channel\r\n");
    }
}

int Server::cmdNICK(string &str, int n, struct kevent &event)//доб. замену ника
{
    string nick(str.substr(n));
    string oldnick = "";
    nick = (nick.substr(nick.find_first_of(' ') + 1));
    size_t found = nick.find('\n');
    size_t f = str.find(':') +1;
    if (found != string::npos)
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
    if (checkClient(nick) == 0 && nick != "marussia")
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
    int space = str.find(' ');int m = str.find(':');
    string nick = "";string nick2 = findNickByFd(e);string message = "";
    list<struct kevent>::iterator it2 = fds.begin();
    for (int i = space + 1;str[i] != ' ';i++)
        nick += str[i];
    for (unsigned long i = m;i<str.size();i++)
        message+=str[i];
    int j = 0;struct kevent event;
    for (list<string>::iterator i = users.begin();i != users.end();i++)
    {
        string str = *i;size_t check = 0;
        if (strlen(str.c_str()) == strlen(nick.c_str()))
            for (size_t g=0;g<strlen(str.c_str());g++)
                if (str[g] == nick[g])
                    check++;
        if (check == strlen(str.c_str()))
            break;
        j++;
    }
    for (int k = 0;k<=j;it2++,k++)
    {
        if (k==j)
            event = *it2;
    }
    if (nick[0] == '#' || nick[0] == '&')
    {
        cout << "find#" << endl;
        chatroom obj = findRoomByName(nick);
        list<string> lst = obj.getListUsers();
        for (list<string>::iterator i = lst.begin();i != lst.end();i++)
        {
            string *str2 = new string(":"+nick2+"! PRIVMSG "+nick+" "+ message + "\r\n");
            struct kevent ev = findFdByNick(*i);
            cout << str2[0] << endl;
            if (nick2 != *i)
                sendAnswer(ev, str2[0]);
            delete str2;
        }    
    }
    else if (nick == "marussia")
        marussia(nick2,message,e);
    else if (nick2 == "")
        sendAnswer(e, "You are not registered!\n");
    else
    {
        string *str2 = new string(":"+nick2+"! PRIVMSG "+nick+" "+ message + "\r\n");
        sendAnswer(event, str2[0]);
    }
}
void Server::cmdNOTICE(string &str, struct kevent &e)
{
    int space = str.find(' ');int m = str.find(':');
    string nick = "";string nick2 = findNickByFd(e);string message = "";
    list<struct kevent>::iterator it2 = fds.begin();
    for (int i = space + 1;str[i] != ' ';i++)
        nick += str[i];
    for (unsigned long i = m;i<str.size();i++)
        message+=str[i];
    int j = 0;struct kevent event;
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
    for (int k = 0;k<=j;it2++,k++)
    {
        if (k==j)
            event = *it2;
    }
    string *str2 = new string(":"+nick2+"! NOTICE "+nick+" "+ message + "\n");
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
void Server::cmdWHOIS(string &str, struct kevent &e)
{
    string nick = str.substr(str.find_first_of(' ') + 1);
    string nick2 = "";size_t fd = 0;int check = 0;
    size_t found = nick.find('\n');
    size_t found2 = nick.find('\r');
    list<string>::iterator it = users.begin();
    if (found != string::npos)
        nick.erase(found);
    if (found2 != string::npos)
        nick.erase(found2);
    for (list<string>::iterator i2 = users.begin();i2!=users.end();i2++)
    {
        string n = *i2;
        if (n == nick)
            check = 1;
    }
    if (check == 0)
        return;
    for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++)
    {
        if (i->ident == e.ident)
            break;
        fd++;
    }
    for (size_t i = 0;i<=fd;i++,it++)
        if (i == fd)
            nick2 += *it;
    sendAnswer(e, ":server 311 " +nick2+" "+nick+" :Adium User\r\n");
    sendAnswer(e, ":server 319 " +nick2+" "+nick+" :\r\n");
    sendAnswer(e, ":server 312 " +nick2+" "+nick+" :server IRC\r\n");
    sendAnswer(e, ":server 317 " +nick2+" "+nick+" 1 1651936678 :seconds idle\r\n");
    sendAnswer(e, ":server 318 " +nick2+" "+nick+" :End of /WHOIS list\r\n");
}
void Server::cmdISON(string &str, struct kevent &e)
{
    string nick = str.substr(str.find_first_of(' ') + 1);
    size_t fd = 0;
    string nick2 = "";int check = 0;
    list<string>::iterator it = users.begin();
    for (unsigned long p = 0;p<nick.size();p++)
        if (nick[p] == '\n'|| nick[p] == '\r')
            nick.erase(p);
    // cout << nick << endl;
    for (list<string>::iterator i2 = users.begin();i2!=users.end();i2++)
    {
        string n = *i2;
        if (n == nick)
            check = 1;
    }
    for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++)
    {
        if (i->ident == e.ident)
            break;
        fd++;
    }
    if (fd <= users.size())
    {
        for (size_t i = 0;i<=fd;i++,it++)
            if (i == fd)
                nick2 += *it;
    }
    // cout << nick2 << endl;
    if (check == 1)
        sendAnswer(e, ":server 303 "+nick2+" :"+nick+"\r\n");
    else
        sendAnswer(e, ":server 303 "+nick2+" :\r\n");
}
void Server::prePRIVMSG(string &str, struct kevent &event)
{
    vector<string> nicks;int i = 0;string message = str.substr(str.find_first_of(':'));
    string buff = str.substr(str.find_first_of(' ') + 1);string buff2 = "";
    // cout << buff << endl;
    for (int j = 0;buff[j] != ':';j++)
    {
        if (buff[j] == ','|| buff[j] == ' ')
        {
            nicks.push_back(buff2);
            buff2 = "";
            i++;
        }
        else
            buff2 += buff[j];
        
    }
    // cout << nicks.size() << endl;
    for (int d = 0;d<=i;d++)
    {
        string a = "PRIVMSG "+nicks[d]+" "+message;
        cmdPRIVMSG(a, event);
    }
}

void Server::nickAnswer(int res,struct kevent &event)
{
    if (res == 0)
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

void Server::cmdLIST()
{
    printf("users: %lu\n", users.size());
    printf("fds: %lu\n", fds.size());
    for (list<string>::iterator i = users.begin(); i!=users.end();i++)
        printf("%s: %lu\n", i->c_str(), strlen(i->c_str()));
    for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++)
        printf("%lu\n", i->ident);
    for (list<struct kevent>::iterator i = auth.begin();i!=auth.end();i++)
        printf("auth: %lu\n", i->ident);
    for (vector<chatroom>::iterator it = rooms.begin();it != rooms.end();it++)
    {
        list<string> nicks = it->getListUsers();
        cout << "-------------------------------\n";
        for (list<string>::iterator i = nicks.begin();i!=nicks.end();i++)
            cout << *i << endl;
        printf("room name: %s\n", it->name.c_str());
        cout << "-------------------------------\n";
    }
}
int Server::parsBuffer(string &str, struct kevent &event)
{
    int in = str.find("NICK");
    int ret = 0;int check = 0;
    if (str.find("PASS") != string::npos)
    {
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
            sendAnswer(event, ":server 451 :You have not registered\r\n");
            onClientDisconnect(event);
            return ret;
        }
    }
    if (ret == 1)
        return ret;
    if (str.find("NICK") != string::npos)
    {
        ret = cmdNICK(str, in, event);
        nickAnswer(ret,event);
    }
    if (Find(str, "PING") == 0)
        sendAnswer(event,"PONG 10.21.32.116");
    if (Find(str, "QUIT") == 0)
        cmdQUIT(event);
    if (Find(str,"PRIVMSG") == 0)
        prePRIVMSG(str,event);
    if (Find(str,"NOTICE") == 0)
        cmdNOTICE(str,event);
    if (Find(str, "ISON") == 0)
        cmdISON(str,event);
    if (Find(str,"WHOIS") == 0)
        cmdWHOIS(str,event);
    if (Find(str,"LIST") == 0)
        cmdLIST();
    if (Find(str, "JOIN") == 0)
        cmdJOIN(str, event);
    return ret;
}