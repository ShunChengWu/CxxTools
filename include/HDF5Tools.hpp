//
//  HDF5Tools.hpp
//
//  Created by Shun-Cheng Wu on 19/09/2017.
//
//

#ifndef HDF5Tools_hpp
#define HDF5Tools_hpp

#include <utilities/HDF5Reader.hpp>
#include <utilities/HDF5Saver.hpp>
namespace H5tools {
    
    
    /**
     Read every h5 files in the given folder path [file_paths] and concat than into the vector [data]

     @param [In] file_paths The path to the folder contains h5 files.
     @param [In] dataset_name The dataset within h5 files you want to shuffled.
     @param [In] datatypes The type of those datasets
     @param [Out] data Return vec
     @param [Out] size_per_data The size of each data
     @param [Out] data_dim Dims of dataset
     @param [Out] total_num_data The total number of processed data
     */
    void readH52Vector(std::vector<std::string> file_paths, std::vector<std::string> dataset_name, std::vector<int> datatypes, std::vector<H5Saver::TYPES>& data, std::vector<int>& size_per_data, std::vector<std::vector<int>>& data_dim, int& total_num_data){
        //init
        data_dim.resize(dataset_name.size());
        size_per_data.resize(dataset_name.size());
        for (auto& v : size_per_data) v = 0;
        
        for (auto path : file_paths){
            for (int i=0;i<dataset_name.size();++i){
                if (datatypes[i] == H5Saver::h5type::INT){
                    H5Reader<int> h5_reader;
                    h5_reader.read(path, dataset_name[i]);
                    data[i].vint.insert(data[i].vint.end(), h5_reader.value->begin(), h5_reader.value->end());
                    if (!size_per_data[i]) {
                        for (int r = 0; r < h5_reader.rank; ++r) {
                            data_dim[i].push_back(h5_reader.template dim<int>(r));
                        }
                        size_per_data[i]=1;
                        for (int r = 1; r < data_dim[i].size(); ++r)
                            size_per_data[i] *= data_dim[i][r];
                    }
                    total_num_data += h5_reader.template dim<int>(0);
                } else if (datatypes[i] == H5Saver::h5type::FLOAT) {
                    H5Reader<float> h5_reader;
                    h5_reader.read(path, dataset_name[i]);
                    data[i].vfloat.insert(data[i].vfloat.end(), h5_reader.value->begin(), h5_reader.value->end());
                    if (!size_per_data[i]) {
                        for (int r = 0; r < h5_reader.rank; ++r) {
                            data_dim[i].push_back(h5_reader.template dim<int>(r));
                        }
                        size_per_data[i]=1;
                        for (int r = 1; r < data_dim[i].size(); ++r)
                            size_per_data[i] *= data_dim[i][r];
                    }
                    total_num_data += h5_reader.template dim<int>(0);
                } else if (datatypes[i] == H5Saver::h5type::UINT8) {
                    H5Reader<uint8_t> h5_reader;
                    h5_reader.read(path, dataset_name[i]);
                    data[i].vuint8.insert(data[i].vuint8.end(), h5_reader.value->begin(), h5_reader.value->end());
                    if (!size_per_data[i]) {
                        for (int r = 0; r < h5_reader.rank; ++r) {
                            data_dim[i].push_back(h5_reader.template dim<int>(r));
                        }
                        size_per_data[i]=1;
                        for (int r = 1; r < data_dim[i].size(); ++r)
                            size_per_data[i] *= data_dim[i][r];
                    }
                    total_num_data += h5_reader.template dim<int>(0);
                } else {
                    printf("input type not supported.");
                    exit(-1);
                }
            }
        }
        total_num_data/= dataset_name.size();
    }
}

#endif /* HDF5Tools_hpp */

