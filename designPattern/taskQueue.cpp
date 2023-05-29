/*使用单例模式实现一个任务队列*/

#include <iostream>
#include <queue>
#include <mutex>
#include <thread>

using namespace std;

//定义任务队列
class TaskQueue{
public:

    TaskQueue(const TaskQueue& t) = delete;
    TaskQueue& operator=(const TaskQueue& t) = delete; 
    static TaskQueue* getInstance(){
        return &_obj;
    }

    //队列是否为空
    bool isEmpty(){
        lock_guard<mutex> locker(_mutex);
        bool flag = _taskQ.empty();
        return flag;
    }
    //添加任务
    bool addTask(int data){
        lock_guard<mutex> locker(_mutex);
        _taskQ.push(data);
    }

    //取出任务
    int takeTask(){
        lock_guard<mutex> locker(_mutex);
        if(!_taskQ.empty()){
            return _taskQ.front();
        }
        return -1;
    }

    //删除任务
    bool popTask()
    {
        lock_guard<mutex> locker(mutex);
        if (!_taskQ.empty())
        {
            _taskQ.pop();
            return true;
        }
        return false;
    }

private:
    TaskQueue() = default;

    static TaskQueue _obj;
    queue<int> _taskQ;
    mutex _mutex;
};

//静态变量的初始化|声明
TaskQueue TaskQueue::_obj;

int main(){

    thread t1([] () {
        TaskQueue* taskQ = TaskQueue::getInstance();
        for(int i = 0; i < 100; i++){
            taskQ->addTask(i + 100);
            cout << "+++push task: " << i + 100 << ", threadID: " 
                << this_thread::get_id() << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    });

    thread t2([] () {
        TaskQueue* taskQ = TaskQueue::getInstance();
        this_thread::sleep_for(chrono::milliseconds(100));
        while (!taskQ->isEmpty()){
            int data = taskQ->takeTask();
            cout << "---take task: " << data << ", threadID: " 
                << this_thread::get_id() << endl;
            taskQ->popTask();
            this_thread::sleep_for(chrono::seconds(1));
        }
    });

    t1.join();
    t2.join();

}