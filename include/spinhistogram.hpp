//
//  spinhistogram.h
//  evaluatelrf
//
//  Created by Shun-Cheng Wu on 10/04/2018.
//

#ifndef spinhistogram_h
#define spinhistogram_h
#include <Eigen/Core>

class spinhistogram{
    struct point3d_easyaccess{
        float &x,&y,&z;
        point3d_easyaccess(float* point):x(point[0]), y(point[1]), z(point[2]){};
    };
    size_t number_of_histograms = 8;
    float angle_per_his;
    float angle_restrict = 30; //degree
    float angle_restrict_rad;
    std::vector<float> bins;
    std::vector<float> weights;
    bool showdebugmsg = false;
public:
    void run(float* data, size_t size, size_t stride = 3){
        init();
        
        for(size_t i=0;i<size; ++i){
            Eigen::Vector3f point(data + stride*i);
            if(sin(angle_between_z(point))>sin(angle_restrict_rad)){
                size_t cate = 0;
                FindNearestBin(bins, angle_between_x(point), cate);
                weights[cate]++;
            }
        }
        norm_weight();
    };
    
    void run(const float* data, size_t size, size_t stride = 3){
        assert(!checkAllzero(data,size));
        init();
        for(size_t i=0;i<size; ++i){
            Eigen::Vector3f point(data + stride*i);
            if(sin(angle_between_z(point))>sin(angle_restrict_rad)){
                size_t cate = 0;
                FindNearestBin(bins, angle_between_x(point), cate);
                weights[cate]++;
            }
        }
        norm_weight();
    };
    template<typename T>
    bool checkAllzero(T* data, size_t size){
        float sum=0;
        for(size_t i=0;i<size*3; ++i){
            sum += data[i];
        }
        return sum==0;
    }
    /** Find angle to rotate histogram1 to histogram2. The resolution will be the histogram_size-(compare_block_size-1)  */
    float findangle(std::vector<float> his1, const std::vector<float>& his2, size_t block_size=1){
        size_t size = his1.size();
        assert(size > 0);
        his1.insert(his1.end(), his1.begin(), his1.end());
        std::vector<float> sums(size-block_size+1);
        for(size_t j=0;j<size;++j){
            auto srd = [](const float& l, const float& r)->float{
                return sqrt((l-r)*(l-r));
            };
            float sum=0;
            for(size_t k=0;k<size;++k){
                sum += srd(his1[j+k], his2[k]);
            }
            int init_value = std::max(0,int(j+1-block_size));
            for(size_t d=init_value; d<=j; ++d){
                if(d<sums.size()) sums[d]+=sum;
                else sums[sums.size()-1] +=sum;
            }
        }
        auto min = std::min_element(sums.begin(), sums.end());
        auto prob = std::distance(sums.begin(), min);
        float angle = -prob*2*EIGEN_PI/size;
        return angle;
    };
    
    std::vector<float> getweights(){return weights;}
    void setHistogramNumber(size_t num){number_of_histograms=num;};
    void setRestrictAngle(float angle){angle_restrict = angle;};
private:
    void FindNearestBin (std::vector<float> bins, float value, size_t& index){
        int upper = bins.size()-1;
        int lower = 0;
        int middle = upper/2;
        while(1){
            if(showdebugmsg)
            printf("u, m, l = {%3d, %3d, %3d}:", upper, middle, lower);
            if(value >= bins[middle]){
                if(showdebugmsg)
                printf("\tvalue(%f) > bins[%3d:%5f]\n", value, middle, bins[middle]);
                if(upper - middle > 1){
                    lower   = middle;
                    middle += (upper-lower)*0.5;
                } else {
                    index = middle;
                    break;
                }
            } else {
                if(showdebugmsg)
                printf("\tvalue(%f) < bins[%3d:%5f]\n", value, middle, bins[middle]);
                if(upper - middle > 1){
                    upper   = middle;
                    middle -= (upper-lower)*0.5;
                } else {
                    index = lower;
                    break;
                }
            }
        }
    };
    float angle_between_x(const Eigen::Vector3f& p){
        Eigen::Vector3f x_axis(1,0,0);
        Eigen::Vector3f input(p(0), p(1), 0);
        float angle = Safeacos(input.dot(x_axis)/input.norm());
        if(p(1)<0) angle=2*EIGEN_PI-angle;
        return angle;
    };
    float angle_between_z(const Eigen::Vector3f& p){
        Eigen::Vector3f z_axis(0,0,1);
        Eigen::Vector3f input(p(0), p(1), p(2));
        //        std::cout << "z: " << tools::MATH().Safeacos(input.dot(z_axis)/input.norm())*180/EIGEN_PI << std::endl;
        return Safeacos(input.dot(z_axis)/input.norm());
    };
    void norm_weight(){ //!!!!: 這邊也許可以改成某種分佈的方式，來降低雜訊所獲得的權重值
        std::vector<float>::iterator prob;
        prob = std::max_element(weights.begin(), weights.end());
        size_t position = std::distance(weights.begin(), prob);
        float max = weights[position];
        for(size_t i=0; i<weights.size(); ++i){
            weights[i] /= max;
        }
    }
    void init(){
        bins.clear();
        weights.clear();
        weights.resize(number_of_histograms);
        angle_restrict_rad = angle_restrict*EIGEN_PI/180;
        angle_per_his = 2*EIGEN_PI/number_of_histograms;
        for(float i=0;i<=2*EIGEN_PI; i+=angle_per_his){
            bins.push_back(i);
        }
    }
    float Safeacos (float x){
        if (x < -1.0) x = -1.0 ;
        else if (x > 1.0) x = 1.0 ;
        return acos (x) ;
    }
};

#endif /* spinhistogram_h */
