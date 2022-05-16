#include "server.hpp"

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

string Server::split(string str, char del)
{
    string *strings = new string("");
    for (size_t i = 0;i<str.size();i++)
    {
        if (str[i] != del)
            strings[0] += str[i];
        else
            return strings[0];
    }
    return strings[0];
}

int Server::spaceCheck(string str)
{
    int k = 0;
    for (size_t i = 0;i<str.size();i++)
    {
        if (str[i] == ' ')
            k++;
    }
    return k;
}

int Server::channelNameCheck(string str)
{
    for (size_t l = 0; l < str.size(); l++)
    {
        if (str[l] == ' ' || str[l] == ',' || str[l] == '\r' || str[l] == '\n' || str[l] == '\0')
        {
            cout << "error" << endl;
            return (-1);
        }
    }
    return 0;
}

void Server::chanPassNum(string chanName, string passName, struct kevent &event)
{
    size_t j = 0;
    for (size_t i = 0; i < chanName.size(); i++)
    {
        if (chanName[i] == ',')
            j++;
    }
    size_t chan_number = j + 1;
    size_t k = 0;
    for (size_t i = 0; i < passName.size(); i++)
    {
        if (passName[i] == ',')
            k++;
    }
    size_t pass_number = k + 1;
    if (pass_number > chan_number)
    {
        sendAnswer(event, ":server 464 :Password incorrect\r\n");
        return ;
    }
}

string Server::addUserToChan(struct kevent &event, string ChanName)
{
    string userToChan = findNickByFd(event);
    for (vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
        if (iter2->name == ChanName)
            iter2->users.push_back(userToChan);
    return (userToChan);
}

void Server::userAlreadyInChan(string chanCheck, struct kevent &event)
{
    for(vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
    {
        if(iter2->name == chanCheck)//проверка, если уже есть такой канал
        {
            unsigned long p = 0;
            for (list<struct kevent>::iterator q = fds.begin(); q != fds.end();q++)//поиск текущего пользователя
            {
                if (q->ident == event.ident)
                    break;
                p++;
            }
            string userInChan = "";
            list<string>::iterator iter = users.begin();
            for (unsigned long r = 0;r <= p; iter++, r++)
            {
                if (r == p)
                    userInChan += *iter;
            }
            for(list<string>::iterator iter3 = iter2->users.begin(); iter3 != iter2->users.end(); iter3++)
            {//проверка, есть ли пользователь в списке данного канала
                if(*iter3 == userInChan)
                {
                    sendAnswer(event, ":server 443 "+chanCheck+" "+userInChan+" :is already on channel\r\n");
                    return ;
                }
                else
                    iter2->users.push_back(userInChan);
            }
        }
    }
}

string Server::findNickByFd(struct kevent &e)
{
    unsigned long a = 0;string nick = "";
    list<string>::iterator it = users.begin();
    for (list<struct kevent>::iterator i = fds.begin();i != fds.end();i++)
    {
        if (i->ident == e.ident)
            break;
        a++;
    }
    for (unsigned long k = 0;k<users.size();it++,k++)
    {
        string str = *it;
        // printf("str2: %s\n", str.c_str());
        if (k==a)
        {
            for (size_t n = 0;n<strlen(str.c_str());n++)
                nick += str[n];
        }    
    }
    return nick;
}

chatroom Server::findRoomByName(string Name)
{
    for (vector<chatroom>::iterator i = rooms.begin();i!=rooms.end();i++)
        if (i->name == Name)
            return *i;
    return *rooms.end();
}

struct kevent& Server::findFdByNick(string Nick)
{
    int j = 0;list<struct kevent>::iterator it2 = fds.begin();
    for (list<string>::iterator it = users.begin();it!=users.end();it++)
    {
        if (*it == Nick)
            break;
        j++;
    }
    for (int i = 0;i<=j;i++,it2++)
    {
        if (i == j)
            return *it2;
    }
    return *it2;
}
int Server::checkUserNick(string &name)
{
    for (list<string>::iterator it = users.begin();it != users.end();it++)
        if (*it == name)
            return 0;
    return -1;
}