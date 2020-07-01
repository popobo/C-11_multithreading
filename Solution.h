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
	
	//std::promise��һ��ģ����: template<typename R> class promise���䷺�Ͳ���RΪstd::promise���󱣴��ֵ�����ͣ�R������void���͡�std::promise�����ֵ�ɱ���֮������std::future��ȡ����ȡ�������Է����������̡߳�
	int promiseTest1();
	//���promiseֱ������ʱ����δ���ù��κ�ֵ����promise��������ʱ�Զ�����Ϊstd::future_error��������std::future.get�׳�std::future_error�쳣
	int promiseTest2();
	//ͨ��std::promise::set_exception�������������Զ����쳣�����쳣���ջᱻ���ݵ�std::future��������get�����б��׳�
	int promiseTest3();

	int packagedTaskTest1();
	//�ú��������ж�std::packaged_task�����Ƿ�����Ч״̬����ͨ��ȱʡ�����ʼ��ʱ��������δ�����κοɵ��ö��������valid�᷵��false��ֻ�е�std::packaged_task��������Ч�ĺ�����ɵ��ö���valid�ŷ���true
	int packagedTaskTest2();
	// std::packaged_task::operator()(ArgTypes...), �ú��������std::packaged_task��������װ�ɵ��ö���R�����亯��ԭ����R���в�ͬ:void operator()(ArgTypes... );
	int packagedTaskTest3();
	//��std::promise��һ���� std::promise������ִ��һ��set_value��set_exception��������std::packagged_task����ִ�ж�Σ�����ؾ���reset����
	//ͨ�����¹���һ��promise���ﵽ��ε��õ�Ŀ�ġ���Ȼ����reset����Ҫ����get_future���Ա��ȡ�´�operator()ִ�еĽ��
	int packagedTaskTest4();


	Solution();
	~Solution();
};

