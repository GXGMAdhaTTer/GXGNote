#include "EventLoop.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
#include <utility>

using std::cout;
using std::endl;

class MyTask {
public:
    MyTask(const string& msg, const TcpConnectionPtr& con)
    : _msg(msg), _con(con) {}
    
    void process() {
        //业务逻辑
        /* _con->send(_msg); */
        _con->sendInLoop(_msg);
        //数据的发送需要在EventLoop里面发送
        //TcpConnection需要将数据发送给EventLoop，让EventLoop进行发送数据IO操作，此时TcpConnection需要知道EventLoop存在
    }
private:
    string _msg;
    TcpConnectionPtr _con;
};

ThreadPool* gThreadPool = nullptr;

void onConnection(const TcpConnectionPtr& con) {
    cout << con->toString() << " has connected!" << endl;
}

void onMessage(const TcpConnectionPtr& con) {
    string msg = con->receive();
    cout << "recv msg   " << msg << endl;
    MyTask task(msg, con);
    gThreadPool->addTask(std::bind(&MyTask::process, task));
    /* con->send(msg); */
}

void onClose(const TcpConnectionPtr& con) {
    cout << con->toString() << " has closed!" << endl;
}

void test() {
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();

    EventLoop loop(acceptor);
    loop.setConnectionCallback(std::move(onConnection));
    loop.setMessageCallback(std::move(onMessage));
    loop.setCloseCallback(std::move(onClose));

    loop.loop();
}

void test2() {
    ThreadPool threadPool(5, 10); //局部变量
    threadPool.start();
    gThreadPool = &threadPool;
    TcpServer server("127.0.0.1", 8888);
    server.setAllCallback(std::move(onConnection), 
                          std::move(onMessage), 
                          std::move(onClose));
    server.start();
}

int main() {
    /* test(); */
    test2();
    return 0;
}
