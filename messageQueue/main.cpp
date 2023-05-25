#include "messageQueue.hpp"
#include <thread>

using namespace std;

//1.使用之线程通信

//定义消息结构体
class MyMessage{
public:
    int type;
    void* param1;
    void* param2;
};


int main(int argc, char* argv[]){
    //初始化消息队列
    MessageQueue<MyMessage>  mq;

    //启动线程
    thread t1([&](){
        MyMessage msg;
        
        while (1){   
            //等待队列消息
            mq.wait(msg);
            printf("receive message type:%d\n", msg.type);
			if (msg.type == 1001)
				break;
        }
        

        /*
        while (1) {
			//轮询队列的消息
			if (mq.poll(msg))
			{
				printf("receive message type:%d\n", msg.type);
				if (msg.type == 1001)
					break;
			}
			this_thread::sleep_for(std::chrono::milliseconds(10));
		}
        */

        printf("thread exited\n");
    });

    //给线程发送消息
    MyMessage msg;
    printf("send number message to thread.1001 exit\n");
    while(1){
        scanf("%d", &msg.type);
        mq.push(msg);
        if(msg.type == 1001)
            break;
    }
    t1.join();
    
    return 0;
}
