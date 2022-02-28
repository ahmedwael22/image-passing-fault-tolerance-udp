//system libraries
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <netdb.h>
#include <sstream>
#include <bitset>
#include <vector>
#include <stdlib.h>
#include <time.h>

#define MAX_BUFFER_LEN 65000
#define IMG_BUFFER_LEN 64000
#define NUM_FRAGMENTS_MAX 50


using namespace std;

//user-defined classes
// #include "message.h"
#include "client.h"



int main (int argc, char** argv)
{

    char * server_address = argv[1];
    int server_port = stoi(argv[2]);

    Message * reply_message = new Message;


    char * message_string ;
    message_string = (char*) malloc(sizeof(char) * 2048); //= "12345678901234567890";


    Message *message = new Message (message_string, strlen(message_string));
    message->setMessageType(Request);



    Client my_client(server_address, server_port);

    char frags_array[NUM_FRAGMENTS_MAX][IMG_BUFFER_LEN];
    int total_num_frags;    

    total_num_frags=0;



    srand(time(0));



    int rpc_id = rand();


    while(true){


        ////////////////////////////////////////////////////////////////////////////////////////////////
        int operation;

        cout << endl;
        cout << "To signup please enter 1\n"; 
        cout << "To upload an image please enter 2\n"; 
        cout << "To view an image please enter 3\n"; 
        cout << "To exit, please enter 4\n";
        cin >> operation;
        cout << endl;

        if (operation ==1)
        {
            string sign_marshalled;

            sign_marshalled = to_string(rpc_id)+",";
            rpc_id++;

            sign_marshalled = sign_marshalled.append(to_string(operation)+",");

            string username;
            cout << "Please enter your username: ";  
            cin >> username; 
            sign_marshalled = sign_marshalled.append(username+",");

            string password;
            cout << "Please enter your password: ";  
            cin >> password; 
            sign_marshalled = sign_marshalled.append(password);


            message->setMessage((char*) sign_marshalled.c_str(), sign_marshalled.size());
            reply_message = my_client.execute(message);

            // cout << reply_message->getMessage() << endl;



        }
        else if (operation == 2)
        {
            string marshalled;

            string imageName;
            cout << "Please enter the image file name: ";  
            cin >> imageName; 
            ifstream imgFile(imageName, ios::binary);
            if(!imgFile.good())
            {
                cout <<"Error reading image.";
                return 1;
            }

            //Get size of the image
            imgFile.seekg(0, ios::end);
            size_t imgSize = (int)imgFile.tellg();
            
            //Return "cursor" to start of file
            imgFile.seekg(0);

            // cout << imgSize << endl;

            if (imgSize < IMG_BUFFER_LEN)
            {
                //do not fragment

                marshalled = to_string(rpc_id)+",";
                rpc_id++;

                marshalled = marshalled.append(to_string(operation)+",");

                marshalled = marshalled.append(imageName+",");

                string username;
                cout << "Please enter your username: ";  
                cin >> username; 
                //username = "wadham";
                marshalled = marshalled.append(username+",");

                string password;
                // password="adel";
                cout << "Please enter your password: ";  
                cin >> password; 
                marshalled = marshalled.append(password+",");

                string views;
                string nameView;
                int numUsers = 2;

                cout << "Please enter the number of viewers of the image: ";
                cin >> numUsers;

                for (int i = 1; i <= numUsers; i++)
                {
                    cout << "Enter the username #"<< i << ": " ;
                    cin >> nameView;
                    cout << "Enter the number of views for "<< nameView << ": " ;
                    cin >> views;

                    marshalled = marshalled.append(nameView+",");
                    marshalled = marshalled.append(views+",");

                }


                marshalled = marshalled.append("abort,");

                marshalled=marshalled.append(to_string(imgSize)+",");


                char imgBuffer[imgSize];
                if(imgFile.read((char*)imgBuffer, imgSize))
                {
                    char mchar[MAX_BUFFER_LEN];
                    memcpy(mchar, marshalled.c_str(), marshalled.size());
                    memcpy(mchar+marshalled.size(), imgBuffer, imgSize);

                    
                    message->setMessage(mchar , MAX_BUFFER_LEN);
                    reply_message = my_client.execute(message);

                    // cout << reply_message->getMessage() << endl;

                }

            }
            else
            {
                //we must fragment

                operation = 20;

                int num_fragments = (imgSize/IMG_BUFFER_LEN) + 1;


                string username;
                cout << "Please enter your username: ";  
                cin >> username;
                string password;
                cout << "Please enter your password: ";  
                cin >> password;



                string views;
                string nameView;
                int numUsers;

                cout << "Please enter the number of viewers of the image: ";
                cin >> numUsers;

                vector<string> users_viewing;
                vector<string> view_counts;

                for (int i = 0; i < numUsers; i++)
                {
                    cout << "Enter the username #"<< i+1 << ": " ;
                    cin >> nameView;
                    cout << "Enter the number of views for "<< nameView << ": " ;
                    cin >> views;

                    users_viewing.push_back(nameView);
                    view_counts.push_back(views);
                }


                for (int j = 0; j < num_fragments; j++)
                {

                    marshalled = to_string(rpc_id)+",";
                    rpc_id++;

                    marshalled = marshalled.append(to_string(operation)+","); //reset string

                    //frag-number
                    marshalled = marshalled.append(to_string(j)+",");

                    //final-frag-flag
                    if (j==num_fragments-1)
                        marshalled = marshalled.append("1,");
                    else
                        marshalled = marshalled.append("0,");

                    marshalled = marshalled.append(imageName+",");
                    marshalled = marshalled.append(username+",");
                    marshalled = marshalled.append(password+",");


                    for (int i = 0; i < numUsers; i++)
                    {
                        marshalled = marshalled.append(users_viewing[i]+",");
                        marshalled = marshalled.append(view_counts[i]+",");
                    }

                    marshalled = marshalled.append("abort,");
                    marshalled = marshalled.append(to_string(imgSize)+",");


                    // cout << "\nAppended image.\n

                    if (j==num_fragments-1)
                    {

                        int last_fragment_size = imgSize - ((num_fragments-1)*IMG_BUFFER_LEN);
                        char* imgBuffer = new char[last_fragment_size];
                        if(imgFile.read((char*)imgBuffer, last_fragment_size))
                        {
                            char mchar[MAX_BUFFER_LEN];
                            memcpy(mchar, marshalled.c_str(), marshalled.size());
                            memcpy(mchar+marshalled.size(), imgBuffer, last_fragment_size);


                            cout <<"Frag " << j << " has sent.\n";
                            message->setMessage(mchar , MAX_BUFFER_LEN);
                            reply_message = my_client.execute(message);

                            // cout << reply_message->getMessage() << endl;
                            


                        }
                        delete imgBuffer;
                    }
                    else
                    {
                        char imgBuffer[IMG_BUFFER_LEN];
                        if(imgFile.read((char*)imgBuffer, IMG_BUFFER_LEN))
                        {


                            // cout << "marshalled " << marshalled << endl;


                            char mchar[MAX_BUFFER_LEN];
                            memcpy(mchar, marshalled.c_str(), marshalled.size());
                            memcpy(mchar+marshalled.size(), imgBuffer, IMG_BUFFER_LEN);

                            

                            cout <<"Frag " << j << " has sent.\n";
                            message->setMessage(mchar , MAX_BUFFER_LEN);
                            reply_message = my_client.execute(message);
                            
                            cout << "Reply received for frag j=" << j << ".\n";

                            // cout << reply_message->getMessage() << endl;
                        }
                    }
                    
                }
                cout << "All fragments have been sent.\n";
            }

      
        }

        else if (operation == 3)
        {
            string view_marshalled;

            view_marshalled = to_string(rpc_id)+",";
            rpc_id++;

            view_marshalled = view_marshalled.append(to_string(operation)+",");

            string imageName;
            cout << "Please enter the image file name: ";  
            cin >> imageName; 
            view_marshalled = view_marshalled.append(imageName+",");

            string username;
            cout << "Please enter your username: ";  
            cin >> username;
            view_marshalled = view_marshalled.append(username+",");

            string password;
            cout << "Please enter your password: ";  
            cin >> password; 
            view_marshalled = view_marshalled.append(password);




            message->setMessage((char*) view_marshalled.c_str(), view_marshalled.size());


            reply_message = my_client.execute(message);


            char * rcvdbuffer;
            rcvdbuffer = (char *) malloc (MAX_BUFFER_LEN*sizeof(char));




            memcpy(rcvdbuffer, reply_message->getMessage(), reply_message->getMessageSize());
            stringstream line_reader(rcvdbuffer);




            string op;
            int rcvd_operation;
            
            string f_no;
            int frag_no;
            string lff;
            int last_frag_flag;

                                                

            int counter_string_length = 0;




            getline(line_reader, op,',');
            rcvd_operation = stoi(op);
            counter_string_length += (op.size()+1);
            if(rcvd_operation == 99)
            {
                cout << "Timeout. Will not attempt to send packet any further.\n";
                continue;
            }


            if(rcvd_operation==44)
            {

                
                getline(line_reader, f_no,',');
                frag_no = stoi(f_no);
                counter_string_length += (f_no.size()+1);

                getline(line_reader, lff,',');
                last_frag_flag = stoi(lff);
                counter_string_length += (lff.size()+1);

                string tempImgSize;
                int imgSize;

                
                getline(line_reader, tempImgSize, ',');
                imgSize = stoi(tempImgSize);
                counter_string_length += (tempImgSize.size()+1);

                int num_fragments_rcvd = imgSize/IMG_BUFFER_LEN + 1;



                if (last_frag_flag==1)
                {



                    int last_fragment_size = imgSize - (frag_no*IMG_BUFFER_LEN);
                
                    memcpy(frags_array[frag_no], rcvdbuffer+counter_string_length, last_fragment_size);
                    total_num_frags++;


                    ofstream* fout_final = new ofstream(imageName, ios::binary);


                    if(!fout_final->good())
                    {
                        cout << "Error writing into image.";
                        return 1;
                    }
                    
                    
                    for (int i=0; i<num_fragments_rcvd; i++)
                    {

                        if (i==num_fragments_rcvd-1)
                        {
                            // cout << i << " " << last_fragment_size << endl;
                            fout_final->write(frags_array[i], last_fragment_size);
                        }
                        else
                        {
                            // cout << i << " " << IMG_BUFFER_LEN << endl;
                            fout_final->write(frags_array[i], IMG_BUFFER_LEN);
                        }
                    }

                    fout_final->close();

                    delete fout_final;

                    total_num_frags = 0;


                }
                else
                {
                    memcpy(frags_array[frag_no], rcvdbuffer+counter_string_length, IMG_BUFFER_LEN);
                    total_num_frags++;
                }



                string ack_message = "ACK"; //major constituent of fault tolerance
                for (int i=1; i<num_fragments_rcvd; i++)
                {


                    counter_string_length=0;



                    message->setMessage((char*) ack_message.c_str(), ack_message.size());
                    reply_message = my_client.execute(message);

                    memcpy(rcvdbuffer, reply_message->getMessage(), reply_message->getMessageSize());
                    stringstream line_reader(rcvdbuffer);






                    //not really needed for anything but advancing the cursor
                    getline(line_reader, op,',');
                    rcvd_operation = stoi(op);
                    counter_string_length += (op.size()+1);
                    if(rcvd_operation == 99)
                    {
                        cout << "Timeout. Will not attempt to send this packet any further.\n";
                        break;
                    }


                    getline(line_reader, f_no,',');
                    frag_no = stoi(f_no);
                    counter_string_length += (f_no.size()+1);

                    getline(line_reader, lff,',');
                    last_frag_flag = stoi(lff);
                    counter_string_length += (lff.size()+1);

                    getline(line_reader, tempImgSize, ',');
                    imgSize = stoi(tempImgSize);
                    counter_string_length += (tempImgSize.size()+1);





                    //////



                    if (last_frag_flag==1)
                    {
                        int last_fragment_size = imgSize - (frag_no*IMG_BUFFER_LEN);
                    
                        memcpy(frags_array[frag_no], rcvdbuffer+counter_string_length, last_fragment_size);
                        total_num_frags++;


                        ofstream* fout_final = new ofstream(imageName, ios::binary);


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
                                // cout << i << " " << last_fragment_size << endl;
                                fout_final->write(frags_array[i], last_fragment_size);
                            }
                            else
                            {
                                // cout << i << " " << IMG_BUFFER_LEN << endl;
                                fout_final->write(frags_array[i], IMG_BUFFER_LEN);
                            }
                        }

                        fout_final->close();

                        delete fout_final;

                        total_num_frags = 0;


                    }
                    else
                    {
                        memcpy(frags_array[frag_no], rcvdbuffer+counter_string_length, IMG_BUFFER_LEN);
                        total_num_frags++;
                    }

                }
            }
            else if(rcvd_operation==55)
            {
                string status;
                getline(line_reader, status, ',');
                cout << "Not allowed to view. Error code = " << status << endl;
                cout << "-1  : username does not exist\n-2  : wrong password\n-3  : image does not exist\n-4  : user not authorized for image\n-5  : user expired his views\n";
            }
        
        }
        else if (operation==4)
        {

            string msg="q";
            message->setMessage((char*) msg.c_str(), msg.size());
            reply_message = my_client.execute(message);

            // cout << reply_message->getMessage();

            break;
        }
        else
        {
            cout << "WRONG INPUT.\n";
            continue;
        }


    }




    //free (message_string)
    return 0;
}

