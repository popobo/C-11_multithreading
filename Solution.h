#pragma once

#include <mutex>

using namespace std;

class Solution
{
public:
	int cppThreadTest1();
	//���̴߳��ݲ���
	int cppThreadTest2();
	//���Ա������Ϊ�߳����
	int cppThreadTest3();
	//�߳���ͣ
	int cppThreadTest4();
	//����
	int cppThreadTest5();
	//call_once
	int cppThreadTest6();
	
	int cppThreadTest7();

	//mutex �������Ҳ����õĻ�����
	//recursive_mutex ͬһ�߳��ڿɵݹ�Ļ�����
	//timed_mutex ���߱�mutex�����⣬���ṩ�˴�ʱ����������������
	//recursive_timed_mutex ͬһ�߳��ڿɵݹ��timed_mutex
	int mutexTest1();
	//try_lock_for��ش���
	int mutexTest2();
	//�˴�ֻ������
	static mutex test3Mutex;
	//local_guard
	int mutexTest3();
	//unique_lockӵ�ж�Mutex������Ȩ��һ����ʼ����unique_lock����ͽӹ��˸�mutex, ��unique_lock������������ǰ(����ǰ)�������ط��Ͳ�Ҫ��ֱ��ʹ�ø�mutex�ˡ�
	
	int promiseTest1();

	Solution();
	~Solution();
};

