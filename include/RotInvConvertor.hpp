//
//  RotInvConvertor.hpp
//
//  Created by Shun-Cheng Wu on 18/01/2017.
//
//

#ifndef RotInvConvertor_hpp
#define RotInvConvertor_hpp
#include <vector>

class rotinv_convertor{
public:
    struct point3 {
        float x, y, z;
        point3 (){};
        point3 (float x_, float y_, float z_) {x=x_;y=y_;z=z_;};
        void init(float x_, float y_, float z_) {x=x_;y=y_;z=z_;};
        float dis(){return sqrtf( square(x) + square(y) + square(z) );};
        float square(float v){return v*v;};
        float ang(point3 p2){
            float tmp = std::acos( *this * p2/(this->dis()*p2.dis()) )/ M_PI;
            return std::isnan(tmp)? (*this * p2 > 0? 0 : M_PI) : tmp;
        };
        float cos(point3 p2){
            float tmp = ( *this * p2/(this->dis()*p2.dis()) );
            return std::isnan(tmp)? (*this * p2 > 0? 0 : M_PI) : tmp;
        };
        float operator*(point3 rvar){return this->x * rvar.x + this->y * rvar.y + this->z * rvar.z;};
    };
    rotinv_convertor(){};
    void run(std::vector<float>& data, int num_of_data, int& size_per_data){
        std::vector<float> inv_data;
        inv_data.resize(data.size());
        if (size_per_data > 1){
            inv_data.resize( num_of_data * combination(size_per_data/3, 2) * 3);
        } else {
            inv_data.resize( data.size() );
        }
        
        int counter_data=0;
        if (size_per_data>1){
            for (int c=0; c < num_of_data; ++c){
                for (int i=0; i<size_per_data/3 - 1; ++i) { // i = 0~ k-1
                    for (int j=i+1; j <= size_per_data/3 - 1; ++j) { //j = i ~ k
                        point3 point1(data[c*size_per_data + i*3], data[c*size_per_data + i*3 + 1], data[c*size_per_data + i*3 + 2]),
                        point2(data[c*size_per_data + j*3],data[c*size_per_data + j*3 + 1],data[c*size_per_data + j*3 + 2]);
                        //Check error: Cannot over the origianl size
                        assert(((i+1)+2)/3 <= data.size()/3-1);//(i+1): real size. +2: plus two more points. /3: count as one point set
                        assert(((j+1)+2)/3 <= data.size()/3);
                        
                        inv_data[counter_data++] = std::min(point1.dis(), point2.dis());
                        inv_data[counter_data++] = std::max(point1.dis(), point2.dis());
                        inv_data[counter_data++] = point1.ang(point2);
                        if (counter_data  >= inv_data.size()){
                            //                            std::cout<< ": "<< counter_data << " " << inv_data.size() << std::endl;
                            assert(counter_data <= inv_data.size());//+1 for the last.
                        }
                    }
                }
            }
            data.swap(inv_data);
        }
        size_per_data = combination(size_per_data/3, 2) * 3;
    }
    //private:
    int permutation(int n, int m, int t=1){
        for (int i=0; i<m;++i) t *= n-i;
        return t;
    }
    int combination(int n, int m, int t=1){return permutation(n, m, t)/fac(m);}
    /** Factorial */
    int fac(int n, int t=1){
        do t *= n--; while (n > 0);
        return t;
    }
};


#endif /* RotInvConvertor_hpp */
