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
	
	//std::promise是一个模板类: template<typename R> class promise。其泛型参数R为std::promise对象保存的值的类型，R可以是void类型。std::promise保存的值可被与之关联的std::future读取，读取操作可以发生在其它线程。
	int promiseTest1();
	//如果promise直到销毁时，都未设置过任何值，则promise会在析构时自动设置为std::future_error，这会造成std::future.get抛出std::future_error异常
	int promiseTest2();
	//通过std::promise::set_exception函数可以设置自定义异常，该异常最终会被传递到std::future，并在其get函数中被抛出
	int promiseTest3();

	int packagedTaskTest1();
	//该函数用于判断std::packaged_task对象是否是有效状态。当通过缺省构造初始化时，由于其未设置任何可调用对象或函数，valid会返回false。只有当std::packaged_task设置了有效的函数或可调用对象，valid才返回true
	int packagedTaskTest2();
	// std::packaged_task::operator()(ArgTypes...), 该函数会调用std::packaged_task对象所封装可调用对象R，但其函数原型与R稍有不同:void operator()(ArgTypes... );
	int packagedTaskTest3();
	//与std::promise不一样， std::promise仅可以执行一次set_value或set_exception函数，但std::packagged_task可以执行多次，其奥秘就是reset函数
	//通过重新构造一个promise来达到多次调用的目的。显然调用reset后，需要重新get_future，以便获取下次operator()执行的结果
	int packagedTaskTest4();


	Solution();
	~Solution();
};

