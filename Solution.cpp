#include "Solution.h"
#include <future>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include "chan.hpp"

using namespace std;

void foo() 
{
	cout << "Hello C++11" << endl;
}

int Solution::cppThreadTest1()
{
	thread threadMain(foo); //启动线程foo
	threadMain.join(); //等待线程执行完成

	system("pause");

	return 0;
}

void hello(const string & name)
{
	cout << "Hello " << name << endl;
}

int Solution::cppThreadTest2()
{
	//string & name = "C++11";//报错
	const string & name = "C++11";
	
	thread threadMain(hello, name);
	threadMain.join();

	system("pause");

	return 0;
}

class Greet
{
	static string owner;
public:
	void sayHello(const string & name) 
	{
		cout << "Hello " << name << " from " << Greet::owner << endl;
	}
};

string Greet::owner = "Greet";

//类成员函数做为线程入口时，仍然十分简单: 把this做为第一个参数传递进去即可
int Solution::cppThreadTest3()
{
	Greet greet;

	thread threadMain(&(Greet::sayHello), &greet, "C++11");

	threadMain.join();

	system("pause");

	return 0;
}


void pauseThread()
{
	//sleep 500ms
	this_thread::sleep_for(chrono::milliseconds(500));
	//sleep 到达指定时间点
	this_thread::sleep_until(chrono::system_clock::now() + chrono::milliseconds(500));
}

int Solution::cppThreadTest4()
{
	thread threadMain(pauseThread);
	threadMain.join();

	system("pause");
	return 0;
}

//最终foo线程是由threadA管理，还是threadB来管理？答案是由threadB来管理。
//std::thread被设计为只能由一个实例来维护线程状态，以及对线程进行操作。因此当发生赋值操作时，会发生线程所有权转移。
int Solution::cppThreadTest5()
{
	thread threadA(foo);
	thread threadB;
	
	//C++11支持拷贝，但好像之后的版本
	//threadB = threadA;//报错
	threadB = move(threadA);

	return 0;
}

void initialize() 
{
	cout << __FUNCTION__ << endl;
}

once_flag of;

void myThread()
{
	//该函数的作用顾名思义：保证call_once调用的函数只被执行一次。
	call_once(of, initialize);
}

// 仅输出一次：initialize
int Solution::cppThreadTest6()
{
	thread threads[10];
	for (auto &thr : threads)
	{
		thr = thread(myThread);
	}
	for (auto &thr : threads)
	{
		thr.join();
	}

	system("pause");
	return 0;
}

void consume(chan<int> ch, int threadId)
{
	int n = 0;
	while (ch >> n)
	{
		printf("[%d] %d\n", threadId, n);
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

int Solution::cppThreadTest7()
{
	chan<int> chInt(3);
	//消费者
	thread consumers[5];
	for (int i = 0; i < sizeof(consumers) / sizeof(consumers[0]); i++)
	{
		consumers[i] = thread(consume, chInt, i + 1);
	}

	//生产数据
	for (int i = 0; i < 16 ; i++)
	{
		chInt << i;
	}
	chInt.close(); //数据生成完毕

	for (auto & thr : consumers)
	{
		thr.join();
	}

	system("pause");
	return 0;
}

void inc(mutex & inMutex, int loop, int & counter)
{
	for (int i = 0; i < loop; i++)
	{
		inMutex.lock();
		++counter;
		inMutex.unlock();
	}
}

int Solution::mutexTest1()
{
	thread threads[5];
	mutex testMutex;
	int counter = 0;
	for (auto &thr : threads)
	{
		thr = thread(inc, ref(testMutex), 1000, ref(counter));
	}

	for (auto &thr : threads)
	{
		thr.join();
	}

	cout << counter << endl;

	system("pause");
	return 0;
}

void run500ms(timed_mutex & inMutex)
{
	auto _500ms = chrono::microseconds(500);
	if (inMutex.try_lock_for(_500ms))
	{
		cout << "获得了锁" << endl;
	}
	else
	{
		cout << "未获得锁" << endl;
	}
}

int Solution::mutexTest2()
{
	timed_mutex testMutex;
	
	testMutex.lock();
	thread threadTest(run500ms, ref(testMutex));
	threadTest.join();
	testMutex.unlock();

	system("pause");

	return 0;
}

mutex Solution::test3Mutex;
//lock_guard的设计保证了即使程序在锁定期间发生了异常，也会安全的释放锁，不会发生死锁
void safeThread()
{
	try
	{
		lock_guard<mutex> lockGuard(Solution::test3Mutex);
		throw logic_error("logic error");
	}
	catch (const exception & ex)
	{
		cerr << "[caught]" << ex.what() << endl;
	}
}

int Solution::mutexTest3()
{
	safeThread();
	//此处仍能上锁
	test3Mutex.lock();
	cout << "Ok, still locked" << endl;
	test3Mutex.unlock();
	system("pause");
	return 0;
}

void read(future<string> *pFuture)
{
	//read会一直阻塞，直到有值到来
	cout << pFuture->get() << endl;
}

int Solution::promiseTest1()
{
	//promise相当于生产者
	promise<string> promiseMain;
	//future相当于消费者，右值构造
	future<string> futureMain = promiseMain.get_future();
	//另一线程中通过future来读取promise的值
	thread threadRead(read, &futureMain);
	//让read等一会
	this_thread::sleep_for(chrono::seconds(1));
	//
	promiseMain.set_value("hello future");
	//等待线程执行完成
	threadRead.join();

	system("pause");

	return 0;
}

void read2(future<int> future)
{
	try
	{
		future.get();
	}
	catch (future_error & e)
	{
		cerr << e.code() << "\n" << e.what() << endl;
	}
}

int Solution::promiseTest2()
{
	thread threadTest;
	{
		//如果promise不设置任何值
		//则在promise析构时会自动设置为future_error
		//这会造成future.get抛出异常
		promise<int> promiseTest;
		threadTest = thread(read2, promiseTest.get_future());
	}

	threadTest.join();

	system("pause");

	return 0;
}

//如果thread通过ref传入参数，则线程函数的参数必须是引用
void catchError(future<void>  & future)
{
	try
	{
		future.get();
	}
	catch (logic_error & e)
	{
		cerr << e.what() << endl;
	}
}


int Solution::promiseTest3()
{
	promise<void> promiseTest;
	future<void> futureTest = promiseTest.get_future();

	thread threadTest(catchError, ref(futureTest));

	//自定义异常需要使用make_exception_ptr转换一下
	promiseTest.set_exception(make_exception_ptr(logic_error("caught")));

	threadTest.join();
	system("pause");
	return 0;
}

int sum(int a, int b)
{
	return a + b;
}

int Solution::packagedTaskTest1()
{
	packaged_task<int(int, int)> task(sum);
	future<int> future = task.get_future();

	//promise一样，packaged_task支持move，但不支持拷贝
	//thread的第一个参数不止是函数，还可以是一个可调用对象，即支持operator()(Args...)操作
	thread t(move(task), 1, 2);
	//等待异步计算结果
	cout << "1 + 2 => " << future.get() << endl;

	t.join();
	system("pause");
	return 0; 
}

int Solution::packagedTaskTest2()
{
	packaged_task<void()> task; //缺省构造、默认构造
	cout << boolalpha << task.valid() << endl; //false
	
	packaged_task<void()> task2(move(task)); //右值构造
	cout << boolalpha << task.valid() << endl; //false

	task = packaged_task<void()>([]() {}); //右值赋值，可调用对象
	cout << boolalpha << task.valid() << endl;

	system("pause");
	return 0;
}


int Solution::packagedTaskTest3()
{
	packaged_task<void()> convert([]() {
		throw logic_error("will catch in future");
	});

	std::future<void> future = convert.get_future();

	convert();

	try
	{
		future.get();
	}
	catch (logic_error & e)
	{
		cerr << typeid(e).name() << ":" << e.what() << endl;
	}

	system("pause");

	return 0;
}

int Solution::packagedTaskTest4()
{
	return 0;
}


Solution::Solution()
{
}


Solution::~Solution()
{
}
