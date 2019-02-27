//
//  HDF5Saver.hpp
//
//  Created by Shun-Cheng Wu on 19/09/2017.
//
//

#ifndef HDF5Saver_hpp
#define HDF5Saver_hpp

#include <iostream>
#include <string>
#include <vector>
#include <utilities/Tools.hpp>
#include <map>
#include "H5Cpp.h"

using std::cout;
using std::endl;

class H5Saver{
    std::map<int, H5::DataType> h5typemap{
        {0, H5::PredType::NATIVE_INT}, {1, H5::PredType::NATIVE_FLOAT},
        {2, H5::PredType::NATIVE_UINT8}
    };
    std::unique_ptr<H5::H5File> file = NULL;
    std::unique_ptr<H5::DataSet> dataset = NULL;
    std::unique_ptr<H5::DataSpace> fspace = NULL;
    std::unique_ptr<H5::DataSpace> mspace = NULL;
    std::unique_ptr<hsize_t[]> start, stride, count, block;
public:
    H5Saver(){};
    H5Saver(std::string filename){ createFile(filename); };
    ~H5Saver(){}
    
    enum h5type { INT = 0, FLOAT = 1, UINT8 = 2 };
    std::map<h5type, unsigned short int> map_h5type_size = {
        {INT, sizeof(int)}, {FLOAT, sizeof(float)}, {UINT8, sizeof(uint8_t)}
    };
    
    struct TYPES{
        std::vector<float> vfloat;
        std::vector<int> vint;
        std::vector<uint8_t> vuint8;
    };
    /**  */
    void savetoH5(std::string filename, std::vector<std::string> dataset_name, std::vector<int> datatypes, std::vector<TYPES> data, std::vector<std::vector<unsigned long long>> dim, std::vector<int> start, std::vector<int> legnth){
        H5Saver saver(filename);
        std::vector<TYPES> trans(data.size());
        for (int d=0;d<data.size();++d){
            if (datatypes[d] == H5Saver::h5type::INT)
                trans[d].vint.assign(data[d].vint.begin()+start[d], data[d].vint.begin()+start[d]+legnth[d]);
            if (datatypes[d] == H5Saver::h5type::FLOAT)
                trans[d].vfloat.assign(data[d].vfloat.begin()+start[d], data[d].vfloat.begin()+start[d]+legnth[d]);
            if (datatypes[d] == H5Saver::h5type::UINT8)
                trans[d].vuint8.assign(data[d].vuint8.begin()+start[d], data[d].vuint8.begin()+start[d]+legnth[d]);
        }
        for (int d=0;d<data.size();++d){
            if (datatypes[d] == H5Saver::h5type::INT)
                saver.createDataset(dataset_name[d], h5type(datatypes[d]), trans[d].vint, dim[d]);
            if (datatypes[d] == H5Saver::h5type::FLOAT)
                saver.createDataset(dataset_name[d], h5type(datatypes[d]), trans[d].vfloat, dim[d]);
            if (datatypes[d] == H5Saver::h5type::UINT8)
                saver.createDataset(dataset_name[d], h5type(datatypes[d]), trans[d].vuint8, dim[d]);
        }
    }
    
    void SaveVec2H5(std::vector<TYPES> data, std::vector<std::string> dataset_names, std::vector<int> datatypes, std::string output_name, int total_num_data, int data_per_file, std::vector<std::vector<int>> data_dim){
        int output_file_num = total_num_data/ data_per_file;
        unsigned long long remainder = total_num_data% (data_per_file);
        std::vector<int> size_per_data(data.size());
        for (int i=0; i< data_dim.size();++i){
            size_per_data[i] = 1;
            for (int r=1; r<data_dim[i].size(); ++r)
                size_per_data[i] *= data_dim[i][r];
        }
        
        tools::timer<float> timer;
        timer.start();
        std::cout<<"Saving data ...\n";
        std::cerr << "Total output file number: " << output_file_num + 1 << std::endl;
        std::cerr << "The last file will contain " << remainder << " number of dataset\n";
        for (int j=0;j<output_file_num+1;++j) {
            if (j < output_file_num) {
                std::vector<std::vector<unsigned long long>> dim(data.size());
                std::vector<int> start(data.size()), length(data.size());
                for (int i=0;i<dim.size();++i){
                    dim[i].push_back(data_per_file);
                    for(int d=1; d< data_dim[i].size();++d)
                        dim[i].push_back(data_dim[i][d]);
                    start [i] = data_per_file * size_per_data[i] * j;
                    length[i] = data_per_file * size_per_data[i];
                    std::cerr<< "Save " << dataset_names[i] << " to file " + output_name + "_" + std::to_string(j) + ".h5" + " with size " << data_per_file << ", ";
                    for (int d=1; d< data_dim[i].size();++d)
                        std::cout << data_dim[i][d] << " ";
                    std::cout << std::endl;
                }
                savetoH5(output_name + "_" + std::to_string(j) + ".h5", dataset_names, datatypes, data, dim, start, length);
            } else {
                if(remainder){
                    std::vector<std::vector<unsigned long long>> dim(data.size());
                    std::vector<int> start(data.size()), length(data.size());
                    for (int i=0;i<dim.size();++i) {
                        dim[i].push_back(remainder);
                        for(int d=1; d< data_dim[i].size();++d)
                            dim[i].push_back(data_dim[i][d]);
                        start [i] = data_per_file * size_per_data[i] * j;
                        length[i] = remainder * size_per_data[i];
                        
                        std::cerr<< "Save " << dataset_names[i] << " to file " + output_name + "_" + std::to_string(j) + ".h5" + " with size " << remainder << ", ";
                        for (int d=1; d< data_dim[i].size();++d)
                            std::cout << data_dim[i][d] << " ";
                        std::cout << std::endl;
                    }
                    savetoH5(output_name + "_" + std::to_string(j) + ".h5", dataset_names, datatypes, data, dim, start, length);
                }
            }
        }
        std::cout<<"Don! time_Saving_data: " <<timer.update_get_eclipse()<<std::endl;
    }
    
    
    void createDataset (std::string datasetname, h5type type, void* data, std::vector<hsize_t> dims){
        H5::DSetCreatPropList plist;
        int fillvalue = 0;
        plist.setFillValue(H5::PredType::NATIVE_INT, &fillvalue);
        
        std::vector<hsize_t> fdim;
        for (auto dim : dims){
            fdim.push_back(dim);
        }
        hsize_t mdim[] = {50};
        fspace = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(fdim.size(), fdim.data()));
        mspace = std::unique_ptr<H5::DataSpace> (new H5::DataSpace(1, mdim));
        dataset = std::unique_ptr<H5::DataSet> (new H5::DataSet(file->createDataSet(datasetname, h5typemap[type], *fspace, plist)));
        //        initialHyperslabParams(data.size());
        initialHyperslabParams(fdim.size());
        //        std::cout << std::endl;
        //        std::cout << fdim.size() << " " << fdim.data()  << std::endl;
        fspace->selectHyperslab(H5S_SELECT_SET, count.get(), start.get(), stride.get(), block.get());
        
        size_t size=1;
        for(auto& d:dims) size*=d;
        
        start[0]    = 1;
        stride[0]   = 1;
        count[0]    = size;
        block[0]    = 1;
        mspace->selectHyperslab(H5S_SELECT_SET, count.get(), start.get(), stride.get(), block.get());
        
        dataset->write(data, h5typemap[type]);
        fspace->selectNone();
    }
    
    template <typename T>
    void createDataset (std::string datasetname, h5type type, std::vector<T> data, std::vector<hsize_t> dims){
        H5::DSetCreatPropList plist;
        int fillvalue = 0;
        plist.setFillValue(H5::PredType::NATIVE_INT, &fillvalue);
        
        std::vector<hsize_t> fdim;
        for (auto dim : dims){
            fdim.push_back(dim);
        }
        hsize_t mdim[] = {50};
        fspace = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(fdim.size(), fdim.data()));
        mspace = std::unique_ptr<H5::DataSpace> (new H5::DataSpace(1, mdim));
        dataset = std::unique_ptr<H5::DataSet> (new H5::DataSet(file->createDataSet(datasetname, h5typemap[type], *fspace, plist)));
//        initialHyperslabParams(data.size());
        initialHyperslabParams(fdim.size());
//        std::cout << std::endl;
//        std::cout << fdim.size() << " " << fdim.data()  << std::endl;
        fspace->selectHyperslab(H5S_SELECT_SET, count.get(), start.get(), stride.get(), block.get());
        
        start[0]    = 1;
        stride[0]   = 1;
        count[0]    = data.size();
        block[0]    = 1;
        mspace->selectHyperslab(H5S_SELECT_SET, count.get(), start.get(), stride.get(), block.get());
        
        dataset->write(data.data(), h5typemap[type]);
        fspace->selectNone();
    }
    
private:
    void createFile (std::string filename){
        file = std::unique_ptr<H5::H5File>(new H5::H5File(filename, H5F_ACC_TRUNC));
    };
    void initialHyperslabParams(int size){
        start = std::unique_ptr<hsize_t[]>(new hsize_t[size]);
        stride = std::unique_ptr<hsize_t[]>(new hsize_t[size]);
        count = std::unique_ptr<hsize_t[]>(new hsize_t[size]);
        block = std::unique_ptr<hsize_t[]>(new hsize_t[size]);
        for (int i=0;i<size;++i){
            start[i]  = 0;
            stride[i] = 1;
            count[i]  = 1;
            block[i]  = 1;
        }
    }
};


class H5Multifiles{
    struct dataholder{
        std::string name;
        void* data;
        std::vector<unsigned long long> dims;
        size_t prob=0;//curring location
        size_t length;
        size_t total_size;
        H5Saver::h5type type;
        size_t number_of_datas;
        dataholder(std::string name_, void* data_, size_t size_, H5Saver::h5type type_, std::vector<unsigned long long> dims_):
        name(name_), data(data_), dims(dims_), total_size(size_), type(type_){
            length = 1;
            for(size_t i=1;i<dims.size();++i){
                length+=dims[i];
            }
            number_of_datas = dims_[0];
        }
        void* get_data(){
            assert(prob<total_size);
            switch (type){
                case H5Saver::h5type::INT:
                    return static_cast<int*>    (data)+prob;
                case H5Saver::h5type::FLOAT:
                    return static_cast<float*>  (data)+prob;
                case H5Saver::h5type::UINT8:
                    return static_cast<uint8_t*>(data)+prob;
            };
        }
    };
public:
    /**
     This class can save input files into multiple h5 files, depends on the desired datas per file. The default data number for one file is 2048. To change it using: setSizePerFile(number).
        Use add to add data in the saving queue.
        Use save to save all the datas into files.

     @param path The path you want to save
     @param log_ pass true if you want to create a log
     */
    H5Multifiles(std::string path, bool log_=false): output_path(path), log(log_){
    };
    void setSizePerFile(size_t size){size_for_one_file=size;}
    size_t getSizePerFile(){return size_for_one_file;}
    
    /**
     Add data into the saving queue.
     @param name the name of the dataset
     @param data a vector which contains every data in 1d
     @param dims {total_number_of_data, other_dimensions}
     */
    void add(std::string name, std::vector<float>& data, std::vector<unsigned long long> dims){
        inputs.push_back(dataholder(name, data.data(), data.size(), H5Saver::h5type::FLOAT, dims));
    }
    template <typename T>
    void add(std::string name, std::vector<float>& data, std::vector<T> dims){
        std::vector<unsigned long long> dims_;
        for(auto dim : dims){
            dims_.push_back(static_cast<unsigned long long>(dim));
        }
        add(name, data, dims_);
    }
    /**
     Add data into the saving queue.
     @param name the name of the dataset
     @param data a vector which contains every data in 1d
     @param dims {total_number_of_data, other_dimensions}
     */
    void add(std::string name, std::vector<int>& data, std::vector<unsigned long long> dims){
        inputs.push_back(dataholder(name, data.data(), data.size(), H5Saver::h5type::INT, dims));
    }
    template <typename T>
    void add(std::string name, std::vector<int>& data, std::vector<T> dims){
        std::vector<unsigned long long> dims_;
        for(auto dim : dims){
            dims_.push_back(static_cast<unsigned long long>(dim));
        }
        add(name, data, dims_);
    }
    /**
     Add data into the saving queue.
     @param name the name of the dataset
     @param data a vector which contains every data in 1d
     @param dims {total_number_of_data, other_dimensions}
     */
    void add(std::string name, std::vector<uint8_t>& data, std::vector<unsigned long long> dims){
        inputs.push_back(dataholder(name, data.data(), data.size(), H5Saver::h5type::UINT8, dims));
    }
    template <typename T>
    void add(std::string name, std::vector<uint8_t>& data, std::vector<T> dims){
        std::vector<unsigned long long> dims_;
        for(auto dim : dims){
            dims_.push_back(static_cast<unsigned long long>(dim));
        }
        add(name, data, dims_);
    }
    
    void save(){
        calculate_files();
        if(log)logfile();
        for (int j=0;j<output_file_num;++j) {
            save_base(j, size_for_one_file);
        }
        if (remainder){
            save_base(output_file_num, remainder);
        }
    }
private:
    size_t size_for_one_file=2048;
    size_t number_of_datas_=0;
    size_t output_file_num;
    size_t remainder;
    std::vector<dataholder> inputs;
    std::string output_path;
    bool log = false;
    
    void calculate_files(){
        for(size_t i=1; i< inputs.size();++i){
            assert(inputs[i].number_of_datas==inputs[i-1].number_of_datas);
        }
        number_of_datas_ = inputs[0].number_of_datas;
        output_file_num = (number_of_datas_) / size_for_one_file;
        remainder = output_file_num? number_of_datas_%size_for_one_file : number_of_datas_;
        if(!remainder){
            std::cerr << "Total output file number: " << output_file_num << std::endl;
        } else {
            std::cerr << "Total output file number: " << output_file_num + 1 << std::endl;
            std::cerr << "The last file will contain " << remainder << " number of dataset\n";
        }
    }
    void save_base(size_t counter, size_t size){
        H5Saver saver(output_path+"_"+std::to_string(counter)+".h5");
        for(auto i=0;i<inputs.size();++i){
            dataholder& hold = inputs[i];
            auto dim = hold.dims; dim[0] = size;
            saver.createDataset(hold.name, hold.type, hold.get_data(), dim);
            hold.prob += hold.length;
        }
    }
    void logfile(){
        std::fstream file_log;
        file_log.open(output_path + "_info.txt", std::fstream::trunc | std::fstream::out);
        assert(file_log.is_open());
        file_log << "number of datas:   " << number_of_datas_ << std::endl;
        for(auto i=0;i<inputs.size();++i){
            auto& d = inputs[i];
            file_log << "--------[" << i << "]--------\n";
            file_log << "data_name :        " << d.name << std::endl;
            file_log << "diemensions:       ";
            for(auto dim:d.dims) file_log << dim << " ";
            file_log << std::endl;
            file_log << "data_type:         ";
            switch (d.type){
                case H5Saver::h5type::INT:
                    file_log << "int" << std::endl;
                    break;
                case H5Saver::h5type::FLOAT:
                    file_log << "float" << std::endl;
                    break;
                case H5Saver::h5type::UINT8:
                    file_log << "uint8_t" << std::endl;
                    break;
            };
        }
        file_log.close();
    }
};
namespace H5 { 
    
}

#endif /* HDF5Saver_hpp */
