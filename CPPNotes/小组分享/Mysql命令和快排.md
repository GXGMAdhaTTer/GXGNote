# Mysql

```sql
//事务操作常用命令
create database if not exists mydb12_transcation;
use mydb12_transcation;
-- 创建账户表
create table account(
	id int primary key, -- 账户id
	name varchar(20), -- 账户名
	money double -- 金额
);

--  插入数据
insert into account values(1,'zhangsan',1000);
insert into account values(2,'lisi',1000);

-- 设置MySQL的事务为手动提交(关闭自动提交)
select @@autocommit;
set autocommit = 0;

-- 模拟账户转账
-- 开启事务 
begin;
update account set money = money - 200 where name = 'zhangsan';
update account set money = money + 200 where name = 'lisi';
-- 提交事务
commit;

-- 回滚事务
rollback;

select * from account;


-- 查看隔离级别 
show variables like '%isolation%';

-- 设置隔离级别
/*
set session transaction isolation level 级别字符串
级别字符串：read uncommitted、read committed、repeatable read、serializable
	
*/
-- 设置read uncommitted
set session transaction isolation level read uncommitted;
-- 这种隔离级别会引起脏读，A事务读取到B事务没有提交的数据

-- 设置read committed
set session transaction isolation level read committed;
-- 这种隔离级别会引起不可重复读，A事务在没有提交事务之前，可看到数据不一致


-- 设置repeatable read （MySQ默认的）
set session transaction isolation level repeatable read;
-- 这种隔离级别会引起幻读，A事务在提交之前和提交之后看到的数据不一致

-- 设置serializable
set session transaction isolation level serializable;
-- 这种隔离级别比较安全，但是效率低，A事务操作表时，表会被锁起，B事务不能操作。

-- SQL的优化

insert into account values(3,'wangwu',1000);
-- 查看当前会话SQL执行类型的统计信息
show session status like 'Com_______';

-- 查看全局（自从上次MySQL服务器启动至今）执行类型的统计信息
show global status like 'Com_______';

-- 查看针对InnoDB引擎的统计信息
show status like 'Innodb_rows_%';

-- 查看慢日志配置信息
show variables like '%slow_query_log%';

-- 开启慢日志查询
set global slow_query_log = 1;

-- 查看慢日志记录SQL的最低阈值时间,默认如果SQL的执行时间>=10秒，则算慢查询，则会将该操作记录到慢日志中去
show variables like '%long_query_time%';
select sleep(12);
select sleep(10);


-- 修改慢日志记录SQL的最低阈值时间

set global long_query_time = 5;

-- 通过show processlist查看当前客户端连接服务器的线程执行状态信息

show processlist;

select sleep(50);

-- ----------------explain执行计划-------------------------
create database mydb13_optimize;
use mydb13_optimize;

-- 1、查询执行计划 
explain  select * from user where uid = 1;

-- 2、查询执行计划
explain  select * from user where uname = '张飞';

-- 2.1、id 相同表示加载表的顺序是从上到下
explain select * from user u, user_role ur, role r where u.uid = ur.uid and ur.rid = r.rid ;

-- 2.2、 id 不同id值越大，优先级越高，越先被执行。 
explain select * from role where rid = (select rid from user_role where uid = (select uid from user where uname = '张飞'))
-- 2.3/
explain select * from role r ,
(select * from user_role ur where ur.uid = (select uid from user where uname = '张飞')) t where r.rid = t.rid ; 

```

# MySQL的binlog日志查询

```sql
-- 查看MySQL是否开启了binlog日志
show variables like 'log_bin';


-- 查看binlog日志的格式
show variables like 'binlog_format';

-- 查看所有日志
show binlog events;

-- 查看最新的日志
show master status;

-- 查询指定的binlog日志
show binlog events in 'binlog.000010';
select * from mydb1.emp2;
select count(*) from mydb1.emp2;
update mydb1.emp2 set salary = 8000;


-- 从指定的位置开始,查看指定的Binlog日志
show binlog events in 'binlog.000010' from 156;


-- 从指定的位置开始,查看指定的Binlog日志,限制查询的条数
show binlog events in 'binlog.000010' from 156 limit 2;
--从指定的位置开始，带有偏移，查看指定的Binlog日志,限制查询的条数
show binlog events in 'binlog.000010' from 666 limit 1, 2;

-- 清空所有的 binlog 日志文件
reset master

-- 查看MySQL是否开启了查询日志
show variables like 'general_log';

-- 开启查询日志
set global  general_log=1;

select * from mydb1.emp2;
select * from mydb6_view.emp;

select count(*) from mydb1.emp2;
select count(*) from mydb6_view.emp;
update mydb1.emp2 set salary = 9000;


-- 慢日志查询

-- 查看慢查询日志是否开启
show variables like 'slow_query_log%';

-- 开启慢查询日志
set global slow_query_log = 1;

-- 查看慢查询的超时时间
show variables like 'long_query_time%';


select sleep(12);



```

# 快排

```c++
1.快速排序的思想、时间复杂度、实现以及优化方法？
快速排序的三个步骤
(1)选择基准：在待排序列中，按照某种方式挑出一个元素，作为’基准(pivot);
(2)分割操作：以该基准在序列中的实际位置，把序列分成两个子序列。此时，在基准 左边的元素都比该基准小，在基准右边的元素都比基准大：
(3)递归地对两个序列进行快速排序，直到序列为空或者只有一个元素。

基准的选择:
    对于分治算法，当每次划分时，算法若都能分成两个等长的子序列时，那么分治算法效率会达到最大。
    即：同一数组，时间复杂度最小的是每次选取的基准都可以将序列分为两个等长的；时间复杂度最大的是每次选择的基准都是当前序列的最大或最小元素；
    快排代码实现：
    我们一般选择序列的第一个作为基数，那么快排代码如下：

注：上述数组或序列V必须是引用类型的形参，因为后续快排结果需要直接反映在原序列中；

=====================    
上述快排的基数是序列的第一个元素，这样的对于有序序列，快排时间复杂度会达到最差
所以，优化方向就是合理的选择基数。
    最好的情况：每次选的pivot（基准值）几乎能把数据均分成两半，这样递归树的深度就是logN，这样快排的时间复杂度为O(NlogN)

最坏的情况：每次找的pivot将数组分成两部分，其中有一部分是空，这样递归树就变成了一棵倾斜的树。树的深度为n-1,这样时间复杂度就变成了O(N^2).(有说退化成冒泡的)
一般当数据有序或者局部有序的时候会出现这种坏的情况，比如数组正序或者逆序，（数字完全相同的时候也是有序的特殊情况）。

========================

优化方式一、   
常见的做法“三数取中法(序列太短还要结合其他排序法，如插入排序、选择排序等), 如下：
    ①当序列区间长度小于7时，采用插入排序：
    ②当序列区间长度小于40时，将区间分成2段，得到左端点、右端点和中点，我们对 这三个点取中数作为基数；
    ③当序列区间大于等于40时，将区间分成8段，得到左三点、中三点和右三点，分 别再得到左三点中的中数、中三点中的中数和右三点中的中数，再将得到的三个中数取 中数，然后将该值作为基数。

具体代码只是在原来的快排代码中将“基数赋值"改为①②③对应的代码即可：
        
    int key=v[left];//基数赋值
    if (right-left+l<=7)
    {
        insertion_sort (v, left, right) ; //插入排序
        return;
    }else if (right-left+l<=8) 
    {
        key = SelectPivotOfThree (v, left, right); //三个取中
    }
    else{
    //三组三个取中，再三个取中(使用4次SelectPivotOfThree，此处不具体展示) //没看完
    }
需要调用的函数：
void insertion_sort (vector<irrt> &unsotrted, irrt left, irrt right) 
        //插入排序算法 
       {
            for (int i = left+1: i <= right: i++)
            {
                if (unsorted[i - 1] > unsorted[i])
                int temp = unsorted[i]:
                int j = i;
                while (j > left && unsorted[j - 1] > temp) 
                {    
                    unsorted[j] = unsorted[j - 1]:
                    unsorted [j] = tenp:             //代码可能有问题
                }
             }
        }
int SelectPivotOfThree (vector<int> &arr, int low, irrt high)
//三数取中，同时将中值移到序列第一位
{
    int mid = low + (high - low)/2;   //计算数组中间的元素的下标
        //使用三数取中法迭择枢轴
    if (arr[mid] > arr[high])//目标：arr[mid] <= arr[high]
    {
        swap (arr [mid], arr [high]):
    }
    if (arr[low] > arr[high])//目标：arr[low] <= arr[high]
    {
        swap (arr [low], arr [high]):
    }
    if (arr [mid] > arr[low])//目标：arr[low] >= arr[mid]
    {
        swap (arr [mid], arr[low]):
    }
    //此时，arr [mid] <= arr[low] <= arr [high]
    return arr[low]：
        //low的位置上保存这三个位置中间的值
        //分割时可以直接使用low位置的元素作为枢轴，而不用改変分割函数了
}
这里需要注意的有两点：
①插入排序算法实现代码；
②三数取中函数不仅仅要实现取中，还要将中值移到最低位，从而保证原分割函数依然 可用。

            


```

# 优化算法2、
    知乎网址
    
    https://zhuanlan.zhihu.com/p/139056506
    //第一个===》作者自写的  结合直接插入算法（效率略优于sort）
    //第二个===》多线程处理快排的算法参考（与递归的总层数有关）



