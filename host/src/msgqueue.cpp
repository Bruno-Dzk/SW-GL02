#include "msgqueue.hpp"

Message::Message(MsgType header, unsigned int numeric) : header(header), numeric(numeric) {}

Message::Message(MsgType header, std::string text) : header(header), text(text) {}

MsgQueue::MsgQueue(void)
    : queue()
    , mutex()
    , convar()
{}

MsgQueue::~MsgQueue(void){}

void MsgQueue::enqueue(Message message)
{
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(message);
    convar.notify_one();
}

Message MsgQueue::dequeue(void)
{
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty())
    {
        // release lock as long as the wait and reaquire it afterwards.
        convar.wait(lock);
    }
    Message message = queue.front();
    queue.pop();
    return message;
}

bool MsgQueue::empty(void)
{
    return queue.empty();
}