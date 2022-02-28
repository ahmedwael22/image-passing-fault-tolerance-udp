#include "udpserversocket.h"
#include "message.h"
#include "img.h"
#include "user.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define MAX_BUFFER_LEN 65000
#define IMG_BUFFER_LEN 64000
#define NUM_FRAGMENTS_MAX 50

#ifndef SERVER_H
#define SERVER_H


class Server
{
private:
    UDPServerSocket *udpServerSocket;

    char frags_array[NUM_FRAGMENTS_MAX][IMG_BUFFER_LEN];
    int total_num_frags;

    int last_rpcid;

    vector<User> users;
    vector<Img> images;

    bool populate_users(string filename);
    bool populate_imgs(string filename);
    int user_on_system(string username, string password);
    int user_allowed_to_view(string imgname, string username, string password);
    bool write_imgs_to_csv(string filename);
    bool write_users_to_csv(string filename);
    bool sign_up(string un, string pwd);
    bool add_img(string imgname, string un, string pwd);
    void append_img_access_list(string imgname, string un, int v);
    void sendReply(char* message);

    Message *getRequest();
    Message *doOperation();

public:
    Server(char *_listen_hostname, int _listen_port);
    bool serveRequest();
    ~Server();
};
#endif // SERVER_H


Server::Server(char * _listen_hostname, int _listen_port)
{
    total_num_frags = 0;
    last_rpcid = -1;
    populate_imgs("imgs.csv");
    populate_users("users.csv");
    udpServerSocket = new UDPServerSocket;
    udpServerSocket->initializeServer(_listen_hostname, _listen_port);
    
    
}




bool Server::populate_users(string filename)
{
    string current_line="";
    string un="";
    string pwd="";

    fstream file_userscsv(filename);
    if (!file_userscsv.is_open())
    {
        cout << "Could not open users table." << endl;
        return false;
    }

    while (getline(file_userscsv, current_line)) {
        
        stringstream line_reader(current_line);
        getline(line_reader, un, ',');
        getline(line_reader, pwd);
        
        User* temp_user = new User(un, pwd);
        users.push_back(*temp_user);
        delete temp_user;
        
    }

    return true;
}

bool Server::populate_imgs(string filename)
{
    string current_line="";
    string imgname="";
    string un="";
    string pwd="";
    string access_user="";
    string access_views=""; //converted to int later using stoi


    fstream file_imgscsv(filename);
    if (!file_imgscsv.is_open())
    {
        cout << "Could not open images table." << endl;
        return false;
    }

    while (getline(file_imgscsv, current_line)) {
        
        stringstream line_reader(current_line);
        getline(line_reader, imgname, ',');
        getline(line_reader, un, ',');
        getline(line_reader, pwd, ',');
        
        Img* temp_img = new Img(imgname, un, pwd);
        while (getline(line_reader, access_user, ','))
        {
            getline(line_reader, access_views, ',');
            temp_img->append_viewing_user(access_user, stoi(access_views));
        }
        images.push_back(*temp_img);
        delete temp_img;
        
    }
    
    return true;
}

/*
 1  : success
-1  : username does not exist
-2  : wrong password
*/
int Server::user_on_system(string username, string password)
{
    //check if username and password are correct
    bool user_exists_flag = false;
    for (int i=0; i<users.size(); i++)
    {
        if (users[i].correct_username(username))
        {
            user_exists_flag = true;
            if (!users[i].correct_password(password))
            {
                return -2;
            }
            else
            {
                return 1;
            }
        }
    }
    if (!user_exists_flag)
        return -1;

    return 1;
}


/*
 1  : success
-1  : username does not exist
-2  : wrong password
-3  : image does not exist
-4  : user not authorized for image
-5  : user expired his views
*/
int Server::user_allowed_to_view(string imgname, string username, string password)                        
{
    //check if username and password are correct
    bool user_exists_flag = false;
    for (int i=0; i<users.size(); i++)
    {
        if (users[i].correct_username(username))
        {
            user_exists_flag = true;
            if (!users[i].correct_password(password))
            {
                return -2;
            }
        }
    }
    if (!user_exists_flag)
        return -1;

    
    //check if user has rights to view image
    //created it or on access list, and if on access list that their views haven't expired
    bool image_exists = false;
    for (int i=0; i<images.size(); i++)
    {
        if (images[i].get_image_name() == imgname)
        {
            if (images[i].get_username()==username)
            {
                cout << "Img's username: " << images[i].get_username() << endl;
                cout << "Parsed username: " << username << endl;
                return 1;
            }
            else
            {
                return (images[i].allowed_to_view(username));
            }
        }
    }
    
    return -3;

}

bool Server::write_imgs_to_csv(string filename)
{
    ofstream file_imgscsv(filename);
    int access_list_size;
    if (!file_imgscsv.is_open())
    {
        cout << "Could not open images table." << endl;
        return false;
    }

    for (int i=0; i<images.size(); i++)
    {
        file_imgscsv << images[i].get_image_name() << ","
                    << images[i].get_username() << ","
                    << images[i].get_password();
        
        access_list_size = images[i].get_access_list_size();
        for (int j=0; j<access_list_size; j++)
        {
            file_imgscsv << "," << images[i].get_access_user(j)
                         << "," << images[i].get_access_view(j);
        }
        file_imgscsv << endl;
    }

    return true;

    return true;
}

bool Server::write_users_to_csv(string filename)
{
    fstream file_userscsv(filename);
    if (!file_userscsv.is_open())
    {
        cout << "Could not open users table." << endl;
        return false;
    }

    for (int i=0; i<users.size(); i++)
    {
        file_userscsv << users[i].get_username() << "," << users[i].get_password() << endl;
    }

    return true;
}

bool Server::sign_up(string un, string pwd)
{
    for (int i=0; i<users.size(); i++)
    {
        if (users[i].correct_username(un))
        {
            //username aleady exists
            cout << "username already exists.\n";
            return false;
        }
            
    }
   
    //this segment is reached if and only if the username does not exist
    User* temp_user = new User(un, pwd);
    users.push_back(*temp_user);
    delete temp_user;
    return true;
    
}

bool Server::add_img(string imgname, string un, string pwd)
{
    for (int i=0; i<images.size(); i++)
    {
        if (images[i].get_image_name()==imgname)
        {
            //image with same name aleady exists
            cout << "image with same name aleady exists.\n";
            return true;
        }
            
    }
   
    //this segment is reached if and only if the image name does not exist
    Img* temp_img = new Img(imgname, un, pwd);
    images.push_back(*temp_img);
    delete temp_img;
    return true;
}

void Server::append_img_access_list(string imgname, string un, int v)
{
    int index;
    int access_list_size;

    for (int i=0; i<images.size(); i++)
    {
        if (images[i].get_image_name()==imgname)
        {
            index = i;
            break;
        }
    }

    images[index].append_viewing_user(un, v);    
}




bool Server::serveRequest()
{
    //GET REQUEST
    char * buffer;
    buffer = (char*) malloc(MAX_BUFFER_LEN * sizeof(char));

    int return_code = udpServerSocket->readSocketWithNoBlock(buffer, MAX_BUFFER_LEN);





    //DO OPERATION
    if (buffer[0] == 'q')
    {
        return false;
    }




    string current_word;

    stringstream line_reader(buffer);

    int imgSize = 0;
    

    string rpcid_string;
    int rpcid;

    string op;
    int operation;
    
    string f_no;
    int frag_no;

    string lff;
    int last_frag_flag;

    string imgname;
    string username;
    string password;

    string user_access;
    string view_access;

    int counter_string_length = 0;





    getline(line_reader, rpcid_string, ',');
    rpcid = stoi(rpcid_string);
    counter_string_length += (rpcid_string.size()+1);
    
    if (rpcid==last_rpcid)
    {
        cout << "Detected duplicate packet.\n";
        string message_buffer = "Success.";
        sendReply((char*) message_buffer.c_str());

        return true;
    }
    else
    {
        cout << "Different rpcid detected. Not a duplicate packet.\n";
        last_rpcid = rpcid;
    }





    getline(line_reader, op,',');
    operation = stoi(op);
    counter_string_length += (op.size()+1);


    if (operation == 1)
    {
        getline(line_reader, username,',');
        getline(line_reader, password,',');

        if (sign_up(username, password))
        {
            // cout << "Successful Sign Up\n";
            string message_buffer = "Successful Sign Up.";
            sendReply((char*) message_buffer.c_str());

            return true;
        }
        else
        {
            // cout << "Unsuccessful Sign Up. Username exists already.\n";
            string message_buffer = "Unsuccessful Sign Up. Username exists already.";
            sendReply((char*) message_buffer.c_str());

            return true;
        }

    }
    else if (operation == 2)
    {
        getline(line_reader, imgname,',');
        getline(line_reader, username,',');
        getline(line_reader, password,',');

        counter_string_length += (imgname.size()+1 + username.size()+1 + password.size()+1);


        if (user_on_system(username, password)>0)
        {
            if (!add_img(imgname, username, password))
            {
                // cout << "Image with the same name exists. Choose different name.\n";
                string message_buffer = "Image with the same name exists. Choose different name.";
                sendReply((char*) message_buffer.c_str());

                return true;
            }

            while (getline(line_reader, current_word,','))
            {
                counter_string_length += (current_word.size()+1);


                if (current_word=="abort")
                {
                    getline(line_reader, current_word,',');
                    counter_string_length += (current_word.size()+1);

                    imgSize = stoi(current_word);
                    // cout <<"The image size is: " << imgSize << endl;
                    break;  
                }
                else
                {
                    user_access = current_word;
                    
                    getline(line_reader, current_word,',');
                    counter_string_length += (current_word.size()+1);

                    view_access = current_word;
                    
                    append_img_access_list(imgname, user_access, stoi(view_access));
                }
            }

            // cout << "There are " << counter_string_length << " characters before the image.\n";
            char* imgBuffer = new char [imgSize];


            memcpy(imgBuffer, buffer+counter_string_length, imgSize); //not 30

            if (return_code<0)
                udpServerSocket->incrementExperimentErrors();

            udpServerSocket->incrementExperimentTotal();
            
            // printf("Read Socket Returned: %d\n", return_code);
            

            Message * reply_message = new Message (buffer, strlen(buffer));
       
            //SEND REPLY
            udpServerSocket->writeToSocket(reply_message->getMessage(), reply_message->getMessageSize());
            printf("Server has sent reply.\n\n");


            string output_filename = imgname;
            ofstream fout(output_filename, ios::binary);
            if(!fout.good())
            {
                cout << "Error writing into image.";
                return 1;
            }
            fout.write(imgBuffer, imgSize);
            // printf("%s\n", imgBuffer);
            fout.close();

            

            delete reply_message;
            delete imgBuffer;



        }
        else
        {
            // cout << "User is not allowed to add images. Please sign up first.\n";
            string message_buffer = "User is not allowed to add images. Please sign up first.";
            sendReply((char*) message_buffer.c_str());

            return true;
        }


        return true;


    }
    else if (operation==20)
    {


        vector<string> temp_list_users;
        vector<int> temp_list_views;


        getline(line_reader, f_no,',');
        frag_no = stoi(f_no);
        counter_string_length += (f_no.size()+1);

        getline(line_reader, lff,',');
        last_frag_flag = stoi(lff);
        counter_string_length += (lff.size()+1);

        getline(line_reader, imgname,',');
        getline(line_reader, username,',');
        getline(line_reader, password,',');

        counter_string_length += (imgname.size()+1 + username.size()+1 + password.size()+1);

        while (getline(line_reader, current_word,','))
        {
            counter_string_length += (current_word.size()+1);

            if (current_word=="abort")
            {
                getline(line_reader, current_word,',');
                counter_string_length += (current_word.size()+1);
                imgSize = stoi(current_word);
                // cout <<"The image size is: " << imgSize << endl;
                break;
            }
            
            else
            {
                if (last_frag_flag==1)
                    temp_list_users.push_back(current_word);
                    
                getline(line_reader, current_word,',');
                counter_string_length += (current_word.size()+1);
                
                temp_list_views.push_back(stoi(current_word));

            }
        }
        
        cout << "Received frame #" << f_no << endl;      


        if (last_frag_flag==1)
        {
            if (user_on_system(username, password)>0)
            {

                if (!add_img(imgname, username, password))
                {
                    // cout << "Image with the same name exists. Choose different name.\n";
                    string message_buffer = "Image with the same name exists. Choose different name.";
                    sendReply((char*) message_buffer.c_str());

                    return true;
                }

                for (int i=0; i<temp_list_users.size(); i++)
                {
                    append_img_access_list(imgname, temp_list_users[i], temp_list_views[i]);
                }


                int last_fragment_size = imgSize - (frag_no*IMG_BUFFER_LEN);
            
                memcpy(frags_array[frag_no], buffer+counter_string_length, last_fragment_size);
                total_num_frags++;
                
                string output_filename = imgname ;
                ofstream* fout_final = new ofstream(output_filename, ios::binary);

                if(!fout_final->good())
                {
                    cout << "Error writing into image.";
                    return 1;
                }
                
                // cout << "total_num_frags: " << total_num_frags<<endl;
                
                for (int i=0; i<total_num_frags; i++)
                {
                    // cout << "\nIn new array loop.\n";

                    if (i==total_num_frags-1)
                    {
                        fout_final->write(frags_array[i], last_fragment_size);
                    }
                    else
                    {
                        fout_final->write(frags_array[i], IMG_BUFFER_LEN);
                    }
                }

                fout_final->close();

                delete fout_final;
                

                total_num_frags = 0;


            }
            else
            {
                // cout << "User is not allowed to add images. Please sign up first.\n";
                string message_buffer = "User is not allowed to add images. Please sign up first.";
                sendReply((char*) message_buffer.c_str());

                return true;
            }

            

            
        }
        else
        {
            memcpy(frags_array[frag_no], buffer+counter_string_length, IMG_BUFFER_LEN);
            total_num_frags++;
        }


    //////////////////////////////////////////////////////////////////////////////////////////////////////
        if (return_code<0)
            udpServerSocket->incrementExperimentErrors();

        udpServerSocket->incrementExperimentTotal();
        



        //SEND REPLY
        string message_buffer = "Successful.";
        sendReply((char*) message_buffer.c_str());

        return true;
        

    }
    else if(operation == 3)
    {
        getline(line_reader, imgname,',');
        getline(line_reader, username,',');
        getline(line_reader, password,',');
        counter_string_length += (imgname.size()+1 + username.size()+1 + password.size()+1);

        int status = user_allowed_to_view(imgname, username, password);
        if(status>0)
        {
            ifstream imgFile(imgname, ios::binary);
            if(!imgFile.good())
            {
                cout <<"Error reading image.";

                string message_buffer = "Error reading the image.";
                sendReply((char*) message_buffer.c_str());

                return true;
            }

            //Get size of the image
            imgFile.seekg(0, ios::end);
            size_t imgSize = (int)imgFile.tellg();
            
            //Return "cursor" to start of file
            imgFile.seekg(0);

            char imgBuffer[imgSize];
            
            int num_fragments = (imgSize/IMG_BUFFER_LEN) + 1;
            int sent_op = 44;

            // cout << "Number of frags: " << num_fragments << endl;

            string marshalled; 

            int last_fragment_size = imgSize - ((num_fragments-1)*IMG_BUFFER_LEN);


            for (int i=0; i<num_fragments; i++)
            {
                marshalled = to_string(sent_op) + ",";
                marshalled = marshalled.append(to_string(i) + ",");
                if (i==num_fragments-1)
                {
                    marshalled = marshalled.append("1,");
                    marshalled=marshalled.append(to_string(imgSize)+",");
                    
                    int last_fragment_size = imgSize - ((num_fragments-1)*IMG_BUFFER_LEN);
                    char* imgBuffer = new char[last_fragment_size];
                    if(imgFile.read((char*)imgBuffer, last_fragment_size))
                    {
                        char mchar[MAX_BUFFER_LEN];
                        memcpy(mchar, marshalled.c_str(), marshalled.size());
                        memcpy(mchar+marshalled.size(), imgBuffer, last_fragment_size);

                        cout <<"Frag " << i << " has sent.\n";

                        Message * img_message = new Message (mchar, MAX_BUFFER_LEN);

                        udpServerSocket->writeToSocket(img_message->getMessage(), img_message->getMessageSize());
                        printf("Server has sent reply.\n");
                        delete img_message;

                    }
                    delete imgBuffer;
                    
                }
                    
                else
                {
                    marshalled = marshalled.append("0,");
                    marshalled=marshalled.append(to_string(imgSize)+",");
                    char imgBuffer[IMG_BUFFER_LEN];
                    if(imgFile.read((char*)imgBuffer, IMG_BUFFER_LEN))
                    {
                        char mchar[MAX_BUFFER_LEN];
                        memcpy(mchar, marshalled.c_str(), marshalled.size());
                        memcpy(mchar+marshalled.size(), imgBuffer, IMG_BUFFER_LEN);

                        cout <<"Frag " << i << " has sent.\n";

                        Message * img_message = new Message (mchar, MAX_BUFFER_LEN);
                        udpServerSocket->writeToSocket(img_message->getMessage(), img_message->getMessageSize());
                        printf("Server has sent reply.\n");
                        delete img_message;

                        int return_code_2 = udpServerSocket->readSocketWithNoBlock(buffer, MAX_BUFFER_LEN); //Waiting for Client to send ACK

                    }
                }

            }
   
        }
        else
        {
            int sent_op = 55;
            string message = to_string(sent_op) + "," + to_string(status) + ",Not allowed to view image.";
            sendReply((char*)message.c_str());
        }

    }

    return true;

}



void Server::sendReply(char* this_reply)
{
    Message * reply_message = new Message (this_reply, strlen(this_reply));
    udpServerSocket->writeToSocket(reply_message->getMessage(), reply_message->getMessageSize());
    printf("Server has sent reply.\n");
    delete reply_message;
}


Server::~Server()
{
    if (!write_imgs_to_csv("imgs.csv"))
    {
        cout << "Could not write images to imgs.csv.\n";
    }
    if (!write_users_to_csv("users.csv"))
    {
        cout << "Could not write users to users.csv.\n";
    }

    delete udpServerSocket;
}