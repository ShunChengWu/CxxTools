#pragma  once
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <functional>
namespace tools {


    template<class OutType>
    class DataLoader {
    public:
        virtual std::shared_ptr<OutType> get_item() = 0;
        virtual size_t dataSize() = 0;
        virtual bool next() = 0;
    };

    template<class OutType>
    class DataWorker {
    public:
        explicit DataWorker(DataLoader<OutType> *loader, size_t buffer_size) :
                loader_(loader), buffer_size_(buffer_size), terminate_(false) {
            thread_ = std::async(std::launch::async, std::bind(&DataWorker<OutType>::process, this));
        }

        ~DataWorker() {
            Stop();
            condition_get_.notify_all();
            condition_process_.notify_all();
            condition_buffer_.notify_all();
            while (!thread_.valid()) {
            }
        }

        void Stop() {
            terminate_ = true;
        }

        void process() {
            while(true) {
                std::unique_lock<std::mutex> lock_process(mutex_process_);
                while (buffer_.size() >= buffer_size_ && !terminate_) {
                    //printf("[thread] wait.\n");
                    condition_process_.wait(lock_process);
                }
                if (terminate_) break;

                auto tmp = loader_->get_item();
                std::unique_lock<std::mutex> lock_buffer(mutex_buffer_);
                buffer_.push(tmp);
                condition_get_.notify_one();
                lock_buffer.unlock();
                if(!loader_->next()) break;
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        std::shared_ptr<OutType> get() {
            if (thread_.valid()) {
                std::unique_lock<std::mutex> lock_get(mutex_get_);
                while (buffer_.empty()) {
                    //printf("[main] wait!\n");
                    condition_get_.wait(lock_get);
                    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                if (!terminate_) {
                    std::unique_lock<std::mutex> lock_buffer(mutex_buffer_);
                    auto buffer = buffer_.front();
                    buffer_.pop();
                    //printf("size--[%zu]\n", buffer_.size());
                    lock_buffer.unlock();
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                    if (buffer_.size() < buffer_size_) {
                        //printf("[main] notify thread.\n");
                        //std::unique_lock<std::mutex> lock_process(mutex_process_);
                        condition_process_.notify_one();
                    }
                    return buffer;
                } else if(!buffer_.empty()){
                    auto buffer = buffer_.front();
                    buffer_.pop();
                    return buffer;
                }
            } else if(!buffer_.empty()){
                auto buffer = buffer_.front();
                buffer_.pop();
                return buffer;
            }
            return nullptr;
        }

    private:
        DataLoader<OutType> *loader_;
        unsigned char buffer_size_;
        std::queue<std::shared_ptr<OutType>> buffer_;
        std::future<void> thread_;
        std::atomic_bool terminate_;
        std::condition_variable condition_get_, condition_process_, condition_buffer_;
        std::mutex mutex_get_, mutex_process_, mutex_buffer_;
    };

}