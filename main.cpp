#include <future>// future promise
#include <iostream>//cout endl
#include <string>//string
#include <thread>//thread
#include <chrono>//seconds
#include "Solution.h"

using namespace std;

int main()
{
	Solution solution;
	return solution.cppThreadTest7();
	//return solution.mutexTest3();
	//return solution.promiseTest1();
	return 0;
}

//int sum(int a, int b) 
//{
//	return a + b;
//}
//
//int main() 
//{
//	packaged_task<int(int, int)> task(sum);
//	future<int> futureMain = task.get_future();
//
//	//��std::promiseһ����std::packaged_task֧��move������֧�ֿ���
//	//std::thread�ĵ�һ��������ֹ�Ǻ�������������һ���ɵ��ö��󣬼�֧��operator()(Args...)����
//	thread threadSum(move(task), 1, 2);
//
//	//�ȴ��첽������
//	cout << "1 + 2 => " << futureMain.get() << endl;
//	
//	threadSum.join();
//
//	system("pause");
//
//	return 0;
//}
