#ifndef MESSAGE_H
#define MESSAGE_H
enum MessageType
{
    Request,
    Reply
};
class Message
{
private:
    MessageType message_type;
    int operation;
    char *message;
    size_t message_size;
    int rpc_id;

public:
    Message();
    Message(int operation, char *p_message, size_t p_message_size, int p_rpc_id);
    Message(char *p_message, size_t p_message_size);
    Message(char *marshalled_base64);
    char *marshal();
    int getOperation();
    int getRPCId();
    char *getMessage();
    size_t getMessageSize();
    MessageType getMessageType();
    void setOperation(int _operation);
    void setMessage(char *message, size_t message_size);
    void setMessage(int operation, char *p_message, size_t p_message_size, int p_rpc_id);
    void setMessageType(MessageType message_type);
    ~Message();
};
#endif // MESSAGE_H


Message::Message()
{

}

Message::Message(char *p_message, size_t p_message_size)
{
    message = p_message;
    message_size = p_message_size;
}

Message::Message(int operation, char *p_message, size_t p_message_size, int p_rpc_id)
{
    operation = operation;
    message = p_message;
    message_size = p_message_size;
    rpc_id = p_rpc_id;
}

char * Message::getMessage()
{
    return message;
}

size_t Message::getMessageSize()
{
    return message_size;
}

void Message::setMessage(char *p_message, size_t p_message_size){
    message = p_message;
    message_size = p_message_size;
}

void Message::setMessage(int operation, char *p_message, size_t p_message_size, int p_rpc_id){
    operation = operation;
    message = p_message;
    message_size = p_message_size;
    rpc_id = p_rpc_id;
}


void Message::setMessageType(MessageType message_type)
{
    message_type = message_type;
}

Message::~Message()
{
    // cout << "Message destructor.\n";
}