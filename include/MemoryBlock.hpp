#pragma once

#include <string>

#ifdef COMPILE_WITH_CUDA
#include <cuda_runtime.h>
#endif
#include <system_error>

#if defined(__CUDACC__) && defined(__CUDA_ARCH__)
#define _CPU_AND_GPU_CODE_ __device__    // for CUDA device code
#else
#define _CPU_AND_GPU_CODE_
#endif

namespace Util {
#ifdef COMPILE_WITH_CUDA
#define cudaSafeCall(err) __cudaSafeCall(err, __FILE__, __LINE__)
    inline void __cudaSafeCall( cudaError err, const char *file, const int line )
    {
        if( cudaSuccess != err) {
            printf("%s(%i) : cudaSafeCall() Runtime API error : %s.\n",
                   file, line, cudaGetErrorString(err) );
            exit(-1);
        }
    }
#endif
    
    enum MemoryDataType {CPU, CUDA};
    enum MemoryCopyDirection { CPU_TO_CPU, CPU_TO_CUDA, CUDA_TO_CPU, CUDA_TO_CUDA };
    template <typename T>
    class MemoryBlock {
    protected:
        bool hasAllocated_CPU, hasAllocated_CUDA;
        T *data_cpu;
        T *data_cuda;
        size_t size_;
    public:
        
        
        MemoryBlock(size_t size, bool cpu, bool cuda):
        data_cpu(NULL), data_cuda(NULL)
        {
            Allocate(size, cpu, cuda);
        }
        MemoryBlock(T *data, size_t size, bool cpu, bool cuda)
        {
            
        }
        
        
        virtual ~MemoryBlock()
        {
            this->Free();
        }
        
        
        template <MemoryDataType datatype>
        inline T* data(){
            switch (datatype){
                case CPU: return data_cpu;
                case CUDA: return data_cuda;
            }
        }
        
        template <MemoryDataType datatype>
        inline const T* data() const{
            switch (datatype){
                case CPU: return data_cpu;
                case CUDA: return data_cuda;
            }
        }
        
        size_t size() {return size_;}
        
        // Suppress the default copy constructor and assignment operator
        MemoryBlock(const MemoryBlock&);
        MemoryBlock& operator=(const MemoryBlock &other) {
            Free();
            this->allocate(other->size_, other.hasAllocated_CPU, other.hasAllocated_CUDA);
            this->SetFrom(other, CPU_TO_CPU);
            this->SetFrom(other, CUDA_TO_CUDA);
        }
        
        void UpdateDeviceFromHost() const {
#ifdef COMPILE_WITH_CUDA
            if (hasAllocated_CUDA && hasAllocated_CPU)
                cudaSafeCall(cudaMemcpy(data_cuda, data_cpu, size_ * sizeof(T), cudaMemcpyHostToDevice));
            else
                printf("called UpdateHostFromDevice but not both memories are allocated: {hasCPU:hasCUDA} = {%d, %d}\n", hasAllocated_CPU, hasAllocated_CUDA);
#else
            std::runtime_error("called UpdateHostFromDevice but the system is not compiled with CUDA\n");
#endif
        }
        /** Transfer data from GPU to CPU, if possible. */
        void UpdateHostFromDevice() const {
#ifdef COMPILE_WITH_CUDA
            if (hasAllocated_CUDA && hasAllocated_CPU)
                cudaSafeCall(cudaMemcpy(data_cpu, data_cuda, size_ * sizeof(T), cudaMemcpyDeviceToHost));
            else
                printf("called UpdateHostFromDevice but not both memories are allocated: {hasCPU:hasCUDA} = {%d, %d}\n", hasAllocated_CPU, hasAllocated_CUDA);
#else
            std::runtime_error("called UpdateHostFromDevice but the system is not compiled with CUDA\n");
#endif
        }
        
        void SetTo(const T source, MemoryDataType type) //TODO: Test me
        {
            switch (type)
            {
                case CPU:
                    memset(this->data_cpu, source, this->size_ * sizeof(T));
                    break;
#ifdef COMPILE_WITH_CUDA
                case CUDA:
                    cudaSafeCall(cudaMemsetAsync(this->data_cuda, source, this->size_ * sizeof(T)));
                    break;
#endif
                default:
                    std::runtime_error("called SetTo with CopyDirection related to CUDA. But the system is not compiled with CUDA\n");
                    break;
            }
        }
        
        void SetFrom(const MemoryBlock<T> *source, MemoryCopyDirection memoryCopyDirection)
        {
            Resize(source->size_);
            switch (memoryCopyDirection)
            {
                case CPU_TO_CPU:
                    memcpy(this->data_cpu, source->data_cpu, source->size_ * sizeof(T));
                    break;
#ifdef COMPILE_WITH_CUDA
                case CPU_TO_CUDA:
                    ORcudaSafeCall(cudaMemcpyAsync(this->data_cuda, source->data_cpu, source->size_ * sizeof(T), cudaMemcpyHostToDevice));
                    break;
                case CUDA_TO_CPU:
                    ORcudaSafeCall(cudaMemcpy(this->data_cpu, source->data_cuda, source->size_ * sizeof(T), cudaMemcpyDeviceToHost));
                    break;
                case CUDA_TO_CUDA:
                    ORcudaSafeCall(cudaMemcpyAsync(this->data_cuda, source->data_cuda, source->size_ * sizeof(T), cudaMemcpyDeviceToDevice));
                    break;
#endif
                default:
                    std::runtime_error("called SetFrom with CopyDirection related to CUDA. But the system is not compiled with CUDA\n");
                    break;
                    
            }
        }
        
        /** Resize a memory block, losing all old data.
         Essentially any previously allocated data is
         released, new memory is allocated.
         */
        void Resize(size_t newDataSize, bool forceReallocation = true)
        {
            if(newDataSize == size_) return;
            
            if(newDataSize > size_ || forceReallocation)
            {
                bool allocate_CPU = this->hasAllocated_CPU;
                bool allocate_CUDA = this->hasAllocated_CUDA;
                this->Free();
                this->Allocate(newDataSize, allocate_CPU, allocate_CUDA);
            }
            
            this->size_ = newDataSize;
        }
    private:
        void Allocate(size_t size, bool cpu, bool cuda)
        {
            this->Free();
            this->size_ = size;
            
            if(cpu){
                if(size == 0) this->data_cpu = NULL;
#ifdef COMPILE_WITH_CUDA
                else cudaSafeCall(cudaMallocHost((void**)&data_cpu, this->size_ * sizeof(T)));
#else
                else this->data_cpu = new T[size];
#endif
                this->hasAllocated_CPU = true;
            }
            
            if(cuda){
#ifdef COMPILE_WITH_CUDA
                if(size == 0) this->data_cuda = NULL;
                else cudaSafeCall(cudaMalloc((void**)&data_cuda, size * sizeof(T)));
                this->hasAllocated_CUDA = true;
#else
                std::runtime_error("called Allocate with cuda enabled. But the system is not compiled with CUDA\n");
#endif
            }
        }
        
        void Free(){
            if(this->hasAllocated_CPU){
                if (data_cpu != NULL){
#ifdef COMPILE_WITH_CUDA
                    if (data_cpu != NULL) cudaSafeCall(cudaFreeHost(data_cpu));
#else
                    if (data_cpu != NULL) delete[] data_cpu;
#endif
                }
                this->hasAllocated_CPU = false;
            }
            
            if(this->hasAllocated_CUDA){
#ifdef COMPILE_WITH_CUDA
                if (data_cuda != NULL) cudaSafeCall(cudaFree(data_cuda));
#endif
                this->hasAllocated_CUDA = false;
            }
        }
    };

}
