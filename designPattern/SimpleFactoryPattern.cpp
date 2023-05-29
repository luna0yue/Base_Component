/*简单工厂模式：一个工厂生产同一种类的不同商品,通过枚举类型实现产品选择*/
#include <iostream>
using namespace std;

//虚基类
class AbstractSmile{
public:
    virtual void transform() {}
    virtual void ability() {}
    virtual ~AbstractSmile() {}
};

//派生类1
class BatSmile : public AbstractSmile{
public:
    void transform() override {
        cout << "变成蝙蝠人形态..." <<endl;
    }

    void ability() override {
        cout << "声纳引箭..." <<endl;
    }
};

//派生类2
class SheepSmile : public AbstractSmile{
public:
    void transform() override {
        cout << "变成山羊人形态..." <<endl;
    }

    void ability() override {
        cout << "将手臂变成绵羊角..." <<endl;
    }
};

//派生类3
class LionSmile : public AbstractSmile{
public:
    void transform() override {
        cout << "变成狮子人形态..." <<endl;
    }

    void ability() override {
        cout << "火遁*豪火球之术..." <<endl;
    }
};

//工厂
enum class Type:char {SHEEP, LION, BAT};
class SmileFactory{
public:
    SmileFactory() {}
    ~SmileFactory() {}
    AbstractSmile* creatSmile(Type type){
        AbstractSmile* ptr = nullptr;
        switch(type){
            case Type::SHEEP:
                ptr = new SheepSmile;
                break;
            case Type::LION:
                ptr = new LionSmile;
                break;
            case Type::BAT:
                ptr = new BatSmile;
                break;
            default:
                break;
        }
        return ptr;
    }
};

int main(){
    SmileFactory* factory = new SmileFactory;
    AbstractSmile * obj = factory->creatSmile(Type::BAT);
    obj->transform();
    obj->ability();
    return 0;
}

