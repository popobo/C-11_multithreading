#include "Solution.h"
#include <future>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

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

Solution::Solution()
{
}


Solution::~Solution()
{
}
