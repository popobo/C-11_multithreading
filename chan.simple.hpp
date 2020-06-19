#pragma once
#include <condition_variable>
#include <list>
#include <mutex>

template <typename T>
class Chan 
{
	class Queue
	{
		//��C++�У�mutableҲ��Ϊ��ͻ��const�����ƶ����õġ���mutable���εı���������Զ���ڿɱ��״̬����ʹ��һ��const�����С�
		mutable std::mutex mMutex;
		std::condition_variable mCondVar;
		std::list<T> mDataList;
		const size_t mCapacity;//mDataList����
		const bool mEnableOverflow;
		bool mClosed = false;//�����Ƿ��ѹر�
		size_t mPopCount = 0;//�������ۼ�pop������
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
			//mCapacity==0ʱ���������һ������Queue����overflow״̬
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
				//�������
				this->mDataList.pop_back();
			}
			this->mCondVar.notify_all();
		}

		template <typename TR>
		bool pop(TR & data)
		{
			unique_lock<mMutex> lock(this->mMutex);
			//���Queue�ǿջ����ѹر�
			this->mCondVar.wait(lock, [&]() { return !isEmpty() || mClosed; });
			if (this->isEmpty())
			{
				return false; //�ѹر�
			}

			data = this->mDataList.front();
			this->mDataList.pop_front();
			this->mPopCount++;

			if (1 == this->freeCount())
			{
				//˵��֮ǰʱfull�����״̬����push���������ڵȴ�
				this->mCondVar.notify_all();
			}

			return true;
		}

		template <typename TR>
		bool push(TR && data)
		{
			unique_lock<mMutex> lock(mMutex);
			//����п��пռ�
			mCondVar.wait(lock, [&]() { return freeCount() > 0 || mClosed; });
			if (mClosed)
			{
				return false;
			}

			mDataList.push_back(forward<TR>(data));
			if (1 == mDataList.size())
			{
				//˵��ԭ���ǿյ�,֪ͨpop����ȡ��
				mCondVar.notify_all();
			}

			//��queue�������ȴ�queue�ظ�����
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

		//֧�ֿ���
		chan(const chan &) = default;
		chan & operator=(const chan &) = default;
		//֧��move
		chan(chan &&) = default;
		chan & operator=(chan &&) = default;

		//��chan,֧��move����
		template <typename TR>
		bool operator<<(TR && data)
		{
			return mQueue->push(forward(TR)(data));
		}

		//��chan(֧�ּ������͵ĳ�chan)
		template <typename TR>
		bool operator>>(TR & data)
		{
			return mQueue->pop(data);
		}

		//close�Ժ����chan��������false������chan���ڶ���Ϊ�պ�ŷ���false
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