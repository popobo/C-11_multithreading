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
	thread threadMain(foo); //�����߳�foo
	threadMain.join(); //�ȴ��߳�ִ�����

	system("pause");

	return 0;
}

void hello(const string & name)
{
	cout << "Hello " << name << endl;
}

int Solution::cppThreadTest2()
{
	//string & name = "C++11";//����
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

//���Ա������Ϊ�߳����ʱ����Ȼʮ�ּ�: ��this��Ϊ��һ���������ݽ�ȥ����
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
	//sleep ����ָ��ʱ���
	this_thread::sleep_until(chrono::system_clock::now() + chrono::milliseconds(500));
}

int Solution::cppThreadTest4()
{
	thread threadMain(pauseThread);
	threadMain.join();

	system("pause");
	return 0;
}

//����foo�߳�����threadA��������threadB������������threadB������
//std::thread�����Ϊֻ����һ��ʵ����ά���߳�״̬���Լ����߳̽��в�������˵�������ֵ����ʱ���ᷢ���߳�����Ȩת�ơ�
int Solution::cppThreadTest5()
{
	thread threadA(foo);
	thread threadB;
	
	//C++11֧�ֿ�����������֮��İ汾
	//threadB = threadA;//����
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
	//�ú��������ù���˼�壺��֤call_once���õĺ���ֻ��ִ��һ�Ρ�
	call_once(of, initialize);
}

// �����һ�Σ�initialize
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
	//������
	thread consumers[5];
	for (int i = 0; i < sizeof(consumers) / sizeof(consumers[0]); i++)
	{
		consumers[i] = thread(consume, chInt, i + 1);
	}

	//��������
	for (int i = 0; i < 16 ; i++)
	{
		chInt << i;
	}
	chInt.close(); //�����������

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
		cout << "�������" << endl;
	}
	else
	{
		cout << "δ�����" << endl;
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
//lock_guard����Ʊ�֤�˼�ʹ�����������ڼ䷢�����쳣��Ҳ�ᰲȫ���ͷ��������ᷢ������
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
	//�˴���������
	test3Mutex.lock();
	cout << "Ok, still locked" << endl;
	test3Mutex.unlock();
	system("pause");
	return 0;
}

void read(future<string> *pFuture)
{
	//read��һֱ������ֱ����ֵ����
	cout << pFuture->get() << endl;
}

int Solution::promiseTest1()
{
	//promise�൱��������
	promise<string> promiseMain;
	//future�൱�������ߣ���ֵ����
	future<string> futureMain = promiseMain.get_future();
	//��һ�߳���ͨ��future����ȡpromise��ֵ
	thread threadRead(read, &futureMain);
	//��read��һ��
	this_thread::sleep_for(chrono::seconds(1));
	//
	promiseMain.set_value("hello future");
	//�ȴ��߳�ִ�����
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
		//���promise�������κ�ֵ
		//����promise����ʱ���Զ�����Ϊfuture_error
		//������future.get�׳��쳣
		promise<int> promiseTest;
		threadTest = thread(read2, promiseTest.get_future());
	}

	threadTest.join();

	system("pause");

	return 0;
}

//���threadͨ��ref������������̺߳����Ĳ�������������
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

	//�Զ����쳣��Ҫʹ��make_exception_ptrת��һ��
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

	//promiseһ����packaged_task֧��move������֧�ֿ���
	//thread�ĵ�һ��������ֹ�Ǻ�������������һ���ɵ��ö��󣬼�֧��operator()(Args...)����
	thread t(move(task), 1, 2);
	//�ȴ��첽������
	cout << "1 + 2 => " << future.get() << endl;

	t.join();
	system("pause");
	return 0; 
}

int Solution::packagedTaskTest2()
{
	packaged_task<void()> task; //ȱʡ���졢Ĭ�Ϲ���
	cout << boolalpha << task.valid() << endl; //false
	
	packaged_task<void()> task2(move(task)); //��ֵ����
	cout << boolalpha << task.valid() << endl; //false

	task = packaged_task<void()>([]() {}); //��ֵ��ֵ���ɵ��ö���
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
