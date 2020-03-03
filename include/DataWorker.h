#pragma  once
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <functional>
#include "thread_pool.hpp"
namespace tools {


    template<class OutType>
    class DataLoader {
    public:
        virtual std::shared_ptr<OutType> get_item(int idx) = 0;
        virtual size_t dataSize() = 0;
        /// get current idx and move to next. if return -1 means reach the end of data
        virtual int next() = 0;
    };

    template<class OutType>
    class DataWorker {
    public:
        explicit DataWorker(DataLoader<OutType> *loader, size_t buffer_size) :
                pool(buffer_size),hasMore_(true), working(0),needMore_(true), waitData_(true), loader_(loader), buffer_size_(buffer_size), terminate_(false) {
//            thread_ = std::async(std::launch::async, std::bind(&DataWorker<OutType>::process, this));
//            pool.runTaskWithID(std::bind(&DataWorker<OutType>::process, this));
            for(size_t i=0;i<buffer_size;++i) {
                auto idx = loader_->next();
                if(idx>=0)
                    pool.runTaskWithID(std::bind(&DataWorker<OutType>::kernel, this, idx));
                else
                    hasMore_=false;
            }
        }

        ~DataWorker() {
            Stop();
            condition_get_.notify_all();
            condition_process_.notify_all();
            condition_buffer_.notify_all();
//            while (!thread_.valid()) {
//            }
        }

        void Stop() {
            terminate_ = true;
        }

        void kernel(int idx){
            working++;
            auto tmp = loader_->get_item(idx);
            {
                std::unique_lock<std::mutex> lock_buffer(mutex_buffer_);
                buffer_.push(tmp);
            }
            {
                std::unique_lock<std::mutex> lock_get(mutex_get_);
                waitData_=false;
            }
            working--;
            condition_get_.notify_one();
        }

        void process() {
            while(true) {
                std::unique_lock<std::mutex> lock_process(mutex_process_);
                if (buffer_.size() >= buffer_size_ && !terminate_) {
                    needMore_=false;
                    condition_process_.wait(lock_process, [&] { return needMore_; });
                }
//                while (buffer_.size() >= buffer_size_ && !terminate_) {
//                }
                if (terminate_) {
                    condition_get_.notify_one();
                    break;
                }

                int idx;
                {
                    std::unique_lock<std::mutex> lock_getData(mutex_geIdx_);
                    idx = loader_->next();
                }
                if(idx<0){
                    condition_get_.notify_one();
                    break;
                }

                auto tmp = loader_->get_item(idx);
                {
                    std::unique_lock<std::mutex> lock_buffer(mutex_buffer_);
                    buffer_.push(tmp);
                }

                {
                    std::unique_lock<std::mutex> lock_get(mutex_get_);
                    waitData_=false;
                }
                condition_get_.notify_one();
            }
        }

        std::shared_ptr<OutType> get() {
            std::unique_lock<std::mutex> lock_get(mutex_get_);
            if(buffer_.empty() && !terminate_ && hasMore_) {
                waitData_=true;
                condition_get_.wait(lock_get, [&]{return waitData_;});
            }

            if(terminate_ || buffer_.empty()) return nullptr;

            std::unique_lock<std::mutex> lock_buffer(mutex_buffer_);
            auto buffer = buffer_.front();
            buffer_.pop();
            lock_buffer.unlock();


            if (buffer_.size() < buffer_size_ && hasMore_) {
                auto idx = loader_->next();
                if(idx>=0) pool.runTaskWithID(std::bind(&DataWorker<OutType>::kernel, this, idx));
                else hasMore_=false;
//                {
//                    std::unique_lock<std::mutex> lock_process(mutex_process_);
//                    needMore_=true;
//                }
//                condition_process_.notify_one();
            }
            return buffer;
        }

    private:
        tools::TaskThreadPool pool;
        std::atomic_int working;
        bool hasMore_;
        bool needMore_, waitData_;
        DataLoader<OutType> *loader_;
        unsigned char buffer_size_;
        std::queue<std::shared_ptr<OutType>> buffer_;
        std::future<void> thread_;
        std::atomic_bool terminate_;
        std::condition_variable condition_get_, condition_process_, condition_buffer_;
        std::mutex mutex_get_, mutex_process_, mutex_buffer_, mutex_geIdx_;
    };

}