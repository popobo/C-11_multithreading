//���õ���https://github.com/hcj116/cpp11thread/blob/master/code/chan.h
/**
 * chan��ʹ��:
 * 1. ��������:
 *   1.1 chan<T> ch(0): �޻�����У�push����ֱ��popȡ�����ݣ���ch�رա�
 *   1.2 chan<T> ch(N): N>0, ����N���������Ķ��У�ֻ�е�������ʱpush�Ż�������
 * 2. ����������
 *   2.1 chan<T> ch(N, discard_old): ����N����������ʵʱ����, ��N==0ʱ���г���Ϊ1��
 *                                   push����������������ʱ����push�����ݻ��滻�����ϵ����ݡ�
 *   2.2 chan<T> ch(N, discard): ����N���������Ķ��У���N==0ʱ���г���Ϊ1��
 *                               push����������������ʱ����push�����ݻ�ʧ�ܡ�
 */
#pragma once
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200)
#endif

#include <condition_variable>  // std::condition_variable
#include <memory>              // unique_ptr
#include <mutex>               // std::mutex
#include <vector>
#include <cassert>
#ifndef CHAN_MAX_COUNTER
#    include <limits>  // std::numeric_limits
 // VC��max����: https://www.cnblogs.com/cvbnm/articles/1947743.html
#    define CHAN_MAX_COUNTER (std::numeric_limits<size_t>::max)()
#endif

// ������ʱ��push����
enum class push_policy : unsigned char {
	blocking,     // ����, ֱ�������ڳ��ռ�
	discard_old,  // �����������������ݣ�������
	discard,      // ������ǰ��chan��ֵ��������false��������
};

namespace ns_chan {
	// ���⾪Ⱥ
	class cv_t {
		std::condition_variable cv_;
		uint32_t thread_count_ = 0;
		uint32_t wait_count_ = 0;

	public:
		template <class Predicate>
		void wait(std::unique_lock<std::mutex> &lock, Predicate pred) {
			if (!pred()) {
				++thread_count_;
				do {
					++wait_count_;
					cv_.wait(lock);
				} while (!pred());
				--thread_count_;
			}
		}

		void notify_one() {
			if (wait_count_ > 0) {
				wait_count_ = (wait_count_ > thread_count_ ? thread_count_ : wait_count_) - 1;
				cv_.notify_one();
			}
		}
		void notify_all() {
			wait_count_ = 0;
			cv_.notify_all();
		}
	};

	template<typename T>
	class queue_t {
		mutable std::mutex mutex_;
		cv_t cv_push_;
		cv_t cv_pop_;
		std::condition_variable *const cv_overflow_;
		const size_t capacity_;     // _data����
		const push_policy policy_;  // ������ʱ��push����
		bool closed_ = false;  // �����Ƿ��ѹر�
		size_t first_ = 0;     // �����еĵ�һ������
		size_t new_ = 0;       // �����ݵĲ���λ�ã�first_==new_����Ϊ��
		T data_[0];            // T data_[capacity_]
	private:
		//capacity_����Ϊ1
		//���capacity_Ϊ0��cv_overflow_Ϊnew std::condition_variable()����Ϊnullptr
		queue_t(size_t capacity, push_policy policy)
			: capacity_(capacity == 0 ? 1 : capacity),
			policy_(policy),
			cv_overflow_(capacity == 0 ? new std::condition_variable() : nullptr) {
		}
	public:
		queue_t(const queue_t &) = delete;//���θ��ƹ��캯��
		queue_t(queue_t &&) = delete;//�����ƶ����캯��
		queue_t &operator=(const queue_t &) = delete;//
		queue_t &operator=(queue_t &&) = delete;//

		~queue_t() {
			//�ͷ���Դ
			for (; first_ < new_; first_++) {
				data(first_).~T();
			}
			delete cv_overflow_;
		}

		// close�Ժ����chan�����᷵��false, ����chan���ڶ���Ϊ�պ󣬲ŷ���false
		void close() {
			std::unique_lock<std::mutex> lock(mutex_);
			closed_ = true;
			if (cv_overflow_ != nullptr && !is_empty()) {
				// �������
				data(--new_).~T();
				cv_overflow_->notify_all();
			}
			cv_push_.notify_all();
			cv_pop_.notify_all();
		}

		bool is_closed() const {
			std::unique_lock<std::mutex> lock(mutex_);
			return closed_;
		}

		// ��chan��֧��move����
		template <typename TR>
		bool push(TR &&data) {
			std::unique_lock<std::mutex> lock(mutex_);
			cv_push_.wait(lock, [&]() { return policy_ != push_policy::blocking || free_count() > 0 || closed_; });
			if (closed_) {
				return false;
			}

			if (!push_thread_unsafe(std::forward<TR>(data))) {
				return false;
			}

			cv_pop_.notify_one();
			if (cv_overflow_ != nullptr) {
				const size_t old = first_;
				cv_overflow_->wait(lock, [&]() { return old != first_ || closed_; });
			}

			return !closed_;
		}

		bool pop(std::function<void(T &&data)> consume) {
			std::unique_lock<std::mutex> lock(mutex_);
			cv_pop_.wait(lock, [&]() { return !is_empty() || closed_; });
			if (is_empty()) {
				return false;  // �ѹر�
			}

			T &target = data(first_++);
			consume(std::move(target));
			target.~T();

			if (cv_overflow_ != nullptr) {
				cv_overflow_->notify_one();
			}
			cv_push_.notify_one();

			return true;
		}

		static std::shared_ptr<queue_t> make_queue(size_t capacity, push_policy policy) {
			if (policy != push_policy::blocking && capacity == 0) {
				capacity = 1;
			}
			const size_t size = sizeof(queue_t) + sizeof(T) * (capacity == 0 ? 1 : capacity);
			// ֻ������ģʽ�²Ŵ��ڡ�������ȴ���
			return std::shared_ptr<queue_t>(new (new char[size]) queue_t(capacity, policy), [](queue_t *q) {
				q->~queue_t();
				delete[](char *) q;
			});
		}

	private:
		template <typename TR>
		bool push_thread_unsafe(TR &&d) {
			if (free_count() > 0) {
				new (&data(new_++)) T(std::forward<TR>(d));
			}
			else if (policy_ == push_policy::discard_old) {
				first_++;  // �滻�����ϵ�
				data(new_++) = std::forward<TR>(d);
			}
			else {
				assert(policy_ == push_policy::discard);
				return false;  // ȡ���˴β���, ����is_closed���ж��Ƿ��ѹر�
			}
			// ��_first��_new���
			if (new_ >= CHAN_MAX_COUNTER) {
				reset_pos();
			}

			return true;
		}
		size_t free_count() const {
			return first_ + capacity_ - new_;
		}
		bool is_empty() const {
			return first_ >= new_;
		}
		T &data(size_t pos) {
			return data_[pos % capacity_];
		}

		void reset_pos() {
			const size_t new_first = (this->first_ % this->capacity_);
			this->new_ -= (this->first_ - new_first);
			this->first_ = new_first;
		}

	};
}

template <typename T>
class chan {
	struct data_t {
		std::vector<std::shared_ptr<ns_chan::queue_t<T> > > queue_;
		std::atomic<unsigned int> push_{ 0 }, pop_{ 0 };
	};

	std::shared_ptr<data_t> data_;

public:
	//sizeof(queue__t) = 216, sizeof(cv) = 48/56, sizeof(mutex) = 64
	// ѡȡ�ʵ���concurrent_shift��capacity��chan��������ɴ�ǧ��/��
	explicit chan(size_t concurrent_shift, size_t capacity, push_policy policy = push_policy::blocking) {
		data_ = std::make_shared<data_t>();
		data_->queue_.resize(1 << concurrent_shift);
		for (auto &r : data_->queue_) {
			r = ns_chan::queue_t<T>::make_queue(capacity, policy);
		}
	}

	explicit chan(size_t capacity = 0, push_policy policy = push_policy::blocking)
		: chan(0, capacity, policy) {
	}

	// ֧�ֿ���
	chan(const chan &) = default;
	chan &operator=(const chan &) = default;
	// ֧��move
	chan(chan &&) = default;
	chan &operator=(chan &&) = default;

	// ��chan��֧��move����
	template <typename TR>
	bool operator<<(TR &&data) {
		unsigned int index = data_->push_.fetch_add(1, std::memory_order_acq_rel);
		return data_->queue_[index % length()]->push(std::forward<TR>(data));
	}
	template <typename TR>
	bool push(TR &&data) {
		unsigned int index = data_->push_.fetch_add(1, std::memory_order_acq_rel);
		return data_->queue_[index % length()]->push(std::forward<TR>(data));
	}

	void close() {
		for (size_t i = 0; i < length(); i++) {
			data_->queue_[i]->close();
		}
	}

	bool is_closed() const {
		return data_->queue_[0]->is_closed();
	}

	// ��chan
	template <typename TR>
	bool operator>>(TR &d) {
		unsigned int index = data_->pop_.fetch_add(1, std::memory_order_acq_rel);
		return data_->queue_[index % length()]->pop([&d](T &&target) { d = std::forward<T>(target); });
	}

	// ���ܽ�operator>>�Բ���ⲿ������������
	// ������falseʱ����chan�ѹر�: while(d = ch.pop()){}
	std::unique_ptr<T> pop() {
		unsigned int index = data_->pop_.fetch_add(1, std::memory_order_acq_rel);
		std::unique_ptr<T> d;
		data_->queue_[index % length()]->pop([&d](T &&target) { d.reset(new T(std::forward<T>(target))); });
		return d;
	}
private:
	size_t length() const {
		return data_->queue_.size();
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif