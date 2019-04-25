#ifndef weightedunitvoxel_hpp
#define weightedunitvoxel_hpp

#include <stdio.h>
#include <vector>

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
            size_t size_x, size_y, size_z;
            void init(float cx, float cy, float cz,
                      int sx, float sy, float sz,
                      float scale){
                for (int i=0;i<sx; ++i) {
                    float pos_x = cx + i * scale;
                    x.emplace_back(pos_x);
                }
                for (int i=0;i<sy; ++i) {
                    float pos_y = cy + i * scale;
                    y.emplace_back(pos_y);
                }
                for (int i=0;i<sz; ++i) {
                    float pos_z = cz + i * scale;
                    z.emplace_back(pos_z);
                }
                std::sort(x.begin(), x.end());
                std::sort(y.begin(), y.end());
                std::sort(z.begin(), z.end());
                size_x = x.size();
                size_y = y.size();
                size_z = z.size();
            };
        };
        struct Centors{
            
            size_t size_x, size_y, size_z, size_total;
            std::vector<float> centrois;
            
            void init(const Boundaries& boundaries, float interval){
                size_x = boundaries.size_x-1;
                size_y = boundaries.size_y-1;
                size_z = boundaries.size_z-1;
                size_total = size_z * size_y*size_x;
                centrois.resize(size_total*3);
                
                float half_interval = 0.5*interval;
                for(size_t bx=0; bx < size_x; ++bx){
                    for(size_t by=0; by < size_y; ++by){
                        for(size_t bz=0; bz < size_z; ++bz){
                            auto point_ = centor(bx,by,bz);
                            point_.x = boundaries.x[bx] + half_interval;
                            point_.y = boundaries.y[by] + half_interval;
                            point_.z = boundaries.z[bz] + half_interval;
                        }
                    }
                }
            }
            
            point3d_easyaccess centor(size_t num){
                if(num < size_total)
                    return point3d_easyaccess(&centrois[3*num]);
                printf("[warning] exceed maximum num\n");
                return point3d_easyaccess(&centrois[3*size_total]);
            }
            point3d_easyaccess centor(size_t x, size_t y, size_t z){
                if(x<size_x && y<size_y && z<size_z);
                return point3d_easyaccess(&centrois[3*((x*size_y+y)*size_z+z)]);
                printf("[warning] exceed maximum num\n");
                return point3d_easyaccess(&centrois[3*size_total]);
            }
        };
        
    public:
        Boundaries boundaries;
        Centors centors;
        
        WeightedUnitVoxel(){};
        
        void init(float cx, float cy, float cz,
                  int sx, int sy, int sz,
                  float size){
            voxel_interval=size;
            boundaries.init(cx, cy, cz, sx, sy, sz, voxel_interval);
            
            centors.init(boundaries, voxel_interval);
            size_total = centors.size_total;
            size_edge_x = centors.size_x;
            size_edge_y = centors.size_y;
            size_edge_z = centors.size_z;
            weights.resize(size_total);
        }
        
        size_t size(){return size_total;}
        
        float& weight(size_t x, size_t y, size_t z ){
            if(x<size_edge_x && y<size_edge_y && z<size_edge_z)
                return weights[(x*size_edge_y+y)*size_edge_z+z];
            printf("[warning] exceed\n");
            return weights[size_total];
        }
        float& weight(size_t num){
            if(num < size_total)
            return weights[num];
            printf("[warning] exceed\n");
            return weights[size_total];
        }
        point3d_easyaccess centor(size_t num){return centors.centor(num);}
        point3d_easyaccess centor(size_t x, size_t y, size_t z){return centors.centor(x, y, z);}
        
        void calculate_weight(Point3D points){
            bool showdebugmsg = 0;
            auto FindNearestVoxelGrid = [showdebugmsg](std::vector<float> boundaries, float value, int& index){
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
                if(index == lower) {
                    if (value < boundaries[lower]){
                        index = -1;
                    }
                } else if (index == upper) {
                    if (value > boundaries[upper]){
                        index = -1;
                    }
                }
                
            };
            for(size_t i=0; i<points.size;++i){
                auto point = points.point(i);
                int cx = 0, cy = 0, cz = 0;
                if(showdebugmsg)printf("x:\n");
                FindNearestVoxelGrid(boundaries.x, point.x, cx);
                if (cx<0) continue;
                if(showdebugmsg)printf("y: \n");
                FindNearestVoxelGrid(boundaries.y, point.y, cy);
                if (cy<0) continue;
                if(showdebugmsg)printf("z: \n");
                FindNearestVoxelGrid(boundaries.z, point.z, cz);
                if (cz<0) continue;
    
                if(showdebugmsg)
                    printf("Query_point[%f,%f,%f], voxel found(%d,%d,%d)=[%f,%f,%f]\n", point.x,point.y,point.z, cx,cy,cz, centors.centor(cx, cy, cz).x, centors.centor(cx, cy, cz).y, centors.centor(cx, cy, cz).z);
                weight(cx,cy,cz)++;
            }
            norm_weight();
    
            if(showdebugmsg)
                for(size_t i=0;i<size_edge_x;++i){
                    for(size_t j=0;j<size_edge_y;++j){
                        for(size_t k=0;k<size_edge_z;++k){
                            printf("voxel[%2zu,%2zu,%2zu] = [%5.3f,%5.3f,%5.3f, %f]\n", i,j,k,centors.centor(i, j, k).x, centors.centor(i, j, k).y, centors.centor(i, j, k).z, weight(i, j, k));
                        }
                    }
                }
        }
        
        void calculate_weight(float* data, size_t size, int interval){
            bool showdebugmsg = 0;
            auto FindNearestVoxelGrid = [showdebugmsg](std::vector<float> boundaries, float value, int& index){
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
                if(index == lower) {
                    if (value < boundaries[lower]){
                        return -1;
                    }
                } else if (index == upper) {
                    if (value > boundaries[upper]){
                        return -1;
                    }
                }
            };
            for(size_t i=0; i<size;++i){
                auto point = point3d_easyaccess(data + interval*i);
                int cx = 0, cy = 0, cz = 0;
                if(showdebugmsg)printf("x:\n");
                FindNearestVoxelGrid(boundaries.x, point.x, cx);
                if (cx<0) continue;
                if(showdebugmsg)printf("y: \n");
                FindNearestVoxelGrid(boundaries.y, point.y, cy);
                if (cy<0) continue;
                if(showdebugmsg)printf("z: \n");
                FindNearestVoxelGrid(boundaries.z, point.z, cz);
                if (cz<0) continue;
                
                if(showdebugmsg)
                    printf("Query_point[%f,%f,%f], voxel found(%zu,%zu,%zu)=[%f,%f,%f]\n", point.x,point.y,point.z, cx,cy,cz, centors.centor(cx, cy, cz).x, centors.centor(cx, cy, cz).y, centors.centor(cx, cy, cz).z);
                weight(cx,cy,cz)++;
            }
            norm_weight();
            
            if(showdebugmsg)
                for(size_t i=0;i<size_edge_x;++i){
                    for(size_t j=0;j<size_edge_y;++j){
                        for(size_t k=0;k<size_edge_z;++k){
                            printf("voxel[%2zu,%2zu,%2zu] = [%5.3f,%5.3f,%5.3f, %f]\n", i,j,k,centors.centor(i, j, k).x, centors.centor(i, j, k).y, centors.centor(i, j, k).z, weight(i, j, k));
                        }
                    }
                }
        };
        
        std::vector<float> weights;
    private:
        size_t size_edge_x, size_edge_y, size_edge_z, size_total;
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




