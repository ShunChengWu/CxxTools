#ifndef weightedunitvoxel_hpp
#define weightedunitvoxel_hpp

#include <utilities/Tools.hpp>

#include <utilities/thread_pool.hpp>
#include <utilities/HDF5Reader.hpp>
#include <utilities/HDF5Saver.hpp>
using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace WUV{
    struct Point3D{
        struct point3d_easyaccess{
            float &x,&y,&z;
            point3d_easyaccess(float* point):x(point[0]), y(point[1]), z(point[2]){};
        };
        std::vector<float> data;
        size_t size;
        void init(size_t num){
            size = num;
            data.resize(num*3);
        }
        point3d_easyaccess point(size_t num){
            assert(num<size);
            return point3d_easyaccess(&data[3*num]);
        }
        ~Point3D(){
        }
    };
    
    class WeightedUnitVoxel{
        struct point3d_easyaccess{
            float &x,&y,&z;
            point3d_easyaccess(float* point):x(point[0]), y(point[1]), z(point[2]){};
        };
        struct Boundaries{
            std::vector<float> x,y,z;
            size_t size;
            void init(float interval){
                for(float i=0;i<=1.f;i+=interval){
                    x.emplace_back(i);
                    y.emplace_back(i);
                    z.emplace_back(i);
                    if(i){
                        x.emplace_back(-i);
                        y.emplace_back(-i);
                        z.emplace_back(-i);
                    }
                }
                std::sort(x.begin(), x.end());
                std::sort(y.begin(), y.end());
                std::sort(z.begin(), z.end());
                size = x.size();
            };
        };
        struct Centors{
            
            size_t size, size_total;
            std::vector<float> centrois;
            
            void init(const Boundaries& boundaries, float interval){
                size = boundaries.size-1;
                size_total = pow(size, 3);
                centrois.resize(size_total*3);
                
                float half_interval = 0.5*interval;
                for(size_t bx=0; bx < size; ++bx){
                    for(size_t by=0; by < size; ++by){
                        for(size_t bz=0; bz < size; ++bz){
                            auto point_ = centor(bx,by,bz);
                            point_.x = boundaries.x[bx] + half_interval;
                            point_.y = boundaries.y[by] + half_interval;
                            point_.z = boundaries.z[bz] + half_interval;
                        }
                    }
                }
            }
            
            point3d_easyaccess centor(size_t num){
                assert(num < size_total);
                return point3d_easyaccess(&centrois[3*num]);
            }
            point3d_easyaccess centor(size_t x, size_t y, size_t z){
                assert(x<size && x<size && z<size);
                return point3d_easyaccess(&centrois[3*((x*size+y)*size+z)]);
            }
        };
        
    public:
        Boundaries boundaries;
        Centors centors;
        
        WeightedUnitVoxel(){};
        WeightedUnitVoxel(float interval){
            init(interval);
        };
        void init(float interval){
            voxel_interval=interval;
            boundaries.init(voxel_interval);
            centors.init(boundaries, voxel_interval);
            size_total = centors.size_total;
            size_edge = centors.size;
            weights.resize(size_total);
        }
        void init(size_t size){
            voxel_interval = 2/float(size);
            boundaries.init(voxel_interval);
            centors.init(boundaries, voxel_interval);
            size_total = centors.size_total;
            size_edge = centors.size;
            weights.resize(size_total);
        }
        
        
        const size_t size(){return size_total;}
        const size_t size_single(){return size_edge;}
        
        float& weight(size_t x, size_t y, size_t z ){
            assert(x<size_edge && x<size_edge && z<size_edge);
            return weights[(x*size_edge+y)*size_edge+z];
        }
        float& weight(size_t num){
            assert(num < size_total);
            return weights[num];
        }
        point3d_easyaccess centor(size_t num){return centors.centor(num);}
        point3d_easyaccess centor(size_t x, size_t y, size_t z){return centors.centor(x, y, z);}
        
        void calculate_weight(Point3D points){
            bool showdebugmsg = 0;
            auto FindNearestVoxelGrid = [showdebugmsg](std::vector<float> boundaries, float value, size_t& index){
                int upper = boundaries.size()-1;
                int lower = 0;
                int middle = upper/2;
                while(1){
                    if(showdebugmsg)
                        printf("u, m, l = {%3d, %3d, %3d}:", upper, middle, lower);
                    if(value >= boundaries[middle]){
                        if(showdebugmsg)
                            printf("\tvalue(%f) > boundaries[%d:%f]\n", value, middle, boundaries[middle]);
                        if(upper - middle > 1){
                            lower   = middle;
                            middle += (upper-lower)*0.5;
                        } else {
                            index = middle;
                            break;
                        }
                    } else {
                        if(showdebugmsg)
                            printf("\tvalue(%f) < boundaries[%d:%f]\n", value, middle, boundaries[middle]);
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
            for(size_t i=0; i<points.size;++i){
                auto point = points.point(i);
                size_t cx = 0, cy = 0, cz = 0;
                if(showdebugmsg)printf("x:\n");
                FindNearestVoxelGrid(boundaries.x, point.x, cx);
                if(showdebugmsg)printf("y: \n");
                FindNearestVoxelGrid(boundaries.y, point.y, cy);
                if(showdebugmsg)printf("z: \n");
                FindNearestVoxelGrid(boundaries.z, point.z, cz);
    
                if(showdebugmsg)
                    printf("Query_point[%f,%f,%f], voxel found(%zu,%zu,%zu)=[%f,%f,%f]\n", point.x,point.y,point.z, cx,cy,cz, centors.centor(cx, cy, cz).x, centors.centor(cx, cy, cz).y, centors.centor(cx, cy, cz).z);
                weight(cx,cy,cz)++;
            }
            norm_weight();
    
            if(showdebugmsg)
                for(size_t i=0;i<size_edge;++i){
                    for(size_t j=0;j<size_edge;++j){
                        for(size_t k=0;k<size_edge;++k){
                            printf("voxel[%2zu,%2zu,%2zu] = [%5.3f,%5.3f,%5.3f, %f]\n", i,j,k,centors.centor(i, j, k).x, centors.centor(i, j, k).y, centors.centor(i, j, k).z, weight(i, j, k));
                        }
                    }
                }
        }
        
        void calculate_weight(float* data, size_t size){
            bool showdebugmsg = 0;
            auto FindNearestVoxelGrid = [showdebugmsg](std::vector<float> boundaries, float value, size_t& index){
                int upper = boundaries.size()-1;
                int lower = 0;
                int middle = upper/2;
                while(1){
                    if(showdebugmsg)
                        printf("u, m, l = {%3d, %3d, %3d}:", upper, middle, lower);
                    if(value >= boundaries[middle]){
                        if(showdebugmsg)
                            printf("\tvalue(%f) > boundaries[%d:%f]\n", value, middle, boundaries[middle]);
                        if(upper - middle > 1){
                            lower   = middle;
                            middle += (upper-lower)*0.5;
                        } else {
                            index = middle;
                            break;
                        }
                    } else {
                        if(showdebugmsg)
                            printf("\tvalue(%f) < boundaries[%d:%f]\n", value, middle, boundaries[middle]);
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
            for(size_t i=0; i<size;++i){
                auto point = point3d_easyaccess(data + 3*i);
                size_t cx = 0, cy = 0, cz = 0;
                if(showdebugmsg)printf("x:\n");
                FindNearestVoxelGrid(boundaries.x, point.x, cx);
                if(showdebugmsg)printf("y: \n");
                FindNearestVoxelGrid(boundaries.y, point.y, cy);
                if(showdebugmsg)printf("z: \n");
                FindNearestVoxelGrid(boundaries.z, point.z, cz);
                
                if(showdebugmsg)
                    printf("Query_point[%f,%f,%f], voxel found(%zu,%zu,%zu)=[%f,%f,%f]\n", point.x,point.y,point.z, cx,cy,cz, centors.centor(cx, cy, cz).x, centors.centor(cx, cy, cz).y, centors.centor(cx, cy, cz).z);
                weight(cx,cy,cz)++;
            }
            norm_weight();
            
            if(showdebugmsg)
                for(size_t i=0;i<size_edge;++i){
                    for(size_t j=0;j<size_edge;++j){
                        for(size_t k=0;k<size_edge;++k){
                            printf("voxel[%2zu,%2zu,%2zu] = [%5.3f,%5.3f,%5.3f, %f]\n", i,j,k,centors.centor(i, j, k).x, centors.centor(i, j, k).y, centors.centor(i, j, k).z, weight(i, j, k));
                        }
                    }
                }
        };
        
        std::vector<float> weights;
    private:
        size_t size_edge, size_total;
        float voxel_interval;
        
        void norm_weight(){ //!!!!: 這邊也許可以改成某種分佈的方式，來降低雜訊所獲得的權重值
            std::vector<float>::iterator prob;
            prob = std::max_element(weights.begin(), weights.end());
            size_t position = std::distance(weights.begin(), prob);
            float max = weights[position];
            for(size_t i=0; i<size_total; ++i){
                weights[i] /= max;
            }
        }
    };
} // end of namesapce WUV


#endif




