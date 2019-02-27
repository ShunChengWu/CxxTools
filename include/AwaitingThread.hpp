//
//  AwaitingThread.h
//  INSEG
//
//  Created by Shun-Cheng Wu on 05/10/2018.
//

#ifndef AwaitingThread_h
#define AwaitingThread_h

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace tools {
    /**
     known issure:
     using array type like a[n] with reference lambda input i.e. pool.runTaskWithID(std::bind([&](){ ... })) will behaive differently.
     maybe is the n captured using lambda is changed alreadly when the kernel run.
     */
    class AwaitingThread{
        struct task_element_t {
            bool run_with_id;
            const std::function< void() > no_id;
            const std::function< void(std::size_t) > with_id;
            
            explicit task_element_t(const std::function< void() >& f) :
            run_with_id(false), no_id(f), with_id(nullptr) { }
            explicit task_element_t(const std::function< void(std::size_t) >& f) :
            run_with_id(true), no_id(nullptr), with_id(f) { }
        };
        std::queue<task_element_t> tasks_;
        std::thread* thread_;
        std::mutex mutex_;
        std::condition_variable condition_;
        std::condition_variable completed_;
        bool available_;
        bool running_;
        bool complete_;
        bool pause_;
    public:
        /// @brief Constructor.
        explicit AwaitingThread()
        :  thread_(0), available_(false), running_(true), complete_(true), pause_(false) {
            if(thread_ == 0)
                thread_ = new std::thread(std::bind(&AwaitingThread::main_loop, this, 0));
        }
        
        /// @brief Destructor.
        ~AwaitingThread() {
            // Set running flag to false then notify all threads.
            {
                std::unique_lock< std::mutex > lock(mutex_);
                running_ = false;
                condition_.notify_all();
            }
            
            try {
                thread_->join();
            }
            // Suppress all exceptions.
            catch (const std::exception&) {}
        }
        
        /// @brief Add task to the thread pool if a thread is currently available.
        template <typename Task>
        void runTask(Task task) {
            std::unique_lock<std::mutex> lock(mutex_);
            
            // Set task and signal condition variable so that a worker thread will
            // wake up and use the task.
            tasks_.push(task_element_t(static_cast<std::function< void() >>(task)));
            complete_ = false;
            condition_.notify_one();
        }
        
        template <typename Task>
        void runTaskWithID(Task task) {
            std::unique_lock<std::mutex> lock(mutex_);
            
            // Set task and signal condition variable so that a worker thread will
            // wake up and use the task.
            tasks_.push(task_element_t(static_cast<std::function< void(std::size_t) >>(task)));
            complete_ = false;
            condition_.notify_one();
        }
        
        /// @brief Wait for queue to be empty
        void waitWorkComplete() {
            std::unique_lock<std::mutex> lock(mutex_);
            while (!complete_)
                completed_.wait(lock);
        }
        
        /**
         Wait for queue to be empty while showing the progress
         @param total_num the total size of input was passed into the pool
         @param display_time The interval of displaying estimation time left. (second)
         */
        void waitWorkCompleteandShow(size_t total_num, long long display_time = 1){
            float time_past = display_time;
            while(checkTaskLeft()){
                std::this_thread::sleep_for(std::chrono::seconds(display_time));
                int current = checkTaskLeft();
                int proceed = total_num-current;
                float sec_left = time_past/proceed*current;
                time_past += display_time;
                printf("%5.2f%%[%d/%zu]ET: %4.0f(s) \n", float(proceed)/total_num*100, proceed, total_num, sec_left);
            }
        }
        
        size_t checkTaskLeft(){return tasks_.size();}
        
        /** The pause funciton will wait for remaining job to complete and pause, until the function is called again with false value.  */
        void pause(bool option){
            pause_ = option;
            if(pause_){
                while(available_){
                    // wait...;
                }
            }
        }
        
    private:
        void main_loop(std::size_t index) {
            while (running_) {
                while (pause_){
                    
                }
                std::unique_lock<std::mutex> lock(mutex_);
                while (tasks_.empty() && running_) {
                    condition_.wait(lock);
                }
                
                if (!running_) break;
                {
                    auto tasks = tasks_.front();
                    tasks_.pop();
                    // Decrement count, indicating thread is no longer available.
                    available_ = false;
                    
                    lock.unlock();
                    
                    // Run the task.
                    try {
                        if (tasks.run_with_id) {
                            tasks.with_id(index);
                        } else {
                            tasks.no_id();
                        }
                    }
                    // Suppress all exceptions.
                    catch ( const std::exception& ) {}
                    
                    // Update status of empty, maybe
                    // Need to recover the lock first
                    lock.lock();
                    
                    // Increment count, indicating thread is available.
                    available_ = true;
                    
                    if (tasks_.empty() && available_ == true) {
                        complete_ = true;
                        completed_.notify_one();
                    }
                }
            }  // while running_
        }
    };
}

#endif /* AwaitingThread_h */
