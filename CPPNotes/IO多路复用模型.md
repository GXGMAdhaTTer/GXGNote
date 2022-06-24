# IO多路复用模型

## 定义

IO多路复用是一种同步IO模型，实现一个线程可以监听多个文件描述符；

一旦某个文件描述符就绪，就能通知用户态程序进行相应的读写操作；

没有文件描述符就绪时，会阻塞用户态程序，交出CPU；

**复用**是指一个线程处理多个文件描述符

常见的IO多路复用模型有**select/poll/epoll**

**多进程与多线程并发服务器，不经常使用这种作为大型服务器开发的原因是，所有的监听与访问请求都 由服务器操作**。可以使用多路IO转接服务器(也叫多任务IO服务器)，思想:不再由应用程序自己监视 客户端连接，取而代之由内核替应用程序监视文件。

![image-20220624101009769](https://gogogxg.xyz/img/20220624-163229-175.png)

## 手段

### 0. 对比

| 手段   | 原理                                                         |
| ------ | ------------------------------------------------------------ |
| select | select本质上是通过设置或者检查存放fd标志位的数据结构来进行下一步处理。这样所带来的缺点是：  1 单个进程可监视的fd数量被限制  2 需要维护一个用来存放大量fd的数据结构，这样会使得用户空间和内核空间在传递该结构时复制开销大  3 对socket进行扫描时是线性扫描 |
| poll   | poll本质上和select没有区别，它将用户传入的数组拷贝到内核空间，然后查询每个fd对应的设备状态，如果设备就绪则在设备等待队列中加入一项并继续遍历，如果遍历完所有fd后没有发现就绪设备，则挂起当前进程，直到设备就绪或者主动超时，被唤醒后它又要再次遍历fd。这个过程经历了多次无谓的遍历。  它没有最大连接数的限制，原因是它是基于链表来存储的，但是同样有一个缺点：  大量的fd的数组被整体复制于用户态和内核地址空间之间，而不管这样的复制是不是有意义。  poll还有一个特点是“水平触发LT”，如果报告了fd后，没有被处理，那么下次poll时会再次报告该fd。 |
| epoll  | 在前面说到的复制问题上，epoll使用mmap减少复制开销。  边缘触发ET，  提高了编程的复杂程度  还有一个特点是，epoll使用“事件”的就绪通知方式，通过epoll_ctl注册fd，一旦该fd就绪，内核就会采用类似callback的回调机制来激活该fd，epoll_wait便可以收到通知 |

| 手段   | 一个进程能打开的最大连接数                                   |
| ------ | ------------------------------------------------------------ |
| select | 单个进程所能打开的最大连接数有FD_SETSIZE宏定义，其大小是32个整数的大小（在32位的机器上，大小就是32*32，同理64位机器上FD_SETSIZE为32*64），当然我们可以对进行修改，然后重新编译内核，但是性能可能会受到影响，这需要进一步的测试。 |
| poll   | poll本质上和select没有区别，但是它没有最大连接数的限制，原因是它是基于链表来存储的 |
| epoll  | 虽然连接数有上限，但是很大，1G内存的机器上可以打开10万左右的连接，2G内存的机器可以打开20万左右的连接  (cat /proc/sys/fs/file-max) 通过命令查看 |

![image-20220624110735819](https://gogogxg.xyz/img/20220624-170342-793.png)

### 1. select

```cc
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);
/*nfds:监控的文件描述符集里最大文件描述符+1，因为此参数会告诉内核检测前多少个文件描述符的状 态。 readfs/writes/exceptfds:监控有读数据/写数据/异常发生到达文件描述符集合，三个都是传入传 出参数。
timeout:定时阻塞监控时间，3中情况:
1、NULL，永远等下去
2、设置timeval，等待固定时间 3、设置timeval里时间均为0，检查描述字后立即返回，轮询。
fd_set:本质是个位图。 
*/
struct timeval
{
};
long    tv_sec;
long    tv_usec;
/* seconds */
/* microseconds */
返回值: 成功:所监听的所有的监听集合中，满足条件的总数。 失败:返回-1.
```

#### 函数

```cc
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

#### 用法

客户端与服务器连接的读事件，1、客户端请求与服务器进行连接的事件   2、客户端发送数据给了服务器，此时服务器需要读这个数据



### 2. poll

#### 图解

![image-20220624101132513](https://gogogxg.xyz/img/20220624-170149-861.png)

#### 函数原型

```cc
#include <poll.h>
int poll(struct pollfd *fds,nfds_t nfds,int timeout);

struct pollfd 
{
	int   fd;	/* file descriptor */
	short events;	/* requested events */
	short revents;	/* returned events */
};
```



### 3. epoll

#### 图解

![image-20220624101243421](https://gogogxg.xyz/img/20220624-170149-861.png)



#### 函数原型

```cc
#include <sys/epoll.h>
int epoll_create(int size);
int epoll_create1(int flags);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

typedef union epoll_data {
	void    *ptr;
	int      fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event {
	uint32_t     events;	/* Epoll events */
	epoll_data_t data;	/* User data variable */
};

```

#### 逻辑

```cc
select/poll/epoll

listenfd       select   监听

//listenfd新的连接请求的标志
if(listenfd == 读事件)
{
    //有新的连接请求进来
    connfd = accept();
   //只要connfd是正常的
}

//之前老的连接上面有新的数据传送过来，write
if(connfd == 读事件)
{
    n = read;
    if(n > 0)
    {
        //数据能正常的接收
        //全部转换为大写
        //write回给客户端
    }
    else if(n == 0)
    {
        //客户端要断开连接
        //close(connfd);
    }
    else if(n == -1)//异常
    {
        close;
        return ;
    }
}

listenfd
    
nready = select/poll;
nready = 5;//有五个读事件满足条件
if(读事件 == listenfd)//有新的连接请求上来
{
    confd = accept;
    confd存在数据结构里面
}

if(读事件 == confd)//老客户端有新的需要读数据传过来，
{
    n = read;
    if(n > 0)
    {
        //数据能正常的接收
        //全部转换为大写
        //write回给客户端
    }
    else if(n == 0)
    {
        //客户端要断开连接
        //close(connfd);
    }
    else if(n == -1)//异常
    {
        close;
        return ;
    }
    
}
```

## 并发服务器方案

### 定义

Reactor模式，又叫反应器模式，应用于同步I/O的场景。以读操作为例来看看Reactor中的具体步骤：
读取操作：

1. 应用程序注册读就绪事件和相关联的事件处理器
2. 事件分离器等待事件的发生
3. 当发生读就绪事件的时候，事件分离器调用第一步注册的事件处理器
4. 事件处理器首先执行实际的读取操作，然后根据读取到的内容进行进一步的处理

> #### 常见并发服务器方案
>
> 1. **循环式/迭代式( iterative )服务器**
>   无法充分利用多核CPU，不适合执行时间较长的服务
> 2. **并发式(concurrent)服务器**
>   one connection per process/one connection per thread
>   适合执行时间比较长的服务
> 3. **prefork or pre threaded（UNP2e 第27章）**
> 4. **反应器( reactive )服务器 (reactor模式)**
>   并发处理多个请求，实际上是在一个线程中完成。无法充分利用多核CPU
>   不适合执行时间比较长的服务，所以为了让客户感觉是在“并发”处理而不是“循环”处理，每个请求必须在相对较短时间内执行。
> 5. **reactor + thread per request（过渡方案）**
> 6. **reactor + worker thread（过渡方案）**
> 7. **reactor + thread pool（能适应密集计算）**
> 8. **multiple reactors（能适应更大的突发I/O）**
>   reactors in threads（one loop per thread）
>   reactors in processes
> 9. **multiple reactors + thread pool**
>    （one loop per thread + threadpool）（突发I/O与密集计算）
> 10. **proactor服务器(前摄器模式、主动器模式，基于异步I/O)**
>   理论上proactor比reactor效率要高一些
>   异步I/O能够让I/O操作与计算重叠。充分利用DMA特性。
>   Linux异步IO
>   glibc aio（aio_*），有bug，基本上不用
>   kernel native aio（io_*），也不完美。目前仅支持 O_DIRECT 方式来对磁盘读写，跳过系统缓存。要自已实现缓存，难度不小。
>   boost.asio实现的proactor，实际上不是真正意义上的异步I/O，底层是用epoll来实现的，模拟异步I/O的。

### 图解

![image-20220624103352267](https://gogogxg.xyz/img/20220624-170149-861.png)

![image-20220624103422491](https://gogogxg.xyz/img/20220624-170149-861.png)

![image-20220624103444053](https://gogogxg.xyz/img/20220624-170149-861.png)

![image-20220624103521511](https://gogogxg.xyz/img/20220624-170149-861.png)

![image-20220624104026848](https://gogogxg.xyz/img/20220624-170149-861.png)

## 组件

###  read/write和recv/send区别

```cc
int recv(int sockfd,void *buf,int len,int flags);
int send(int sockfd,void *buf,int len,int flags);
```

p前面的三个参数和read,write相同，第四个参数能够是0或是以下的组合

- MSG_DONTROUTE--不查找路由表 
- MSG_OOB--接受或发送带外数据 
- MSG_PEEK--查看数据,并不从系统缓冲区移走数据
- MSG_WAITALL--等待任何数据

p如果flags为0，则和read,write一样的操作

p在linux中，所有的设备都可以看成是一个文件，所以可以用read/write来读写socket数据。



### Eventfd —— Reactor与线程池融合的组件

从Linux 2.6.27版本开始，新增了不少系统调用，其中包括eventfd，它的主要是用于进程或者线程间通信(如通知/等待机制的实现)。

特点：可以在进程或线程间进行通信。

eventfd返回一个文件描述符，evtfd，A线程与B线程如果都可以看到evtfd。A线程取进行read操作，B线程进行write操作

函数接口

```C++
#include <sys/eventfd.h>
int eventfd(unsigned int initval, int flags);
//initval:计数器的值，由内核进行维护。
//flag：早期版本（2.6.26）之前直接设置0，2.6.26版本之后设置为其他值。
```

initval：初始化计数器值，该值保存在内核。

flags：如果是2.6.26或之前版本的内核，flags 必须设置为0。

flags支持以下标志位：

- EFD_NONBLOCK      类似于使用O_NONBLOCK标志设置文件描述符。
- EFD_CLOEXEC  类似open以O_CLOEXEC标志打开， O_CLOEXEC 应该表示执行exec()时，之前通过open()打开的文件描述符会自动关闭.

返回值：函数返回一个文件描述符，与打开的其他文件一样，可以进行读写操作。

A进程对应是父进程，父进程进行read操作。会读evtfd文件描述符对应的数据，并且将该数据清空为0。

B进程对应是子进程，子进程进行warite操作，write可以将evtfd对应的内核维护的数据进行累加操作。

##### 支持的操作

- read：如果计数器A的值不为0时，读取成功，获得到该值。如果A的值为0，非阻塞模式时，会直接返回失败，并把error置为EINVAL;如果为阻塞模式，一直会阻塞到A为非0为止。
- write：将缓冲区写入的8字节整形值加到内核计数器上，即会增加8字节的整数在计数器A上，如果其值达到0xfffffffffffffffe时，就会阻塞（在阻塞模式下），直到A的值被read。
- select/poll/epoll：支持被io多路复用监听。当内核计数器的值发生变化时，就会触发事件

### Timerfd

timerfd是Linux提供的一个定时器接口。这个接口基于文件描述符，通过文件描述符的可读事件进行超时通知，所以能够被用于select/poll/epoll的应用场景。timerfd是linux内核2.6.25版本中加入的接口。

```cc
#include <sys/timerfd.h>

int timerfd_create(int clockid, int flags);
/*
功能：该函数生成一个定时器对象，返回与之关联的文件描述符。
参数详解：
clockid:可设置为
    CLOCK_REALTIME：相对时间，从1970.1.1到目前的时间。更改系统时间 会更改获取的值，它以系统时间为坐标。
    CLOCK_MONOTONIC：绝对时间，获取的时间为系统重启到现在的时间，更改系统时间对齐没有影响。
flags: 可设置为
    TFD_NONBLOCK（非阻塞），
    TFD_CLOEXEC（同O_CLOEXEC）
    linux内核2.6.26版本以上都指定为0
*/

int timerfd_settime(int fd, int flags,const struct itimerspec *new_value, struct itimerspec *old_value);
struct timespec {
    time_t tv_sec;                /* Seconds */
    long   tv_nsec;               /* Nanoseconds */
};

struct itimerspec {
    struct timespec it_interval;  /* Interval for periodic timer */
    struct timespec it_value;     /* Initial expiration */
};
/*
功能：该函数能够启动和停止定时器
参数详解：
fd: timerfd对应的文件描述符
flags: 
0表示是相对定时器
TFD_TIMER_ABSTIME表示是绝对定时器
new_value:
	设置超时时间，如果为0则表示停止定时器。
old_value:
    一般设为NULL, 不为NULL,
    则返回定时器这次设置之前的超时时间

*/

int timerfd_gettime(int fd, struct itimerspec *curr_value);
/*
timerfd_gettime() returns, in curr_value, an itimerspec structure that contains the current setting of the timer referred to by the file descriptor fd.

The  it_value  field  returns  the amount of time until the timer will next expire.  If both fields of this
structure are zero, then the timer is currently disarmed.  This field always contains a relative value, re‐
gardless of whether the TFD_TIMER_ABSTIME flag was specified when setting the timer.
The  it_interval  field returns the interval of the timer.  If both fields of this structure are zero, then
the timer is set to expire just once, at the time specified by curr_value.it_value.
*/
```

##### 支持操作

read：读取缓冲区中的数据，其占据的存储空间为sizeof(uint64_t)，表示超时次数。

select/poll/epoll：当定时器超时时，会触发定时器相对应的文件描述符上的读操作，IO复用操作会返回，然后再去对该读事件进行处理。

### UTF-8

![image-20220624105843559](https://gogogxg.xyz/img/20220624-170149-861.png)

![image-20220624105921993](https://gogogxg.xyz/img/20220624-170149-861.png)

## 封装

代码详见`Reactor模板`文件夹

### IO

1、InetAddress：网络地址类，负责所有的地址相关的操作，获取ip地址，获取端口号，strcut sockaddr_in

2、Socket：套接字类，所有的与套接字相关的，都可以用这个类。

3、Acceptor：连接器类，将所有的，端口复用、listen、bind、accept

4、TcpConnection：TCP连接类，该对象创建完毕，就表名三次握手己经建立完毕，该连接就是一个TCP连接，该连接就可以进行发送数据与接收数据

5、SocketIO：真正进行数据发送与接收的类。

![image-20220621175638386](https://gogogxg.xyz/img/20220624-170149-861.png)



recv（xx，xx，xx，MSG_PEEK）发生拷贝操作，但不会从缓冲区中移除数据

#### Tcp通信过程中的三个半事件

连接建立：包括服务器端被动接受连接（accept）和客户端主动发起连接（connect）。TCP连接一旦建立，客户端和服务端就是平等的，可以各自收发数据。

连接断开：包括主动断开（close、shutdown）和被动断开（read()返回0）。

消息到达：**文件描述符可读**。这是最为重要的一个事件，对它的处理方式决定了网络编程的风格（阻塞还是非阻塞，如何处理分包，应用层的缓冲如何设计等等）。

消息发送完毕：这算半个。对于低流量的服务，可不必关心这个事件；另外，这里的“发送完毕”是指数据写入操作系统缓冲区（内核缓冲区），将由TCP协议栈负责数据的发送与重传，不代表对方已经接收到数据。

EventLoop对应的线程：就是负责数据的收发，也就是基本IO操作，**IO线程**

ThreadPool线程池对应的线程：去处理正常的业务逻辑，处理编解码、计算的。**计算线程**



> #### 回调函数
>
> C语言  回调函数
>
> ```C++
> int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
>                           void *(*start_routine) (void *), void *arg);
> 
> void *threadFunc(void *arg)
> {
>     //
> }
> 
> //C语言  中的结构体可以一次存大量数据
> //void *
> pthread_create(&thid, nullptr, threadFunc, arg);//注册threadFunc回调函数
> 
> //C语言中有个函数指针，有了结构体
> void (*pFunc)();
> 
> pFunc = add;
> pFunc = func;
> ```
>
> C++中的多态：类、虚函数、继承
>
> C中：struct、函数指针

### 线程池

#### 类图

![image-20220620180025724](https://gogogxg.xyz/img/20220624-170149-861.png)

#### 代码实现

1、如果保证任务队列TaskQueue中的任务全部都执行完毕？

![image-20220620145406921](https://gogogxg.xyz/img/20220624-170149-861.png)

2、如何将线程池退出来？

工作线程执行getTask函数，如果执行的速度过慢，任务是可以执行完毕的，并且也可以退出；但是如果执行的速度非常快，就会卡在ThreadPool中的threadFunc中，也就是其中的getTask函数，也就是TaskQueue的pop方法上面，也就是卡在`_notEmpty`条件变量的wait上面，所以可以让这些线程不用睡眠，直接唤醒。可以使用notifyAll函数。

![image-20220620173058286](https://gogogxg.xyz/img/20220624-170149-861.png)

但是又会有一个问题，虽然唤醒了子线程，但是判断条件仍然是while，所以需要进一步修改。

![image-20220620173356896](https://gogogxg.xyz/img/20220624-170149-861.png)



#### 2、基于对象的线程池

#### 类图

![image-20220624111138503](https://gogogxg.xyz/img/20220624-170149-861.png)

基于对象的时候，bind绑定到成员函数的时候，可以使用值传递，也可以使用地址传递

![image-20220624111148624](https://gogogxg.xyz/img/20220624-170149-861.png)

```C++
void func(int x1, int x2, int x3, const int &x4, int x5)
{
    
}
int number = 10;
autpo f = bind(func, 1, _2, _1, std::cref(number), number);//值传递
number = 20;

f(1, 2, 3, 4, 5,6);
```

