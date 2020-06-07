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
	return solution.cppThreadTest4();
	
	//return solution.promiseTest1();
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
//	//和std::promise一样，std::packaged_task支持move，但不支持拷贝
//	//std::thread的第一个参数不止是函数，还可以是一个可调用对象，即支持operator()(Args...)操作
//	thread threadSum(move(task), 1, 2);
//
//	//等待异步计算结果
//	cout << "1 + 2 => " << futureMain.get() << endl;
//	
//	threadSum.join();
//
//	system("pause");
//
//	return 0;
//}
