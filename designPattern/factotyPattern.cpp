/*工厂模式：每一个工厂生产同一种类下的单一商品,定义不同的的工厂类，生成对应的商品，便于不同商品的拓展*/
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
class AbstructFactory{
public:
   virtual AbstractSmile* creatSmile() = 0;
   virtual ~AbstructFactory() {}
};

class SheepFactory : public AbstructFactory{
public:
    AbstractSmile* creatSmile(){
        return new SheepSmile;
    }

    ~SheepFactory(){
        cout << "释放 sheepFactory相关内存资源..." << endl;
    }
};

class LionFactory : public AbstructFactory{
public:
    AbstractSmile* creatSmile(){
        return new LionSmile;
    }

    ~LionFactory(){
        cout << "释放 lionFactory相关内存资源..." << endl;
    }
};

class BatFactory : public AbstructFactory{
public:
    AbstractSmile* creatSmile(){
        return new BatSmile;
    }

    ~BatFactory(){
        cout << "释放 batFactory相关内存资源..." << endl;
    }
};




int main(){
    AbstructFactory* factory1 = new SheepFactory;
    AbstractSmile * obj1 = factory1->creatSmile();
    obj1->transform();
    obj1->ability();

    AbstructFactory* factory2 = new LionFactory;
    AbstractSmile * obj2 = factory2->creatSmile();
    obj2->transform();
    obj2->ability();

    AbstructFactory* factory3 = new BatFactory;
    AbstractSmile * obj3 = factory3->creatSmile();
    obj3->transform();
    obj3->ability();
    return 0;
}
