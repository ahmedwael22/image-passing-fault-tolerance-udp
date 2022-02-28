#ifndef USER_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;


#define USER_H

class User
{
    private:
        string username;
        string password;
    public:
        User(string un, string pwd);
        string get_username();
        string get_password();
        bool correct_username(string un);
        bool correct_password(string pwd);
        void print_user();
};

#endif //USER_H

User::User(string un, string pwd)
{
    username = un;
    password = pwd;
}

string User::get_username()
{
    return username;
}

string User::get_password()
{
    return password;
}

bool User::correct_username(string un)
{
    return (username == un);
}

bool User::correct_password(string pwd)
{
    return (password == pwd);
}

void User::print_user()
{
    cout << username << " " << password << endl;
}