#ifndef regionalpointdescriptor_hpp
#define regionalpointdescriptor_hpp

#include <utilities/thread_pool.hpp>
#include <Eigen/Core>
#include <iostream>
using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace RPD{
    class RegionalPointDescriptor{
        size_t
            azimuth = 12,
            elevation = 11,
            radial = 17;
        std::vector<float> vazimuth, velevation, vradial;
        float
            rmin = 0.1,
            rmax = 1.0;
        bool showdebugmsg = false;
        std::vector<float> weights;
    public:
        std::vector<float>& getvector_azimuth(){return vazimuth;};
        std::vector<float>& getvector_elevation(){return velevation;};
        std::vector<float>& getvector_radial(){return vradial;};
        RegionalPointDescriptor(){};
        int run(const float* data, size_t size, size_t stride = 3){
            assert(!checkAllzero(data, size));
            init();
            for(size_t i=0;i<size; ++i){
                Eigen::Vector3f point(data + stride*i);
                if(point.hasNaN()) continue; //跳過nan
                size_t cate_a = 0, cate_e = 0, cate_r = 0;
                float distance = distance_between_origin(point);
                if(distance < rmin) continue; //必須大於rmin
                float angle_x = angle_between_x(point);
                float angle_z = angle_between_z(point);
                if(FindNearestBin(vazimuth, angle_x, cate_a)) continue;
                if(FindNearestBin(velevation, angle_z, cate_e)) continue;
                if(FindNearestBin(vradial, distance, cate_r)) continue;
                size_t prob = (cate_a*elevation+cate_e)*radial+cate_r;
                assert(prob<weights.size());
                weights[prob]++;
            }
            if(!norm_weight()) return 0;
            return 1;
        }
        int run(float* data, size_t size, size_t stride = 3){
            assert(!checkAllzero(data, size));
            init();
            for(size_t i=0;i<size; ++i){
                Eigen::Vector3f point(data + stride*i);
                size_t cate_a = 0, cate_e = 0, cate_r = 0;
                float distance = distance_between_origin(point);
                if(!distance) continue;
                assert(!std::isnan(distance));
                if(distance < rmin) continue; //必須大於rmin
                if(!FindNearestBin(vazimuth, angle_between_x(point), cate_a)) continue;
                if(!FindNearestBin(velevation, angle_between_z(point), cate_e)) continue;
                if(!FindNearestBin(vradial, distance, cate_r)) continue;
                size_t prob = (cate_a*elevation+cate_e)*radial+cate_r;
                assert(prob<weights.size());
                if(point(0) == 0 && point(1) == 0){
                    //????: when (0,0,x) add to all bins?
                    //printf("distance: %f, cate[%zu,%zu,%zu]\n", distance, cate_a, cate_e, cate_r);
                }
                weights[prob]++;
            }
            if(!norm_weight()) return 0;
            return 1;
        };

        template<typename T>
        bool checkAllzero(T* data, size_t size){
            float sum=0;
            for(size_t i=0;i<size*3; ++i){
                sum += data[i];
            }
            return sum==0;
        }
        
        float findangle(std::vector<float> his1,const std::vector<float>& his2,size_t stride){
            size_t size = his1.size();
            assert(size>0);
            his1.insert(his1.end(), his1.begin(), his1.end());
            size_t block = size/stride;
            std::vector<float> sums(stride);
            for(size_t j=0; j<size; j+=block){
                auto srd = [](const float& l, const float& r)->float{return sqrt((l-r)*(l-r));};
                float sum=0;
                for(size_t k=0;k<size;++k) sum += srd(his1[j+k], his2[k]);
                sums[j/block] = sum;
            }
            auto min = std::min_element(sums.begin(), sums.end());
            auto prob = std::distance(sums.begin(), min);
            float angle = -prob*2*EIGEN_PI/stride;
            return angle;
        }
        /**
         Set Parameters
         @param azi azimuth the number of bins around xy-plane
         @param ele elevation the number of bins along z direction, start from z=1
         @param rad radial the number of bins from rmin to rmax
         @param rmin_ rmin the minimum distance required to start creating bins
         @param rmax_ rmax the max value for raidial
         */
        void setBoundaryParameters(size_t azi, size_t ele, size_t rad, float rmin_ = 0.1, float rmax_ = 1.0){
            azimuth = azi;
            elevation = ele;
            radial = rad;
            rmin = rmin_;
            rmax = rmax_;
        }
        std::vector<float> getweights(){return weights;};
        std::vector<float>& getweights_raw(){return weights;};
    private:
        inline void init(){
            calculate_boundaries();
            weights.clear();
            weights.resize(azimuth*elevation*radial);
        }
        inline float angle_between_x(const Eigen::Vector3f& p){
            Eigen::Vector3f x_axis(1,0,0);
            Eigen::Vector3f input(p(0), p(1), 0);
            float angle = Safeacos(input.dot(x_axis)/input.norm());
            if(p(1)<0) angle=2*EIGEN_PI-angle;
            return angle;
        };
        inline float angle_between_z(const Eigen::Vector3f& p){
            Eigen::Vector3f z_axis(0,0,1);
            Eigen::Vector3f input(p(0), p(1), p(2));
//            std::cout << "input: " << input << std::endl;
//            std::cout << "angle: " << Safeacos(input.dot(z_axis)/input.norm()) << std::endl;
            return Safeacos(input.dot(z_axis)/input.norm());
        };
        inline float distance_between_origin(const Eigen::Vector3f& p){
            return p.norm();
        }
        inline float Safeacos (float x){
            if (x < -1.0) x = -1.0 ;
            else if (x > 1.0) x = 1.0 ;
            return acos (x) ;
        }
        inline bool calculate_boundaries(){
            if(vazimuth.size() == azimuth + 1 &&
               velevation.size() == elevation+1 &&
               vradial.size() == radial+1){
                return 1;
            }
            vazimuth.resize(azimuth+1);
            velevation.resize(elevation+1);
            vradial.resize(radial+1);
            /* 增量值 */
            float ca = 2*M_PI/azimuth;
            float ce = M_PI/elevation;
            /* 目前值 */
            float va=0;
            float ve=0;
            
            for(size_t a=0;a<=azimuth;++a){
                vazimuth[a]=va;
                va+=ca;
            }
            for(size_t e=0;e<=elevation;++e){
                velevation[e]=ve;
                ve+=ce;
            }
            vradial[0]=rmin;
            for(size_t r=1;r<=radial;++r){
                float tmp = rmin != 0 ?exp(log(rmin)+float(r)/radial*log(rmax/rmin)) : rmax;
                vradial[r]=tmp;
            }
            return 1;
//            printf("\n");
        }
        inline int FindNearestBin (const std::vector<float>& bins, const float& value, size_t& index){
            int upper = bins.size()-1;
            int lower = 0;
            int middle = upper/2;
            if(value > bins[upper]) return 0;
            if(value < bins[lower]) return 0;
            
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
            return 1;
        };
        inline int norm_weight(){
            std::vector<float>::iterator prob;
            prob = std::max_element(weights.begin(), weights.end());
            size_t position = std::distance(weights.begin(), prob);
            float max = weights[position];
            if(max == 0){
//                printf("Weights: ");
//                for(auto w : weights){
//                    printf("%f ", w);
//                }
//                printf("\n");
                printf("Input vector are all zero.\n");
                return 0;
            }
            for(size_t i=0; i<weights.size(); ++i){
                weights[i] /= max;
            }
            return 1;
        }
    };
} // end of namespace RPD
#endif




