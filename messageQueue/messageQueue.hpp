/*使用场景：线程间通信
需要到的组件：队列、互斥变量、条件变量
使用泛型编程，可以自定义消息的类型*/

#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

template<class  T>
class MessageQueue{
private:
    queue<T> _queue;
    mutex _mtx;
    condition_variable _cv;
    
public:
    //推入消息
    void push(const T& msg){
        unique_lock<mutex> lck(_mtx);
        _queue.push(msg);
        _cv.notify_one();
    }

    //轮询消息,取队头元素
    bool poll(T& msg){
        unique_lock<mutex> lck(_mtx);
        if(_queue.size()){
            msg = _queue.front();
            _queue.pop();
            return true;
        }
        return false;
    }

    //等待消息,轮询的函数会叫醒他
    void wait(T& msg){
        unique_lock<mutex> lck(_mtx);
        while(!_queue.size()) _cv.wait(lck);
        msg = _queue.front();
        _queue.pop();
    }

    //队列长度
    size_t size(){
        unique_lock<mutex> lck(_mtx);
        return _queue.size();
    }

};

