//
//  HDF5Reader.hpp
//
//  Created by Shun-Cheng Wu on 19/09/2017.
//
//

#ifndef HDF5Reader_hpp
#define HDF5Reader_hpp

#include <iostream>
#include <string>
#include <vector>
#include <utilities/Tools.hpp>

#include "H5Cpp.h"


using std::cout;
using std::endl;

template<typename t>
class H5Reader{
public:
    int rank=0;
    int size=1;
    std::unique_ptr<std::vector<hsize_t>> dims;
    std::unique_ptr<std::vector<t>> value;
    H5Reader(){};
    H5Reader(std::string filename, std::string Name){
        read(filename, Name);
    }
    void read(std::string filename, std::string Name){
        H5::H5File fi(filename, H5F_ACC_RDWR);
        H5::DataSet dataset = fi.openDataSet(Name);
        H5::DataType dtype = dataset.getDataType(); /* Get datatype for dataset */
        H5::DataSpace dataspace = dataset.getSpace();
        rank = dataspace.getSimpleExtentNdims();/* Get the number of dimensions in the dataspace.*/
        dims = std::unique_ptr<std::vector<hsize_t>> (new std::vector<hsize_t> (rank));
        dataspace.getSimpleExtentDims( dims->data(), NULL);
        for (int i=0;i<dims->size();++i){
            size *= dim<int>(i);
        }
        value = std::unique_ptr<std::vector<t>> (new std::vector<t>(size));
        dataset.read( value->data(), dtype);
    }
    
    ~H5Reader(){
    }
    
    template<class ty>
    ty dim(int x){
        assert(x<rank && "Shouldn't larger than original dimension");
        return (*dims)[x];
    }
    
    template<typename ty>
    ty size_total(){
        hsize_t size=1;
        for (auto dim : *dims){
            size *= dim;
        }
        return size;
    }
    
    template <class ty>
    ty at(int x){
//        assert (rank==1 && "This data is not 1D.");
//        assert (x<dim<int>(0) && "specified x larger than data dimension");
        return value->at(x);
    }
    
    template <class ty>
    ty at(int x, int y){
        assert (rank==2 && "This data is not 2D.");
        assert (x<dim<int>(0) && "specified x larger than data dimension");
        assert (y<dim<int>(1) && "specified y larger than data dimension");
        return value->at(x*dim<int>(1)+y);
    }
    
    template <class ty>
    ty at(int x, int y, int z){
        assert (rank==3 && "This data is not 3D.");
        assert (x<dim<int>(0) && "specified x larger than data dimension");
        assert (y<dim<int>(1) && "specified y larger than data dimension");
        assert (z<dim<int>(2) && "specified z larger than data dimension");
        return value->at(x*dim<int>(1)*dim<int>(2)+y*dim<int>(2)+z);
    }
};



#endif /* HDF5Reader_hpp */
