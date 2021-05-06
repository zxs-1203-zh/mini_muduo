#pragma once

#include <mutex>
#include <memory>
#include <condition_variable>
#include <utility>


template<typename T>
class threadsafe_queue{

private:
	struct node{
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};

	mutable std::mutex head_mutex;
	mutable std::mutex tail_mutex;
	std::unique_ptr<node> head;
	struct node *tail;
	std::condition_variable cond;

	node *get_tail(){
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		return tail;
	}

	std::unique_ptr<node> pop_head(){
		std::unique_ptr<node> old_head = std::move(head);
		head = std::move(old_head->next);
		return old_head;
	}

	std::unique_ptr<node> try_pop_head(){
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if(head.get() == get_tail())
		{
			return std::unique_ptr<node>();
		}
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head(T &value){
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if(head.get() == get_tail())
		{
			return std::unique_ptr<node>();
		}
		value = std::move(*(head->data));
		return pop_head();
	}

	std::unique_lock<std::mutex> wait_for_data(){
		std::unique_lock<std::mutex> head_lock(head_mutex);
		cond.wait(head_lock, [&]{return head.get() != get_tail();});
		return head_lock;
	}

	std::unique_lock<std::mutex> wait_pop_head(){
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		return pop_head();
	}

	std::unique_lock<std::mutex> wait_pop_head(T &value){
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		value = std::move(*(head->data));
		return pop_head();
	}

public:
	threadsafe_queue() : head(new node), tail(head.get()) {}
	threadsafe_queue(const threadsafe_queue &) = delete;
	threadsafe_queue& operator=(const threadsafe_queue&) = delete;

	bool empty(){
		std::lock_guard<std::mutex> head_lock(head_mutex);
		return head.get() == get_tail();
	}

	std::shared_ptr<T> try_pop(){
		std::unique_ptr<node> old_head = try_pop_head();
		if(old_head)
		{
			return old_head->data;
		}
		return std::shared_ptr<T>();
	}

	bool try_pop(T &value){
		std::unique_ptr<node> const old_head = try_pop_head(value);
		if(old_head)
		{
			return true;
		}
		return false;
	}

	std::shared_ptr<T> wait_and_pop(){
		const std::unique_ptr<node> old_head = wait_pop_head();
		return old_head->data;
	}

	void wait_and_pop(T &value){
		const std::unique_ptr<node> old_head = wait_pop_head(value);
	}

	void push(T new_value){

		std::shared_ptr<T> new_data = std::make_shared<T>(std::move(new_value));
		std::unique_ptr<node> p(new node);

		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);

			tail->data = new_data;
			node* const new_tail = p.get();
			tail->next = std::move(p);
			tail = new_tail;
		}
		

		cond.notify_one();
	}
};
