#include "server.hpp"
#include <ctime>
void Server::marussia(string Nick,string Message,struct kevent &event)
{
    cout << "marussia here!\n";
    string m = split(split(Message,'\n'),'\r');
    if (m == ":users")
    {
        string nicks = "";
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :users on server: \r\n");
        for (list<string>::iterator i = users.begin();i!=users.end();i++)
            nicks += *i + "; ";
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :"+nicks+"\r\n");
    }
    else if (m[1] == 'm'&&m[2] == 's'&&m[3] == 'g')
    {
        string m2 = m.substr(m.find_first_of(':') + 1);
        m2 = m2.substr(m2.find_first_of(':') + 1);
        cout << m2 << endl;
        string user = m2.substr(m2.find_first_of(':') + 1);
        cout << user << endl;
        m2 = split(m2,':');
        if (checkUserNick(user) == -1)
        {
            sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :Нет такого юзера!\r\n");
            return;
        }
        struct kevent e = findFdByNick(user);
        sendAnswer(e, ":marussia! PRIVMSG "+user+" :Вам кто-то написал: "+m2+"\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :Успешно!\r\n");
    }
    else if (m[2] == 'a'&&m[3] == 'l'&&m[4] == 'l'&&m[1] == '!')
    {
        string m2 = m.substr(m.find_first_of(':') + 1);
        m2 = m2.substr(m2.find_first_of(':') + 1);
        list<string>::iterator it = users.begin();
        for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++,it++)
            if (i->ident != event.ident)
                sendAnswer(*i, ":"+Nick+"! PRIVMSG "+*it+" :"+m2+"\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :Успешно!\r\n");
    }
    else if (m[2] == 'a'&&m[3] == 'l'&&m[4] == 'l'&&m[1] == '@')
    {
        string m2 = m.substr(m.find_first_of(':') + 1);
        m2 = m2.substr(m2.find_first_of(':') + 1);
        list<string>::iterator it = users.begin();
        for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++,it++)
            if (i->ident != event.ident)
                sendAnswer(*i, ":marussia! PRIVMSG "+*it+" :Вам кто-то написал: "+m2+"\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :Успешно!\r\n");
    }
    else if (m == ":groups")
    {
        string groups = "";
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :groups on server: \r\n");
        for (vector<chatroom>::iterator i = rooms.begin();i!=rooms.end();i++)
            groups += i->name + "; ";
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :"+groups+"\r\n");
    }
    else
    {
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :Привет! Я ваш личный ассистент на этом сервере!\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :Вот что я умею: \r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :При команде - 'users', вывожу список всех пользователей на сервере.\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :При команде - 'msg:сообщение:юзер', отправляю аннонимное сообщение указанному пользователю.\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :При команде - '!all:сообщение', отправляю сообщение от вашего имени всем пользователям.\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :При команде - '@all:сообщение', отправляю аннонимное сообщение всем пользователям.\r\n");
        sendAnswer(event, ":marussia! PRIVMSG "+Nick+" :При команде - 'groups', вывожу список всех групп на сервере.\r\n");
    }
}