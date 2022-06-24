## 计算机网络

### 1、网络模型

OSI七层模型、TCP/IP四层（五层）模型

![image-20220621090232880](https://gogogxg.xyz/img/20220624-170637-979.png)

### 2、每一层数据包的名字

![image-20220621090305184](https://gogogxg.xyz/img/20220624-170725-601.png)

### 3、每一层的协议格式

以太网帧格式、IP段格式、TCP数据报格式、UDP数据报格式

### 4、TCP协议（重点）

TCP协议是一个传输层的协议；TCP面向连接的协议；TCP是一个可靠的协议；TCP是全双工的协议。

**TCP的建立连接的三次握手、断开连接的四次挥手？SYN、ACK、FIN（这个是面试常考点）**

相应的文字解析，希望自己可以看看课件，自己好好复习一下。

![image-20220621091051593](https://gogogxg.xyz/img/20220624-170728-262.png)

### 5、TCP状态迁移图（重点）

记住其中的11个状态；以及使用代码验证这部分状态。

可以使用nc命名

```bash
nc ip   port

nc 127.0.0.1 8888
```

![image-20220621090827235](https://gogogxg.xyz/img/20220624-170932-001.png)

**半关闭状态、2MSL？**

### 6. 散知识

网络字节序、主机字节序、大端、小端

### 7.  网络编程函数

#### 结构体

```cc
struct sockaddr {
	sa_family_t sa_family;
    char sa_data[14];
};

struct sockaddr_in {
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
};

struct in_addr {
    uint32_t s_addr;
};
```

#### 函数

socket、bind、listen、accept、connect、close

##### 服务器

```C++
int lfd = socket;
bind;
listen;//服务器处于监听状态
int cfd = accept;//cfd有正常值的时候，证明三次握手已经完成（TCP连接已经建立）

while(1)
{
    //服务器真正的业务逻辑
    read;
    //
    //....
    //
    write;
}

close(lfd);//四次挥手
close(cfd);
```

##### 客户端

```C++
int lfd = socket;
connect;
while(1)
{
    //客户端的业务逻辑
     read;
    //
    //....
    //
    write;
}

close(lfd);//四次挥手
```

![image-20220621102555658](https://gogogxg.xyz/img/20220624-170736-185.png)

#### 端口复用

```C++
int opt = 1;
setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
int opt = 1;
setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
```
