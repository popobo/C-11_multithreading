#pragma once

#include <mutex>

using namespace std;

class Solution
{
public:
	int cppThreadTest1();
	//向线程传递参数
	int cppThreadTest2();
	//类成员函数作为线程入口
	int cppThreadTest3();
	//线程暂停
	int cppThreadTest4();
	//拷贝
	int cppThreadTest5();
	//call_once
	int cppThreadTest6();
	
	int cppThreadTest7();

	//mutex 最基本的也是最常用的互斥类
	//recursive_mutex 同一线程内可递归的互斥类
	//timed_mutex 除具备mutex功能外，还提供了带时限请求锁定的能力
	//recursive_timed_mutex 同一线程内可递归的timed_mutex
	int mutexTest1();
	//try_lock_for相关代码
	int mutexTest2();
	//此处只是声明
	static mutex test3Mutex;
	//local_guard
	int mutexTest3();
	//unique_lock拥有对Mutex的所有权，一但初始化了unique_lock，其就接管了该mutex, 在unique_lock结束生命周期前(析构前)，其它地方就不要再直接使用该mutex了。
	
	int promiseTest1();

	Solution();
	~Solution();
};

