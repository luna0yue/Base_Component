/*模仿nginx内存池实现
参考：知乎——玉米*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

class SmallBlock{
public:
    char *cur_usable_buffer;
    char *buffer_end;
    SmallBlock * next_block;
    int no_enough_times;
};

class BigBlock{
public:
    char * big_buffer;
    BigBlock * next_block;
};


class MemoryPool{
public:
    size_t small_buffer_capacity;                //small_block的大小
    SmallBlock  * curr_usable_small_block;      //指向最新未用的small_block
    BigBlock * big_block_start;                 //big_block的链头
    SmallBlock small_block_start[0];            //samll_block的链头,“软性数组”

    static MemoryPool * createPool(size_t capacity);     //默认创建含一个small_block,零个big_block的内存块
    static void destoryPool(MemoryPool * pool);          //释放内存，销毁数据结构
    static char* createNewSmallBlock(MemoryPool * pool, size_t size);
    static char* mallocBigBlock(MemoryPool * pool, size_t size);
    static void* poolMalloc(MemoryPool * pool, size_t size);
    static void freeBigBlock(MemoryPool * pool, char * buffer_ptr);
};

//内存池的创建，默认含有一个samll_block,无big_block
MemoryPool* MemoryPool::createPool(size_t capacity){
    //分配一块内存，由pool, small_block, small_buffer构成,这样他们的内存是连在一起的，比较优雅~
    size_t total_size = sizeof(MemoryPool) + sizeof(SmallBlock) + capacity;
    void *temp = malloc(total_size);
    memset(temp, 0, total_size);

    MemoryPool * pool = (MemoryPool*)temp;
    fprintf(stdout, "pool address: %p \n", pool);

    //初始化pool对象
    pool->small_buffer_capacity = capacity;
    pool->big_block_start = nullptr;
    pool->curr_usable_small_block = (SmallBlock*)(pool->small_block_start);

    //从pool后面开始是第一个SmallBlock的地址
    SmallBlock * sbp = (SmallBlock*)(pool + 1);
    fprintf(stdout, "first samll_block address: %p \n", sbp);
    //初始化第一个small_block
    sbp->cur_usable_buffer = (char*)(sbp + 1);
    sbp->buffer_end = sbp->cur_usable_buffer + capacity;
    sbp->next_block = nullptr;
    sbp->no_enough_times = 0;

    return pool;
}

//内存分配
void* MemoryPool::poolMalloc(MemoryPool * pool, size_t size){
    //若所需size小于small_block的capacity则使用small_block
    if(size < pool->small_buffer_capacity){
        //从curr_small_block往后找
        SmallBlock * temp = pool->curr_usable_small_block;
        do{
            if(size < temp->buffer_end - temp->cur_usable_buffer){
                //curr_small_block够用，直接分配即可
                char* res = temp->cur_usable_buffer;
                temp->cur_usable_buffer = temp->cur_usable_buffer + size;
                return res;
            }
            temp = temp->next_block;
        }while(temp);

        //如果最后一个small_block不够用，则需创建新的
        return createNewSmallBlock(pool, size);
    }
    //否则创建big_block
    return mallocBigBlock(pool, size);
}

//创建small_block
char* MemoryPool::createNewSmallBlock(MemoryPool * pool, size_t size){
    size_t malloc_size = sizeof(SmallBlock) + pool->small_buffer_capacity;
    void * temp = malloc(malloc_size);
    memset(temp, 0, malloc_size);

    //初始化small_block
    SmallBlock * sbp = (SmallBlock*)temp;
    fprintf(stdout, "new samll_block address: %p \n", sbp);
    sbp->cur_usable_buffer = (char*)(sbp + 1);  //跨过一个small_block步长， ？sbp + 1是跨一个sbp的地址
    fprintf(stdout,"new small block buffer address:%p\n",sbp->cur_usable_buffer);
    sbp->buffer_end = (char*)temp + malloc_size;
    sbp->next_block = nullptr;
    sbp->no_enough_times = 0;

    //预留size给新分配的内存
    char* res = sbp->cur_usable_buffer;
    sbp->cur_usable_buffer += size;

    //更新pool的curr_usable_block
    SmallBlock * p = pool->curr_usable_small_block;
    while(p->next_block){
        if(p->no_enough_times > 4){
            pool->curr_usable_small_block = p->next_block;
        }
        ++(p->no_enough_times);
        p = p->next_block;
    }

    //将新的block接上去
    p->next_block = sbp;

    if(pool->curr_usable_small_block == nullptr){
        pool->curr_usable_small_block = sbp;
    }

    return res;
}

//分配大内存
char* MemoryPool::mallocBigBlock(MemoryPool * pool, size_t size){
    void * temp = malloc(size);
    memset(temp, 0, size);

    //从头节点开始查找,查找的是big_buffer为nullptr的big_block
    BigBlock * bbp = pool->big_block_start;
    int i = 0;
    while(bbp){
        if(bbp->big_buffer == nullptr){
            bbp->big_buffer = (char*) temp;
            return bbp->big_buffer;
        }
        if(i > 3){
            //若三轮还没找到则直接创建
            break;
        }
        bbp = bbp->next_block;
        ++i;
    }

    //插入时也采用头插
    //用来给big_block数据结构分配内存，它本身是小内存，所以调用poolMalloc分配空间
    BigBlock * new_bbp = (BigBlock*)MemoryPool::poolMalloc(pool, sizeof(BigBlock)); //为何套娃？
    new_bbp->big_buffer = (char*)temp;
    new_bbp->next_block = pool->big_block_start;
    pool->big_block_start = new_bbp;

    return new_bbp->big_buffer;
}

//释放大内存的buffer,传入buffer指针
void MemoryPool::freeBigBlock(MemoryPool * pool, char *buffer_ptr){
    BigBlock* bbp = pool->big_block_start;
    while(bbp){
        if(bbp->big_buffer == buffer_ptr){
            free(bbp->big_buffer);
            bbp->big_buffer = nullptr;
            return;
        }
        bbp = bbp->next_block;
    }
}

//销毁线程池
void MemoryPool::destoryPool(MemoryPool * pool){
    BigBlock * bbp = pool->big_block_start;
    while(bbp){
        if(bbp->big_buffer){
            free(bbp->big_buffer);
            bbp->big_buffer = nullptr;
        }
        bbp = bbp->next_block;
    }
    //为什么不删除big_block节点？因为big_block在小内存池中，等会就和小内存池一起销毁了
    //好巧妙！！！

    //第一个small_block在pool的最后，会随着pool一起释放 
    SmallBlock * temp = pool->small_block_start->next_block;
    while(temp){
        SmallBlock * next = temp->next_block;
        free(temp);
        temp = next;
    }
    free(pool);
}
















