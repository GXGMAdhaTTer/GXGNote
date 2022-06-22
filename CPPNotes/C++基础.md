# C++基础

## C++概念



### 命名空间

匿名空间中的实体只能在本模块内部使用，无法跨模块使用，功能与static变量相同 。

全局变量可以跨模块调用

同一个文件、不同模块中可以多次定义命名空间。



### const 关键字

不变的，修饰类型，不能再修改变量的值。

> #### 宏定义与const常量有什么区别
>
> 宏定义是常量表达式，在预处理阶段对代码进行简单的字符串替换，没有类型替换，
> const关键字修饰变量在编译时进行处理，有类型检查。
>
> 宏定义在编译阶段若没有报错，会将出错时机延迟到运行时，更难发现。
>
> #### 什么是**常量指针**和**指针常量**？什么是**数组指针**和**指针数组**？什么是**函数指针**和**指针函数**？请举例说明。
>
> ```c++
> 常量指针（pointer to const）
> 	const int* p1 = &a;
> 	int const* p2 = &a;
> 	不能修改的是指针所指的值
> 指针常量（const pointer）
> 	int* const p3 = &a;
> 	不能修改指针的指向
> 常量指针常量？
> 	const int* const p4 = &a;
> 	两者都不能修改
> 
> 数组指针（array pointer）
> 	int *p = &p[0];//指向数组第一个元素的指针
> 指针数组（pointer array）
> 	char* s[10];//含有10个元素的数组，每个元素是指针
> 
> 函数指针（function pointer）
> 	int (*p)(int) = &func;//指向函数的指针
> 指针函数（pointer function）
>  char* strcpy(char* dest, const char* src);//返回值是指针的函数
> ```
>

### new/delete 表达式



> #### 请你来回答一下new和malloc的区别
>
> （1）new分配内存按照数据类型进行分配，malloc分配内存按照指定的大小分配；
> （2）new返回的是指定对象的指针，而malloc返回的是void*，因此malloc的返回值一般都需要进行类型转化。
> （3）new不仅分配一段内存，而且会调用构造函数，malloc不会。
> （4）new分配的内存要用delete销毁，malloc要用free来销毁；delete销毁的时候会调用对象的析构函数，而free则不会。
> （5）new是一个操作符可以重载，malloc是一个库函数。
> （6）malloc分配的内存不够的时候，可以用realloc扩容。扩容的原理？new没用这样操作。
> （7）new如果分配失败了会抛出bad_malloc的异常，而malloc失败了会返回NULL。
> （8）申请数组时： new[]一次分配所有内存，多次调用构造函数，搭配使用delete[]，delete[]多次调用析构函数，销毁数组中的每个对象。而malloc则只能sizeof(int) * n。
>
> #### 请你说一说内存溢出和内存泄漏
>
> （1）**内存溢出**
> 指程序申请内存时，没有足够的内存供申请者使用。内存溢出就是你要的内存空间超过了系统实际分配给你的空间，此时系统相当于没法满足你的需求，就会报内存溢出的错误
> 内存溢出原因：
> 内存中加载的数据量过于庞大，如一次从数据库取出过多数据集合类中有对对象的引用，使用完后未清空，使得不能回收
> 代码中存在死循环或循环产生过多重复的对象实体使用的第三方软件中的BUG启动参数内存值设定的过小
>
> （2）**内存泄漏**
> 内存泄漏是指由于疏忽或错误造成了程序未能释放掉不再使用的内存的情况。内存泄漏并非指内存在物理上的消失，
> 而是应用程序分配某段内存后，由于设计错误，失去了对该段内存的控制，因而造成了内存的浪费。
>
> **内存泄漏的分类：**
> （1）堆内存泄漏 （Heap leak）。对内存指的是程序运行中根据需要分配通过malloc,realloc new等从堆中分配的一块内存，
> 再是完成后必须通过调用对应的 free或者delete 删掉。如果程序的设计的错误导致这部分内存没有被释放，
> 那么此后这块内存将不会被使用，就会产生Heap Leak。
> （2）系统资源泄露（Resource Leak）。主要指程序使用系统分配的资源比如 Bitmap,handle ,SOCKET等没有使用
> 相应的函数释放掉，导致系统资源的浪费，严重可导致系统效能降低，系统运行不稳定。
> （3）没有将基类的析构函数定义为虚函数。当基类指针指向子类对象时，如果基类的析构函数不是virtual，那么子类
> 的析构函数将不会被调用，子类的资源没有正确是释放，因此造成内存泄露
>
> 野指针就是指向一个已删除的对象或者未申请访问受限内存区域的指针
>
> #### malloc底层实现
>
> ？？？





### 引用

在实现上，在内存上占大小；在逻辑上，可以认为引用不占空间，它代表被绑定的对象。

引用底层实现：还是指针，一个受限制的指针，会占据大小，也会分配一个指针大小的空间，一旦绑定不能改变。

好处：没有复制的开销，可以提高程序执行效率，更直观，减少犯错概率。

引用作为函数的返回值有一个前提条件，返回的变量本身的生命周期要大于函数，不要返回一个局部变量的引用。不要轻易返回一个堆空间变量的引用，除非已有内存回收的策略。

> #### 引用与指针的区别是什么？"引用"作为函数参数有哪些特点？在什么时候需要使用"常引用"
>
> 指针是实体，占用空间，可以改变指向的对象（非常量指针），可以独立存在也可以不初始化。
> 引用是给变量起别名，在使用时操作引用就相当于操作变量本身，不占用空间，不能改变引用的对象，没有复制的开销，可以提高程序执行效率，不能独立存在，必须绑定到一个对象。
> 特点：更直观。
> 常引用：既要利用引用提高程序的效率,又要保护传递给函数的数据不在函数中被改变, 就应使用常引用。
> const 类型名& 引用名=目标变量名;



### C++强制转换

static_cast/const_cast/dynamic_case/reinterpret_cast(重新解释)



```
static_cast 常见的指针转换
const_cast 去除常量属性
dynamic_cast 只用在多态时，基类与派生类之间转换


int* p = static_cast<int*>(malloc(sizeof(int)));
```



### 函数重载

函数名相同，但参数列表不同，函数参数、类型、个数、顺序不同。

C语言不支持函数重载

实现原理：名字改编（name mangling）

具体步骤：当函数名相同时，会根据函数名

```
gcc -c overload.c
nm overload.o
```



C++需要C的库，必须要能够按照C的方式进行调用，因为成本太高

希望用C++的编译器来编译C的源码，但必须要采用C的方式调用。

C++需要适应C的调用方式。

希望C和C++的混合编程

```
解决方案：

#ifdef __Cplusplus
extren "C" {
#endif

#ifdef __Cplusplus
}
#endif
```

> 默认参数的设置要求必须从右到左进行。
>
> #### 什么是`inline`函数？`inline`与带参数的宏定义之间的区别是什么？
>
> c语言中，如果一些函数比较短且频繁调用，为了消除函数调用开销都影响，一般采用宏函数（即带参数的宏定义）；在C++中使用inline函数，效率上与宏定义一致，但是更安全。
> 宏定义的进行时在预处理阶段，而inline函数是在编译时替换。
> inline的使用有要求，使用inline时，函数的定义只能放在头文件中，不能拆分声明与实现。











##### C++标准库的头文件位置

`usr/include/c++/7.5.0`

##### 对于输入的健壮实现

```cc
int readIntegerNumber() {
	int number = -1;
    cout << ">>Please input a valid interger number: " << endl;
    while (cin >> number, !cin.eof()) {
        if (cin.bad()) {
            cout << "cin has broken!" << endl;
            return -1;
        } else if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << ">> Please input a valid interger number: " << endl;
            continue;
        } else {
            cout << "number: " << number << endl;
            return number;
        }
    }
}
```

##### 读取配置文件的函数

```cc
void readConfiguration() {
    string filename("test.txt");
    ifstream ifs(filename);
    if(!ifs) {
        cout << "ifstream open file error" << endl;
        return;
    }
    string line;
    string key, value;
    while(getline(ifs, line)) {
        istringstream iss(line);
        iss >> key >> value;
        cout << keey << "-->" << value << endl;
    }
    ifs.close();
    
}
```

> union类型
>
> ```cc
> #include <iostream>
> #include <cstdint>
> union S
> {
>     std::int32_t n;     // occupies 4 bytes
>     std::uint16_t s[2]; // occupies 4 bytes
>     std::uint8_t c;     // occupies 1 byte
> };                      // the whole union occupies 4 bytes
>  
> int main()
> {
>     S s = {0x12345678}; // initializes the first member, s.n is now the active member
>     // at this point, reading from s.s or s.c is undefined behavior
>     std::cout << std::hex << "s.n = " << s.n << '\n';
>     s.s[0] = 0x0011; // s.s is now the active member
>     // at this point, reading from n or c is UB but most compilers define it
>     std::cout << "s.c is now " << +s.c << '\n' // 11 or 00, depending on platform
>               << "s.n is now " << s.n << '\n'; // 12340011 or 00115678
> }
> 
> /*
> s.n = 12345678
> s.c is now 0
> s.n is now 115678
> */
> ```
>
> 



