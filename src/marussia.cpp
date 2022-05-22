#include "server.hpp"
#include <ctime>
void Server::marussia(string Nick,string Message,struct kevent &event)
{
    cout << "marussia here!\n";
    string *m = new string("");
    m[0] = split(split(Message,'\n'),'\r');
    if (m[0] == ":users")
    {
        string *nicks = new string("");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :users on server: \r\n");
        for (list<string>::iterator i = users.begin();i!=users.end();i++)
            nicks[0] += *i + "; ";
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :"+nicks[0]+"\r\n");
    }
    else if (m[0][1] == 'm'&&m[0][2] == 's'&&m[0][3] == 'g')
    {
        string *m2 = new string(""); 
        m2[0] = m[0].substr(m[0].find_first_of(':') + 1);
        m2[0] = m2[0].substr(m2[0].find_first_of(':') + 1);
        cout << m2[0] << endl;
        string *user = new string("");
        user[0] = m2[0].substr(m2[0].find_first_of(':') + 1);
        cout << user[0] << endl;
        m2[0] = split(m2[0],':');
        if (checkUserNick(user[0]) == -1)
        {
            sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :Нет такого юзера!\r\n");
            return;
        }
        uintptr_t e = findFdByNick(user[0]);
        sendAnswer(e, ":marussia! PRIVMSG "+user[0]+" :Вам кто-то написал: "+m2[0]+"\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :Успешно!\r\n");
    }
    else if (m[0][2] == 'a'&&m[0][3] == 'l'&&m[0][4] == 'l'&&m[0][1] == '!')
    {
        string *m2 = new string("");
        m2[0] = m[0].substr(m[0].find_first_of(':') + 1);
        m2[0] = m2[0].substr(m2[0].find_first_of(':') + 1);
        list<string>::iterator it = users.begin();
        for (list<uintptr_t>::iterator i = fds.begin();i!=fds.end();i++,it++)
            if (*i != event.ident)
                sendAnswer(*i, ":"+Nick+"! PRIVMSG "+*it+" :"+m2[0]+"\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :Успешно!\r\n");
    }
    else if (m[0][2] == 'a'&&m[0][3] == 'l'&&m[0][4] == 'l'&&m[0][1] == '@')
    {
        string *m2 = new string("");
        m2[0] = m[0].substr(m[0].find_first_of(':') + 1);
        m2[0] = m2[0].substr(m2[0].find_first_of(':') + 1);
        list<string>::iterator it = users.begin();
        for (list<uintptr_t>::iterator i = fds.begin();i!=fds.end();i++,it++)
            if (*i != event.ident)
                sendAnswer(*i, ":marussia! PRIVMSG "+*it+" :Вам кто-то написал: "+m2[0]+"\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :Успешно!\r\n");
    }
    else if (m[0] == ":groups")
    {
        string *groups = new string("");
        groups[0] = "";
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :groups on server: \r\n");
        for (vector<chatroom>::iterator i = rooms.begin();i!=rooms.end();i++)
            groups[0] += i->name + "; ";
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :"+groups[0]+"\r\n");
    }
    else
    {
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :Привет! Я ваш личный ассистент на этом сервере!\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :Вот что я умею: \r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :При команде - 'users', вывожу список всех пользователей на сервере.\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :При команде - 'msg:сообщение:юзер', отправляю аннонимное сообщение указанному пользователю.\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :При команде - '!all:сообщение', отправляю сообщение от вашего имени всем пользователям.\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :При команде - '@all:сообщение', отправляю аннонимное сообщение всем пользователям.\r\n");
        sendAnswer(event.ident, ":marussia! PRIVMSG "+Nick+" :При команде - 'groups', вывожу список всех групп на сервере.\r\n");
    }
}