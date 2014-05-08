12.awk学习， awk 'program' data  ||||or |||| data | awk 'program'


====================
====================
====================
====================
====================

1.MySQL表锁的操作语句。  select * from table where id = xx for update;   //for update 就是表锁的关键字。

2.go的接口编程方式：
在一个接口go文件中，定义接口
在多个实现go文件中，实现该接口的编程方式。

3.go bytes包，做一些字符串匹配的操作，针对byte数组。 对于string可用strconv

4.val = bytes.Trim(val, `"`) //针对引号

5.c++返回引用的函数也要用引用变量接受。  ptr &getxxx();  
			ptr &_ptr = getxxx();

6.垃圾回收器，是基于图（从一个节点一定可以遍历完所有的in
use节点。），这样就可以将所有不在图内的节点删除。				mark &
clear，先标记再清楚

7.boost::ptime使用

8.关于Log

9.node.js的事件监听器模式是一种事件钩子的机制。

10.获取C++
vector中元素的地址会出现奇怪的现象，获得一个位置地址的垃圾对象。因为它返回的就是引用，而引用虽然是对象本身的别名但是取引用的地址并不能得到对象的地址


====================
====================
====================
====================
===================

1.trie树，重新学习
http://www.instapaper.com/read/442680831
考虑空间与时间的复杂度而言，主要是实现双数组的形势来存储当前节点已经其多个子节点。

2.C++/C中，对于存数组int t[];的复制操作时, 是对t.size()敏感的。  如int t[1];
int p[2];   t = p;是不能通过编译的。

3.python	------>		python中如果子类有自己的构造函数，不会自动调用父类的构造函数，如果需要用到父类的构造函数，则需要在子类的构造函数中显式的调用。

子类从多个父类派生，而子类又没有自己的构造函数时，

（1）按顺序继承，哪个父类在最前面且它又有自己的构造函数，就继承它的构造函数；

（2）如果最前面第一个父类没有构造函数，则继承第2个的构造函数，第2个没有的话，再往后找，以此类推。(如果都没有就继续往上找--->object对象)


4.C语言 struct的内存布局(需要注意的是内存以结构体中的最大步长为单位,
		1、2、4、8bytes以此类推)
//在64bit的机器上
struct foo6 {
    char c;           /* 1 byte*/
    char pad1[7];     /* 7 bytes */
    struct foo6_inner {
        char *p;      /* 8 bytes */
        short x;      /* 2 bytes */
        char pad2[6]; /* 6 bytes */
    } inner;
};
24bytes的数据浪费了13bytes

5.what is Monads in function language, monads is a structure that represents
computations defined as sequence of steps.
是一种按步骤进行操作的一种程序表现形式（例如pipeline）



6.Y combinator(7步实现一个简单的Y结合子)
			（1）简单的递归
			（2）显示递归（函数自己调用自己---无限循环，栈溢出）
			（3）有条件的显示递归、并包装成一个函数
			var fact = (function(f){
							return function(n){
								if (n < 2) return 1;
								return n * f(f)(n - 1);
							};
						})(function(f){
								return function(n){
									if (n < 2) return 1;
									return n * f(f)(n - 1);
								};
							});
			（4）减少代码的重复
			var recur = function(f){ return f(f); };
			这样就减少了重复代码量。
			
			（5）如何消除双重调用f(f)？
			把f(f)(n - 1)看成一个整体，提取出一个function g;
			g = function(n){ return f(f)(n); }
			然后替换。

			（6）将代码进一步封装 (这一步的封装时重点，也比较难懂)
			var wrap = function(h){
				return recur(function(f){
						var g = function(n){
							return f(f)(n);
						};
						return h(g);
				});
			};

			var fact = wrap(function(g) {
						return function(n){
							if (n < 2) return 1;
							return n * g(n - 1);
						};
					});

			（7） 将g函数内敛进wrap中（这步不难）
			var wrap = function(h){
				return recur(function(f){
						return h(function(n){
								return f(f)(n);
							});
						});
			};


			（8） 将recur内敛进wrap中形成Y combinator
			var Y = function(h){
				return function(function(f){
					return f(f);	
				})(function (f){
					return h(function(n){
							return f(f)(n);
						});
					});
			};


================================================================
================================================================
================================================================
2014/01/06
1.strcpy 与 strdup的区别：
strdup是由malloc来分配内存，并由free释放内存。  to = strdup(from);
strcpy则是实现已经分配好了内存，且strcpy(to, from) ;
to与from的地址不能重叠。

2.RedditMQ (can use python client)

3.信号处理------>进程间通信的一种，SIGKILL &
SIGSTOP是不能被屏蔽的信号，而一般而言在程序开始的时候都先屏蔽所有的信号，然后再在各个进程中注册需要监控的信号及其处理函数。

4.time wheeling
设计，用shared_ptr以及weak_ptr（自动提升能力来检测链接是否存在），在桶中放置shared_ptr当桶失效的时候自动析构（减少引用计数），在conn连接中保存weak_ptr当有新数据进来的时候可以提升为shared_ptr并放入到当前桶中。通过使用boost::circule_buffer其在插入一个桶的时候会将最开始的桶弹出并析构其中的所有shared_ptr.
桶中存放的数据entry ------> 带有weak_ptr的conn信息。
weak_ptr由shared_ptr构造conn信息原始是保存在shared_ptr中的。在entry的析构函数中会主动提升weak_ptr，如果返回的是null
ptr则conn对象已经被释放，如果不是Null则需要手动释放。在析构桶数据的时候析构shared_ptr（shared_ptr减少对象的引用计数知道为0的时候析构对象）。

5.yacc & lex编写简单的编译器
通过lex来编写简单的词法分析程序


6.高阶函数（first function type）:    如果一个函数以函数作为实在参数，或者返回函数作为值，那么它就是一个高阶函数（high-order function），也称为函数形式。

7.如果再对活动这块做设计的话，我将人物的所有数据都存在一个playerinfo中，然后统一由festival_mgr控制，将数据与逻辑部分分类


================================================================
================================================================
================================================================
2014/01/20
1. visitor pattern					访问者模式适用于数据结构相对未定的系统，它把数据结构和作用于结构上的操作之间的耦合解脱开，使得操作集合可以相对自由地演化。
访问者设计模式的一个实例----->用以理解其设计原理：
银行的各个窗口都能处理多种业务，每个客户进来以后随机选取窗口，进行不同业务的处理，而在程序实现中，业务之间的调度将成为性能的瓶颈。现在的一个解决方案，在client进门的时候，设置一个visitor来询问每个client所要办理的业务，然后将他们安排到对应的业务窗口中，这样每个窗口就能只办理固定业务提高效率。

2.面向对象的设计原则中最重要的便是所谓的"开一闭"原则。一个软件系统的设计应当尽量做到对扩展开放，对修改关闭。达到这个原则的途径就是遵循"对变化的封装"的原则。这个原则讲的是在进行软件系统的设计时，应当设法找出一个软件系统中会变化的部分，将之封装起来。

3.C++模板嵌套：	template <typename U, typename V> ---> tmp<int, int>这样
				template <typename U, template<typename k, typename v> class
				T>  -----> tmp<int, T<k,v> >

4.　generic Programming的思想精髓是基于接口编程（相对于OOP，连多态所需的基类都不要了），它的技术出发点是选择子，核心技术是：类型推导、类型萃取、特化/偏特化，其成果是STL库：一组通用容器和一组操作于通用容器上的通用算法. 
generative programming的思想精髓是基于策略编程（编译器根据策略自动生成所需代码，由于具有更高的抽象性，所以代码复用度也更高），在Loki库的实现中，目前只使用了递归策略，它的技术出发点是Typelist，核心技术是：类型推导、类型萃取、特化/偏特化、多重继承、类型间去耦合，其成果是Loki库：对设计模式的封装. 
C++ 模板元编程实例   Loki库 。
什么是模板的特化、偏特化呢？大致的意思为：如果一个template拥有一个或者一个以上的template参数，我们可以针对其中一个或者多个参数进行特化处理（如果全部进行特化处理就是全特化，否则就是偏特化，切记：函数模板只能进行全特化，不能进行部分特化）。也就是说，我们可以提供一个特别版本，符合泛化条件，但是其中某些（全部）template参数已经由实际类型或者数值取代。

5.红黑树的插入情况：（如果有节点的color发生变化，则需要对改节点进行插入性质检测）
	1、node_insert->parent == null_node; ---> node_insert->color = BLACK;
	2、node_insert->parent->color == BLACK;  fine!!	
	3、node_insert->parent->color == RED;  uncle->color == RED; --->
	node_insert->parent->color = BLACK; uncle->color = BLACK; grandpa->color =
	RED; ---->继续对grandpa进行插入检测
	4、uncle->color = BLACK; 此时如果node_insert
	是parent的left/right，且parent
	是grandpa的left/right，既形成了LL,RR形式，则对rotate_right/left(grandpa);
	grandpa->color = RED; parent->color = BLACK;
	5、否则出现LR,RL情况，则先转换成LL,RR型，LR--->rotate_left(parent)
	RL--->rotate_right(parnet)再交给(4)处理。



===================================================================
2014/02/08
1.对称加密算法与非对称加密算法的使用
现在在通信中一般都是先用非对称加密（加密密匙）加密一个对称机密算法需要的secret，然后发给对端(在这之前对端先产生非对称加密的加密密匙和解密密匙，并将加密密匙发送给对端)。当连接建立以后
就一直使用对称加密（比如RC4）来加密数据就可以了。

2.0xffffffffull的意思（最后的ull是unsigned long long）：uint32_t l = v &
0xffffffffull  ---->  64bit 的后32bit全是1的写法。

===================================================================
2014/02/11
1.c++ set的insert函数有3中形式，在insert single
element时返回std::pair<iterator, bool>
在insert(iterator pos, const value_type
		&val)的时候返回iterator（指向插入的元素）
在insert(iterator first, iterator last)的时候不返回void
且set插入会自动保持有序，所以最有效的插入式刚好插到有序的那个point之后。

2.C++中定义类型的typename使用(内嵌依赖类型)、（因为缺省情况下内嵌依赖类型不被C++编译器视为类型）---》所以需要typename
来让编译器当这个内嵌类型为一种类型，
如果要用其他已有C++的类型来充当新类中的类型，我们需要typedef typename
std::list<T>::iterator iterator;
iterator是list的内嵌类型，
因为iterator的类型依赖于具体的T是什么，
最后typedef成一个类型提供使用。

3.复合语句in c++，(宏定义需要注意点的一个Point)
和普通语句的区别就是：复合语句是以‘}’结尾的（不能有;），而普通语句是以‘;’结尾的。
如果在条件语句中，执行语句是复合语句宏而在后面加上‘;’就会提前结束if-else语句.

4.enable_shared_from_this的使用：
使用情景：当类对象被 shared_ptr 管理时，需要在类自己定义的函数里把当前类对象作为参数传给其他函数时，这时需要传递一个 shared_ptr ，否则就不能保持 shared_ptr 管理这个类对象的语义（因为有一个 raw pointer 指向这个类对象，而 shared_ptr 对类对象的这个引用没有计数，很有可能 shared_ptr 已经把类对象资源释放了，而那个调用函数还在使用类对象——显然，这肯定会产生错误）。

主要解决的是在类内部传递this引起的引用计数不递增而使得shared_ptr在free对象的时候所产生的野指针错误。

5.using声明（using namespace xxxx;）  VS      using指令（using xxx::yy)
using声明只是添加了一层名字查找规则（在local查找不到的时候，优先到using的名字空间中查找），而using
指令则相当于在当前的local域中定义了该变量。这可能与Local本身的变量名冲突。

6.C++中的局部变量在离开作用域的时候立即析构，比如匿名对象，如果它没有被别的语句引用，则会在它的下一句就立即析构掉（编译时候一句控制了）。

7.编译器的乱序始终是要考虑的一个point，
特别是在资源管理的时候，更应该考虑清楚，如果一个调用在资源分配之间throw
exception那么有可能导致资源泄露.
不免的做法就是不要在一个statement中写这样的语句，如果不确定可以分开写。<因为编译器只对语句内才有重排的能力，跨语句它是不会有这个权限的>（ effective c++ 17）

8.在使用 const reference的时候，参数调用的函数也必须是const的。

9.C++ 只允许偏特化class，而不能对function偏特化，但是可以全特化。如果实在需要只能通过函数重载来实现类似的功能。
	
10.C++的名字查找规则：
1.如果在声明函数的参数时使用了一个类，那么在查找匹配的函数名字时，编译器会在包含参数类型的名字空间中也进行查找。（不过这只对一条statement有用，当进入下一条语句的时候，编译器不会保持搜索上一条语句搜索过的名字空间。
这时候就可能搜索不到那个名字空间中的函数（如果用的是基本类型，而不是该名字空间中的类型的话。）这样就会出现一种编译错误
-----Koenig lookup规则。）  只有显示的using declear xxx;
才会把xxx名字空间引入到当前作用域的编译器名字搜索规则中。

11.类型转换的两种写法：  1、C类型 （T）expression    2.函数类型T（expression）

12.宏定义并不重视作用域这个概念。

13.C++ mutable关键字-----》因为要遵循bitwise constness原则所以对于const
成员函数其不能修改内部non-static成员变量。如果想要修改但又不想编译不通过，就需要对修改的变量加上mutable修饰。

14.局部静态变量与non-local static variable ：
局部静态变量的作用域只在局部，它保存在静态存储区中，但当离开了当前作用域就再也访问不到了（可以说是一种内存泄漏式的浪费，除非返回改对象的引用或者指针。）
注意static这个关键字很大程度上表示当前编译单元可见而非全局可见------->静态全局变量
VS 普通全局变量的区别在此。


15.偏特化：  //貌似实用的就是对指针进行的偏特化
	template<class I> //特化的类型   可能是void *  ----> I = void *
	struct iterator_trait<I *>{ I * ----> 偏特化的形式表示。
		typedef I value_type;
	};

	全特化：
	template<>
	struct iterator_trait<具体类型名>{
		typedef XXX::value_type value_type;
	};


===================================================================
2014/02/17
1. 循环不变式：(写二分的条件查找的时候可以使用)
	　循环不变式主体是不变式，也就是一种描述规则的表达式。其过程分三个部分：初始，保持，终止。
　　1、初始：保证在初始的时候不变式为真。（不变式就是循环条件判定内的条件）
　　2、保持：保证在每次循环开始和结束的时候不变式都为真。
　　3、终止：如果程序可以在某种条件下终止，那么在终止的时候，就可以得到自己想要的正确结果。 


2. C++ lambda------> 

[]        Capture nothing (or, a scorched earth strategy?)
[&]       Capture any referenced variable by reference
[=]       Capture any referenced variable by making a copy
[=, &foo] Capture any referenced variable by making a copy, but capture variable foo by reference
[bar]     Capture bar by making a copy; don't copy anything else
[this]    Capture the this pointer of the enclosing class

3.在用select来监听fifo的fd（比如管道）的时候，需要将RDWR文件属性开启，而不是只开启RDONLY。
管道的多写是非原子性的需要锁来保护。     通过pipe(int
		fd[2]);在内核中创建一个管道，然后通过读写fd[0],
	fd[1]来判断是否有数据，因为管道的BUFF在内核中设定所以不能写太多的数据适合做消息传递，但可以写一个简单的字符来做消息到达的标示。
	signal(SIGPIPE, SIG_IGN);
	防止因为对一个close的端口（fd）进行读写而出发SIGPIPE退出进程。

4.write，read时候的错误处理：如果网络操作无法立即完成(readable/writeable == 0 或 read/write的错误码是EWOULDBLOCK) 则将请求保存在pendingread/pendingwrite中.


5. __asm__ volatile("" : : : "memory");
使用在确定下面的代码不要被优化到上面去的时候就在那行代码上加入这个编译器指令来确保编译器不会对这段代码进行顺序优化。
__asm__ volatile("mfence" : : : "memory"); 这是对CPU乱序的汇编级指令

 

6.多线程编程中的锁的时候	<什么时候使用什么样的锁> ：
			Mutex：属于sleep-waiting类型的锁.
			spinlock：也称自旋锁,是属于busy-waiting类型的锁.在多处理器环境中,
			自旋锁最多只能被一个可执行线程持有。如果一个可执行线程试图获得一个被争用(已经被持有的)自旋锁，那么该线程就会一直进行忙等待，自旋，也就是空转，等待锁重新可用。如果锁未被争用，请求锁的执行线程便立刻得到它，继续执行。（小心其的忙等待情形，所以spinlock不应该被长时间的持有，他的临界区保护应该尽量小）Kernel中的自旋锁不能够在能够导致睡眠的环境中使用。举个例子，一个线程A获得了自旋锁L；这个时候，发生了中断，在对应的中断处理函数B中，也尝试获得自旋锁L，就会中断处理程序进行自旋。但是原先锁的持有者只有在中断处理程序结束后，采用机会释放自旋锁，从而导致死锁。
由于涉及到多个处理器环境下，spin lock的效率非常重要。因为在等待spin lock的过程，处理器只是不停的循环检查，并不执行其他指令。但即使这样， 一般来说，spinlock的开销还是比进程调度(context switch）少得多。这就是spin lock 被广泛应用在多处理器环境的原因
			读写锁：读写锁实际是一种特殊的自旋锁，它把对共享资源的访问者划分成读者和写者，读者只对共享资源进行读访问，写者则需要对共享资源进行写操作。这种锁相对于自旋锁而言，能提高并发性，因为在多处理器系统中，它允许同时有多个读者来访问共享资源，最大可能的读者数为实际的逻辑CPU数。写者是排他性的，一个读写锁同时只能有一个写者或多个读者（与CPU数相关），但不能同时既有读者又有写者。 
　在读写锁保持期间也是抢占失效的。 
如果读写锁当前没有读者，也没有写者，那么写者可以立刻获得读写锁，否则它必须自旋在那里，直到没有任何写者或读者。如果读写锁没有写者，那么读者可以立即获得该读写锁，否则读者必须自旋在那里，直到写者释放该读写锁。（因为存在自旋的情况，如果再写锁写的数据大的时候是不适合用读写锁的，或者临界区过大的时候。）


7.linux下线程私有数据的创建以及使用： pthread_key_t ---->
对线程私有数据的访问key.
pthread_key_create();调用并不能确保线程安全，所以可以出现多个线程看到的key不一致的情况（原则上别的线程可以看到你的Key并访问你的私有数据），通过pthread_once(pthread_once_t
	*flag, void (*fn)(void));来保证原子性。
在使用pthread_once的时候需要，需要保证flag是一个非本地变量（静态或者全局），且
*flag = PTHREAD_ONCE_INIT;
void generate_pthread_key();
pthread_once(flag, generate_pthread_key);
通过pthread_setspecific(pthread_key_t key, const void
	*data);将key于私有数据关联。
void *pthread_getspecific(pthread_key_t key);  获得线程的私有数据。
全局只需要一个pthread_key_t，它在每个线程下面是具有不同的数据的。

8.游戏引擎中的SIMD运算，在c++中有实际的__m128这个变量来表示，它被编译器直接存于CPU的SSE寄存器中以供引擎的数学计算。

9.内存对齐，当对于局部变量编译器会负责帮我们对齐，但当时malloc或者new的对象的时候就需要我们自己来做内存对齐了。

10. C++的无类型参数模板<即传入基础类型>， template<int size>....

11. C++中的内部链接与外部链接变量 
内部链接变量：
1.加 static 前缀的全局变量定义.如: static int x;
2.枚举类型的定义.如: enum Boolean {NO,YES };
3.类的定义. 如: class Point { int d_x; int d_y; ... };
4.内联函数的定义.如: inline int operator==(const Point& left,const Point&right) { ... }
5.union的定义.
6.名字空间中const常量定义

外部链接变量：
1.非内联的类成员函数.如: Point& Point::operator+=(const Point& right) { ... }
2.非内联,非静态的自由函数. 如: Point operator+(const Point& left, const Point& right) { ... }
3.非静态的全局定义.
当用extern修饰const变量将强制为其分配内存，因为extern表示采用外部链接，因此其必须有某个地址保存其值。

12.将怪物AI与怪物的行走分离，
怪物的行走可以独立的做成一个通用的模块，以一个较低的频率（1s）来刷新所有的怪我来降低CPU的负荷。怪物的AI可以由别的事件触发而成。这里就会出现怪物的状态有两种的情况<1.AI,
	2.行走>， 不同状态下必须停走对面状态的运行不然就会出现奇怪的现象。


13.Erlang是一个函数式语言，不存在可变状态。这种"纯"函数的特性，天生就支持高并发而不需要加锁。



===================================================================
2014/02/24
1.C++的继承体系中可以继承static成员函数，
而且可以通过类的实例来调用static成员函数。

2. .h文件中可不能进行循环引用，使用前向声明来解决这个问题。

3.Memory Barriers（内存屏障）, 
为啥需要内存屏障？
编译器优化，打乱代码的顺序
CPU的乱序执行、延期及合并、预加载、分支预测、以及多种类型的缓存，都会打乱代码的顺序
内存屏障的种类
Write Memory Barriers
Data Dependency Barriers
Read Memory Barriers
General Memory Barriers

CPU对于相互依赖的操作时不会进行乱序的，只有对相互没有依赖的操作才会进行乱序优化。（原则就是乱序了也不能影响程序的结果）

	CPU级别的内存屏障：
GCC的通用内存屏障(CPU级别)
__sync_synchronize (...)
X86-64的内存屏障(摘自Linux实现)
#define smp_mb() asm volatile("mfence":::"memory")
#define smp_rmb() asm volatile("lfence":::"memory")
#define smp_wmb() asm volatile("sfence" ::: "memory")
#define smp_read_barrier_depends() do { } while (0) //why?
	编译器级别的内存屏障：
__asm__ volatile("" ::: "memory")

4.游戏AI的设计：（输入-----> AI模型 -----> 输出执行动作 ------> 输入）
（1）可以直接以脚本的形式来编写并挂到逻辑上
（2）用FSM来做
（3）用行为树来做。

通过行为树主要要构造出这个AI模型的行为树结构，然后挂到AI对象上进行执行。

5.C++用多态（源于virtual
table）来完成继承类对象中函数的调用,而在C中往往是内部设置一个局部变量
func和args指针，然后在初始化的时候传入已经定义好的函数指针以及数据，在调用的地方取出来执行就好。

6.通过socketpair来实现进程之间或线程之间的全双工通信。 int s[2];
if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) == -1){ exit(-1);}
s[0], s[1]每个端都可以进行读写

7.socket在设置了nonblock之后，如果在send的时候还是出现errno ==
EAGAIN；就需要扩大sendbuf来, setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
		&new_buf,new_buf_size);然后继续::send调用。

8.C++的function结构的使用： std::function<返回值（参数列表）> 函数对象------>>也可以用函数指针构造  （即需要的是一个callable的东西--->函数对象或者函数指针）
关于函数对象的解释：A FunctionObject type is the type of an object that can be used on the left of the function call operator.

std::bind(f, args); f ----> Callable object (function object, pointer to function, reference to function, pointer to member function, or pointer to data member) that will be bound to some arguments

	// store a free function
    std::function<void(int)> f_display = print_num;
    f_display(-9);
 
    // store a lambda
    std::function<void()> f_display_42 = []() { print_num(42); };	// 闭包
    f_display_42();
 
    // store the result of a call to std::bind
    std::function<void()> f_display_31337 = std::bind(print_num, 31337); //std::bind 将函数与它所要的参数绑定起来并且返回一个function结构
    f_display_31337();
 
    // store a call to a member function
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    const Foo foo(314159);
    f_add_display(foo, 1);
 
    // store a call to a member function and object
    using std::placeholders::_1;
    std::function<void(int)> f_add_display2= std::bind( &Foo::print_add, foo, _1 );		// 绑定类成员函数时，第二个参数是类实例，之后的参数才是函数调用所需要的参数
    f_add_display2(2);
 
    // store a call to a function object
    std::function<void(int)> f_display_obj = PrintNum();			// struct xxx { operator()(){ ... } } xxx()返回一个function结构
    f_display_obj(18);

typedef std::function<void(int)> callback_fn;
callback_fn fn_1 = .....;			// 函数回调的基本设置。


9.通过socketpair或者pipe来完成框架内外层的通信（它相当于做了一个中间通信层），首先在框架内层监听pipe的read端或者socketpair的read事件，然后通过一个共享队列（外部多写，内部单读），当多路复用器监听到read事件，就取出队列中的一个事件进行处理。（libevent的event_msgqueue就是这么实现的）

10. char vs wchar_t 和 string vs wstring
 C++标准中，wchar_t是宽字符类型，每个wchar_t类型占2个字节，16位宽。汉字的表示就要用到wchar_t 。char，我们都知道，占一个字节，8位宽。
其实知道了这个以后，要在wchar_t 和 char两种类型之间转换就不难实现了。wchar_t 转换为char 的代码如下： 
有如下的wchar_t和char变量:
wchar_t w_cn = '中'; char c_cn[2] = {'0'} ;

char *C2W(wchar_t w_cn , char c_cn[2])

{    //following code convert wchar to char 

      c_cn[0] = w_cn >> 8 ;	// 高位存在前面

      c_cn[1] = w_cn ;

      c_cn[2] = '\0';

      return c_cn ;
}
直接使用cout << w_cn << endl; 是不行的。


11. 按照C++ Template最初的想法，模板不就是为了提供一个类型安全、易于调试的宏吗？有类型就够了，为什么要引入整型参数呢？考虑宏，它除了代码替换，还有一个作用是作为常数出现。所以整型模板参数最基本的用途，也是定义一个常数。例如这段代码的作用：

template <typename T, int Size> struct Array
{
    T data[Size];
};

Array<int, 16> arr;
因为模板的匹配是在编译的时候完成的，所以实例化模板的时候所使用的参数，也必须要在编译期就能确定。例如以下的例子编译器就会报错：
template <int i> class A {};
void foo()
{
    int x = 3;
    A<5> a;         // 正确！
    A<x> b;         // error C2971: '_1_3::A' : template parameter 'i' : 'x' : a local variable cannot be used as a non-type argument
}


12.从C++的优秀开源项目中可以发现，一个最小粒子的操作体结构是多么重要的一个设计。就像leveldb中的slice.


===================================================================
2014/03/03
1.虚函数是不可能写成template模式的，因为template本身需要在编译时期验证它的正确性，而虚函数是在运行期做的特性。

2.C++中的reinterpret_cast是不可移植的。且一般而言需要将转到的类型足够的大能够容纳下被转的类型。

3.C++的内嵌类不能定义static成员变量，也不能访问non-static局部变量。 他是final class 外界不能继承它。


4.C++11中的返回值类型后置的语法：
int &foo(int &i);
float foo(float &f);

// 这里的decltype就保存了包装函数是否返回一个引用类型的数据
template<class T>
auto return_value_backforward(T &t)->decltype(foo(t))
{
	return foo(t);
}

5.如果想在C++中重载operator delete操作，我们就必须以一个指向void的指正作为它的唯一参数。或者采用这样的重载 void operator delete(void *p, std::size_t size);

6.C++的隐式转型还可以用操作符重载的方式实现。  例如loki智能指针转型为原始指针 
operator T*()   // 是没有返回值的
{
	return _pointer;
}

7.用std::vector而尽量避免使用new A[]; delete []A;

8.Linux direct I/0 VS 缓存IO ----->  O_DIRECT标志(对于字符设备而言是无效的)
普通的 I/O 操作一般会被内核缓存，这种 I/O 被称作缓存I/O。（将数据先缓存到系统的页缓存中）
直接 I/O不经过操作系统内核的缓存，数据直接在磁盘和应用程序地址空间进行传输。缓存 I/O 的缺点

在缓存 I/O 机制中，DMA 方式可以将数据直接从磁盘读到页缓存中，或者将数据从页缓存直接写回到磁盘上，而不能直接在应用程序地址空间和磁盘之间进行数据传输，这样的话，数据在传输过程中需要在应用程序地址空间和页缓存之间进行多次数据拷贝操作，这些数据拷贝操作所带来的 CPU 以及内存开销是非常大的。
对于某些特殊的应用程序来说，避开操作系统内核缓冲区而直接在应用程序地址空间和磁盘之间传输数据会比使用操作系统内核缓冲区获取更好的性能，下边这一小节中提到的自缓存应用程序就是其中的一种。

凡是通过直接 I/O
方式进行数据传输，数据均直接在用户地址空间的缓冲区和磁盘之间直接进行传输，完全不需要页缓存的支持。操作系统层提供的缓存往往会使应用程序在读写数据的时候获得更好的性能，但是对于某些特殊的应用程序，比如说数据库管理系统这类应用，他们更倾向于选择他们自己的缓存机制，因为数据库管理系统往往比操作系统更了解数据库中存放的数据，数据库管理系统可以提供一种更加有效的缓存机制来提高数据库中数据的存取性能。

9.unix下的字符设备 VS 块设备
字符设备需要顺序读写，且以字符为单位读写（字符可能是一个bytes,也可能是多个bytes，因为编码格式）
块设备可以随机读写---->就像硬盘。

10.CopyOnWrite容器只能保证数据的最终一致性，不能保证数据的实时一致性。所以如果你希望写入的的数据，马上能读到，请不要使用CopyOnWrite容器。    COW的特点就是在write的时候 加锁拷贝出一份并修改替换。 read的时候是不用加锁的并发读


===================================================================
2014/03/10
1.sem_wait(sem_t*); lock the semaphore, when the sem_t's value == 0, and block here, else return and running next code.
sem_post(sem_t*);

2.C++11中的move语义 <utility>，　type &&move(type &&value){ return value;},可以将一个左值转化成一个右值。在多种值语义重载的时候有用（有些重载函数用到左值为参数，有些用到右值为参数,通过move语义可以调用右值为参数的重载，当右值被opeator = 的参数时，原来的右值就不再持有那个对象，这是一个对象专一的操作）

C++11还有一个std::move函数<algorithm>  OutputIt move( InputIt first, InputIt last, OutputIt d_first );
Moves the elements in the range [first, last), to another range beginning at d_first.  OutputIt 指向d_first + (last-first);

std::vector<Event> expired;
std::move(byExpiration.begin(), end, std::back_inserter(expired));
std::back_inerter(container) <iterator> --- return the iterator last from the container.



3. 做名字匹配查找的时候，可以用STL的map<string, ptr>.
然后用name_list.find(name); 来查找。  这是因为name_list在插入的时候根据string key排序，在查找的时候能快点。


===================================================================
2014/03/17
1.网络库选型：libevent是一个不错的选择，不过对于有些应用，比如redis等都是自己实现一个简易的高并发网络库。
对于http服务器的网络库推荐使用libevhtp库，它是对libevent自带的evhttp库的改进，且性能上比nginx的http服务更好。

2.socket的send与recv调用的注意点：
	send调用：当调用该函数时，send先比较待发送数据的长度len和套接字s的发送缓冲的长度，如果len大于s的发送缓冲区的长度，该函数返回SOCKET_ERROR；send函数只负责将数据拷贝到套接字s的发送缓存中而已，具体的发送操作由内核完成，如果内核发送缓冲区的剩余空间小于send提供的buf长度但总容量大于buf长度，则send就阻塞住直到内核将发送缓冲中的数据发送出去有足够的空间之后再copy过去（如果是非阻塞的send则立即返回0）。如果在内核发送数据的时候网路出错，再下一个socket调用的时候就会返回一个SOCKET_ERROR，因为（每一个除send外的Socket函数在执行的最开始总要先等待套接字的发送缓冲中的数据被协议传送完毕才能继续，如果在等待时出现网络错误，那么该Socket函数就返回SOCKET_ERROR）在Unix系统下，如果send在等待协议传送数据时网络断开的话，调用send的进程会接收到一个SIGPIPE信号，进程对该信号的默认处理是进程终止。

	recv调用：recv先等待s的发送缓冲中的数据被协议传送完毕，如果协议在传送s的发送缓冲中的数据时出现网络错误，那么recv函数返回SOCKET_ERROR，如果s的发送缓冲中没有数据或者数据被协议成功发送完毕后，recv先检查套接字s的接收缓冲区，如果s接收缓冲区中没有数据或者协议正在接收数据，那么recv就一直等待，只到协议把数据接收完毕。当协议把数据接收完毕，recv函数就把s的接收缓冲中的数据copy到buf中（注意协议接收到的数据可能大于buf的长度，所以在这种情况下要调用几次recv函数才能把s的接收缓冲中的数据copy完。recv函数仅仅是copy数据，真正的接收数据是协议来完成的），recv函数返回其实际copy的字节数。如果recv在copy时出错，那么它返回SOCKET_ERROR；如果recv函数在等待协议接收数据时网络中断了，那么它返回0。注意：在Unix系统下，如果recv函数在等待协议接收数据时网络断开了，那么调用recv的进程会接收到一个SIGPIPE信号，进程对该信号的默认处理是进程终止。

	所以在recv于send的时候要用while(recv()/write())来检测是否写完的操作。
	

3. C中的两种多版本函数的选择使用写法：（select, poll, epoll等的选择）
一种在头文件中定义一个结构体 struct event_multiexplor{ 定义各种实现函数指针
	void (*add)(...)  };

另一种在头文件中定义静态方法声明， 并在实现文件中 epoll.c,kqueue.c等中对于同一个函数名的定义。最后在头文件中根据平台来include实现文件。

4. getopt函数----> Normally, getopt is called in a loop. When getopt returns -1, indicating no more options are present, the loop terminates.(.......)

5.时间的管理最好使用monotonic时钟类型。
	检测：
static void detect_monotonic(void)
{
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
struct timespec ts;
if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
use_monotonic = 1; // 系统支持monotonic时间
#endif
}
Monotonic时间指示的是系统从boot后到现在所经过的时间。

因为时间系统调用时一个耗时的系统调用，如果对性能要求比较高的时候，就需要换成时间，并在每次做time_correct()(如果是monotonic的就不用校正，否则要相应的做调整)，并且在循环的下一次开始之前更新缓存的时间。


6.ucontext.h		
// 
void makecontext(ucontext_t *ucp, void (*func)(),int argc, ...);
The makecontext() function shall modify the context specified by ucp, which has been initialized using getcontext(). When this context is resumed using swapcontext() or setcontext(), program execution shall continue by calling func, passing it the arguments that follow argc in the makecontext() call.
makecontext(&coro->ctx_, (void (*)(void))mainfunc, 1, this);
int setcontext();
int swapcontext(from, to);
int getcontext(); //如果没有setcontext调用在前，则getcontext将初始化ucontext_t参数--->分配空间。之后需要我们自己进行
context.uc_stack.ss_sp = char stack[size];
context.uc_stack.ss_size = sizeof(stack);
context.uc_link = 链到main_context(主scheduler的context)
如果有setcontext则直接获取对应的ucontext_t数据。
void
Scheduler::Resume(int id) {
  Coroutine *coro = coros_[id];

  if (coro == NULL) {
    return;
  }
  int status = coro->status_;
  switch(status) { 
  case kStatusReady:
    getcontext(&coro->ctx_);
    coro->ctx_.uc_stack.ss_sp = coro->stack_;
    coro->ctx_.uc_stack.ss_size = kDefaulaStackSize;
    coro->ctx_.uc_link = &main_;
    running_ = id;
    coro->status_ = kStatusRunning;
    makecontext(&coro->ctx_, (void (*)(void))mainfunc, 1, this);
    swapcontext(&main_, &coro->ctx_);
    break;
  case kStatusSuspend:
    running_ = id;
    coro->status_ = kStatusRunning;
    swapcontext(&main_, &coro->ctx_);
    break;
  default:
    break;
  }
}


7.BOOST::asio的异步编程中比较烦的地方是，先像框架注册事件已经一个回调函数，并在回调函数中处理逻辑，最后又要往框架中重新注册一次该事件，这样循环往复。

8.进程内的线程共享的数据：
	进程代码段、进程的公有数据(利用这些共享的数据，线程很容易的实现相互之间的通讯)、进程打开的文件描述符、信号的处理器、进程的当前目录和进程用户ID与进程组ID。

9.
 memcached  就是利用这两个数据结构配合上sendmsg来完成数据的传输。
<sys/socket.h>
sendmsg(Socketfd, struct msghdr *, int flags);
recvmsg(Socketfd, struct msghdr *, int flags);

struct msghdr
{
	void	*	msg_name;	/* Socket name			*/
	int		msg_namelen;	/* Length of name		*/
	struct iovec *	msg_iov;	/* Data blocks			*/
	int		msg_iovlen;	/* Number of blocks		*/
	void	*	msg_accrights;	/* Per protocol magic (eg BSD file descriptor passing) */
	int		msg_accrightslen;	/* Length of rights list */
};
In the msghdr structure, the msg_name and msg_namelen members specify the source address if the socket is unconnected. If the socket is connected, the msg_name and msg_namelen members shall be ignored.

<sys/uio.h>
struct iovec {  
	void *iov_base; /* Starting address */  
	size_t iov_len; /* Length in bytes */  
};  


10.new必须要对应到delete否则就应该用智能指针来管理，对于指针变量的set方法是致命的，因为它将使得我们失去对于那块new内存的管理索引，导致内存泄露！！！！

11.linux下的多种IO函数， posix函数 read write针对的是fd其并没有设置用户态缓冲的特性。 而C标准的API fread fwrite函数则是针对流对象FILE*,进行操作，他们可以再用户态设置行缓冲，全缓冲已经不缓冲模式，这样可以降低陷入内核态的次数从而提高效率。 当然内核态中肯定也有相应的缓冲buf，而不是立即写入到磁盘中。

12.BOOST::multi_index_container容器：   
typedef  
boost::multi_index_container<  
    Student,				------------> 类型
    indexed_by<				------------> 索引
        ordered_unique<		------------> 唯一值索引
                  // 学号是唯一值的索引  
        tag<stu_num>,  BOOST_MULTI_INDEX_MEMBER(Student,unsigned int,stu_num)>,  
            // 姓名是非唯一值的索引  
                     ordered_non_unique<  
        tag<stu_name>,BOOST_MULTI_INDEX_MEMBER(Student,std::string,stu_name)>,  
                  // 年龄是非唯一值的索引  
        ordered_non_unique<  -----------> 非唯一索引
        tag<stu_age>, BOOST_MULTI_INDEX_MEMBER(Student,unsigned int,stu_age)>    
    >  
> StudentContainer;  


13.上下文切换只能发生在内核态中。在类 Unix 系统中共存意味着当系统调用发生时 CPU 切换到内核态是必要的。这应该叫做模式切换而不是上下文切换，因为没有改变当前的进程


14.first class function:
the language supports passing functions as arguments to other functions, returning them as the values from other functions, and assigning them to variables or storing them in data structures.


15.C++ <future>
promise 对象可以保存某一类型 T 的值，该值可被 future 对象读取（可能在另外一个线程中），因此 promise 也提供了一种线程同步的手段。在 promise 对象构造时可以和一个共享状态（通常是std::future）相关联，并可以在相关联的共享状态(std::future)上保存一个类型为 T 的值。
    std::promise<int> prom; // 生成一个 std::promise<int> 对象.
    std::future<int> fut = prom.get_future(); // 和 future 关联.
    std::thread t(print_int, std::ref(fut)); // 将 future 交给另外一个线程t.
    prom.set_value(10); // 设置共享状态的值, 此处和线程t保持同步.
    t.join();
promise是一种线程间同步的C++特性机制。

packaged_task  C++提供的任务对象它包装了一个可调用的对象，并且允许异步获取该可调用对象产生的结果，从包装可调用对象意义上来讲，std::packaged_task
与 std::function 类似。只不过 std::packaged_task 将其包装的可调用对象的执行结果传递给一个 std::future 对象（该对象通常在另外一个线程中获取 std::packaged_task 任务的执行结果）。
packaged_task内部包含两个对象----> （1）任务对象，（2）共享状态(shared state)，用于保存任务的返回值，可以通过 std::future 对象来达到异步访问共享状态的效果。
    std::packaged_task<int(int,int)> task(countdown); // 设置 packaged_task
    std::future<int> ret = task.get_future(); // 获得与 packaged_task 共享状态相关联的 future 对象.
    std::thread th(std::move(task), 10, 0);   //创建一个新线程完成计数任务.
    int value = ret.get();                    // 等待任务完成并获取结果.
    std::cout << "The countdown lasted for " << value << " seconds.\n";
    th.join();

future  std::future可以用来获取异步任务的结果，因此可以把它当成一种简单的线程间同步的手段。调用 std::future::get 会阻塞当前的调用者，直到 Provider 设置了共享状态的值（此时共享状态的标志变为 ready），std::future::get 返回异步任务的值或异常（如果发生了异常）。
 // call function asynchronously:
    std::future < bool > fut = std::async(is_prime, 444444443);

总结：future是一种C++新引进的线程间同步的机制，它可以由promise或者packaged_task构造并在调用结束之后返回设置该值。并被多个线程共享，future.get()获取共享的值，其在结果未返回之前阻塞。


===================================================================
2014/03/31
1. std::atomic类，
typedef enum memory_order {
    memory_order_relaxed,   // relaxed  \\ 不指定内存屏障，所以内存操作执行时可能是乱序的。
    memory_order_consume,   // consume	
    memory_order_acquire,   // acquire  \\ 按照我理解就是插入一个 内存读屏障，保证之前的读操作先一步完成。清空自己cpu上"invalidate queues"  。
    memory_order_release,   // release  \\ 按照我理解就是插入一个 内存写屏障，保证之前的写操作的顺序一定 。清空自己cpu上 的 “ store buffer"
    memory_order_acq_rel,   // acquire/release  \\ 同时插入读写两个屏障。  清空自己cpu上 的 “ store buffer" 和 "invalidate queues"  。
    memory_order_seq_cst    // sequentially consistent
} memory order;

method:
	load(), store(value), fetch_add(), fetch_sub(), operator=, operator T

2.C++ POD
表达式	POD类型T	non-POD类型T
new T	不初始化	缺省初始化
new T()		总是缺省初始化
new T(x)	总是调用构造函数初始化
所以POD类型可能未被初始化。 且POD类型没有显示的构造析构函数，理论上来说它就是一个C结构体，如果我们手动的new了就需要显示的手动析构。



===================================================================
2014/04/14
1.老大通过go 的 symbol包在编译单元的代码段中查找go函数的func
ptr来实行运行期调用。因为go reflect 只对interface 和method set起作用。
函数变量实际指向编译期常量 FuncVal 地址，所以目标应该是 .rodata。不同平台 section 名字有些不同，在 OSX 环境下，名字以 “__” 开头。
import (
    "debug/gosym"
    "debug/macho"
)

// Go 函数实际上被包装成一个全局的 FuncVal 对象，因此从 rodata 找。
    rodata := f.Section("__rodata")

    // 相关符号信息段。
    symtab := f.Section("__gosymtab")
    pclntab := f.Section("__gopclntab")

	symdata, _ := symtab.Data()
    pclndata, _ := pclntab.Data()

    pcln := gosym.NewLineTable(pclndata, rodata.Addr)
    table, _ := gosym.NewTable(symdata, pcln)

    // FuncVal 符号名会添加一个后缀。
    sym := table.LookupSym(name + ".f")

	// 转换指针，写入 FuncVal 地址。
    *((*uintptr)(fn)) = uintptr(sym.Value)

===================================================================
2014/04/21
1.要注意整数溢出的情况，特别对于有符号的类型。
*需要格外注意的是编译器在开启-O2的时候也许会对代码进行优化，而直接优化掉你的溢出检查，所以在有符号溢出检查的时候最好都转型成uintptr_t进行比较，这project样可以防止编译器优化。


2. VISUAL STUDIO中需要添加依赖包----> Tool下的options中projects and
solutions中的VC++ Directories中的 依赖 需要Include files 和 Library files

3.写一个lazy evaluation的C++函数 ====> building blocks of lazy programming:
the suspension, the lazy stream, and a whole slew of functional algorithms
that let you operate on them.     http://bartoszmilewski.com/2014/04/21/getting-lazy-with-c/

4.c++11新增list initialization。 {value1, value2, .... valueN}来初始化数据。std::pair<const T*, std::size_t> c_arr() const {
	return {&v[0], v.size()};  // list-initialization in return statement
	// this is NOT a use of std::initializer_list
}

5.initializer_list   ====>  array of objects .  是std::vector的替代品。
why use initializer_list instead of std::vector:
由于C++11新增的列表初始化表达式， （/*******/）C++ compiler对{value1, value2 ... valueN}自动转型为initializer_list。
std::initializer_list can be used wherever you need an expression of the form of {value1, value2, ...., valueN}. It is why other containers from the Standard library also define constructor that takes std::initializer_list as argument. In this way, no container depends on any other container for construction from expressions of the form of {value1, value2, ...., valueN}. 如果需要支持g(std::vector<T> &); 必须显示的在std::vector中加入std::vector(std::initializer_list<T> &);的构造函数。

6. 预处理 ===> 编译 ====> 链接   对于预处理是由预处理器完成的，所以可以不用using引入预处理宏或方法，因为还没到编译阶段，编译器还没有介入。

7.在python这种弱类型中，类型并不那么重要，重要的是接口，如果它走路像鸭子，叫声也像鸭子，我们就认为它是鸭子（起码在走路和叫声这样的行为上）。/***duck like ***/


===================================================================
2014/05/05
1.sockaddr_in相当于end_point（addr, port）的linux下封装的结构体。
std::string InetAddress::getIP()
{
	return inet_ntoa(m_addr.sin_addr);
}

uint16_t InetAddress::getPort()
{
	return ntohs(m_addr.sin_port);
}

2. 尾递归与递归的区别，在于尾递归必须return func();
而递归一般是return func()+num;这样父调用栈就无法被清空因为需要记录num，所以可能出现stack overflow，而尾递归不需要记录num所以其父调用栈是没用的编译器会帮我们优化掉这个调用栈所以就不会出现stack overflow。

3. freopen("data.in", "r", stdin);//重定向输入流
   freopen("data.out", "w", stdout);//重定向输出流 
   这样以后的scanf 与 printf都会被从这两个重定向的流中读写。

4.不管是snprintf还是vsnprintf都只写入buff_size-1个字符进入到buff中，最后会补上一个'\0'的结束符，多余的数据被截断。

5.问题的焦点在于：删除 ctx 不能立刻删除 mq ，这是因为 mq 可能还被 globalmq 引用。而 mq 中并没有记录 ctx 指针（保存 ctx 指针在多线程环境是很容易出问题的，因为你无非保证指针有效），而保存的是 ctx 的 handle 。

