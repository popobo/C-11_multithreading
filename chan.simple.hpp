#pragma once
#include <condition_variable>
#include <list>
#include <mutex>

template <typename T>
class Chan 
{
	class Queue
	{
		//在C++中，mutable也是为了突破const的限制而设置的。被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中。
		mutable std::mutex mMutex;
		std::condition_variable mCondVar;
		std::list<T> mDataList;
		const size_t mCapacity;//mDataList容量
		const bool mEnableOverflow;
		bool mClosed = false;//队列是否已关闭
		size_t mPopCount = 0;//计数，累计pop的数量
	public:
		Queue(size_t capacity) :mCapacity(capacity == 0 ? 1 : capacity), mEnableOverflow(capacity == 0)
		{
		}

		bool isEmpty() const
		{
			return mDataList.empty();
		}

		size_t freeCount() const
		{
			//mCapacity==0时，允许放入一个，但Queue处于overflow状态
			return mCapacity - mDataList.size();
		}

		bool isOverflow() const
		{
			return mEnableOverflow && mDataList.size() >= mCapacity;
		}

		bool isClosed() const
		{
			unique_lock<mutex> lock(this->mMutex);
			return this->mClosed;
		}

		void close()
		{
			unique_lock<mMutex> lock(this->mMutex);
			this->mClosed = true;
			if (this->isOverflow())
			{
				//消除溢出
				this->mDataList.pop_back();
			}
			this->mCondVar.notify_all();
		}

		template <typename TR>
		bool pop(TR & data)
		{
			unique_lock<mMutex> lock(this->mMutex);
			//如果Queue非空或者已关闭
			this->mCondVar.wait(lock, [&]() { return !isEmpty() || mClosed; });
			if (this->isEmpty())
			{
				return false; //已关闭
			}

			data = this->mDataList.front();
			this->mDataList.pop_front();
			this->mPopCount++;

			if (1 == this->freeCount())
			{
				//说明之前时full或溢出状态，则push操作可能在等待
				this->mCondVar.notify_all();
			}

			return true;
		}

		template <typename TR>
		bool push(TR && data)
		{
			unique_lock<mMutex> lock(mMutex);
			//如果有空闲空间
			mCondVar.wait(lock, [&]() { return freeCount() > 0 || mClosed; });
			if (mClosed)
			{
				return false;
			}

			mDataList.push_back(forward<TR>(data));
			if (1 == mDataList.size())
			{
				//说明原来是空的,通知pop可以取了
				mCondVar.notify_all();
			}

			//当queue溢出，需等待queue回复正常
			if (isOverflow())
			{
				const size_t old = this->mPopCount;
				mCondVar.wait(lock, [&]() { return old != mPopCount || mClosed; });
			}

			return !this->close();
		}

		shared_ptr<Queue> mQueue;

	public:
		explicit chan(size_t capacity = 0)
		{
			mQueue = make_shared<Queue>(capacity);
		}

		//支持拷贝
		chan(const chan &) = default;
		chan & operator=(const chan &) = default;
		//支持move
		chan(chan &&) = default;
		chan & operator=(chan &&) = default;

		//入chan,支持move语义
		template <typename TR>
		bool operator<<(TR && data)
		{
			return mQueue->push(forward(TR)(data));
		}

		//出chan(支持兼容类型的出chan)
		template <typename TR>
		bool operator>>(TR & data)
		{
			return mQueue->pop(data);
		}

		//close以后的入chan操作返回false，而出chan则在队列为空后才返回false
		void close()
		{
			mQueue->close();
		}

		void isClosed() const
		{
			return mQueue->isClosed();
		}

	};
};