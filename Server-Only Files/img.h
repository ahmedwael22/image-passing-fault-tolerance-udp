#ifndef IMG_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;


#define IMG_H

struct viewing_user
{
	string username;
	int num_of_views;
};


class Img
{
    private:
        string image_name;
        string username;
        string password;
        vector <viewing_user> access_list;


    public:
        Img(string img_name, string un, string pwd);
        void append_viewing_user(string un, int v);
        string get_image_name();
        string get_username();
        string get_password();
        string get_access_user(int j);
        int get_access_view(int j);
        int get_access_list_size();
        int allowed_to_view(string un);
        void print_info();
        void print_access_list();
};
#endif // IMG_H


Img::Img(string img_name, string un, string pwd)
{
	image_name = img_name;
	username = un;
	password = pwd;
}

void Img::append_viewing_user(string un, int v)
{
    viewing_user* temp = new viewing_user;
    temp->username=un;
    temp->num_of_views = v;
    access_list.push_back(*temp);
    delete temp;
}

string Img::get_image_name()
{
    return image_name;
}

string Img::get_username()
{
    return username;
}

string Img::get_password()
{
    return password;
}

string Img::get_access_user(int j)
{
    return (access_list[j].username);
}

int Img::get_access_view(int j)
{
    return (access_list[j].num_of_views);
}

int Img::get_access_list_size()
{
    return access_list.size();
}


/*
 1  : success
-4  : user not authorized for image
-5  : user expired his views
*/
int Img::allowed_to_view(string un)
{
    for (int i=0; i<access_list.size(); i++)
    {
        if (access_list[i].username==un)
        {
            if (access_list[i].num_of_views > 0)
            {
                access_list[i].num_of_views--; //decrement
                return 1;
            }
            else
            {
                return -5;
            }
        }
    }
    return -4;
}

void Img::print_info()
{
    cout << image_name << " " << username << " " << password;
    cout << endl;
    print_access_list();
}

void Img::print_access_list()
{
    for (int i=0; i<access_list.size(); i++)
        cout << access_list[i].username << " " << access_list[i].num_of_views << endl;
    cout << endl;
}