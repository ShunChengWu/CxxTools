//
//  Tools.hpp
//  pointnet_cpp
//
//  Created by Shun-Cheng Wu on 18/10/2017.
//
//

#ifndef Tools_hpp
#define Tools_hpp

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef WIN32 //For create/delete files
#include <direct.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif


#include <fstream> //Load & Save file
#include <chrono> //For timer
#include <ctime>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <sstream>
#include <unistd.h>
#include <stdexcept>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <random>

namespace tools {
    std::vector<std::string> ReadDatainLine(std::string line, char* symbol);
    
    void ProgessBar (const int& current, const int& total, int type =0);
    
    template <typename T>
    T Getrandom(int min, int max){
        std::random_device rd;     // only used once to initialise (seed) engine
        std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        std::uniform_int_distribution<int> uni(min,max); // guaranteed unbiased
        return (T)uni(rng);
    }
    
    //!!!!: MATH
    class MATH{
    public:
        MATH(){};
        int factorial (int n, int t=1){
            do t *= n--; while (n > 0);
            return t;
        }
        int permutation(int n, int m, int t=1){
            for (int i=0; i<m;++i) t *= n-i;
            return t;
        }
        int combination(int n, int m, int t=1){return permutation(n, m, t)/factorial(m);}
        float Safeacos (float x){
            if (x < -1.0) x = -1.0 ;
            else if (x > 1.0) x = 1.0 ;
            return acos (x) ;
        }
    };
    
    
    template <typename tptr, typename t>
    std::vector<tptr> initpvec (int num){
        std::vector<tptr> vec;
        vec.reserve(num);
        for (int i=0; i< num;++i) vec.emplace_back(new t());
        return vec;
    }
    
    class parser{
        std::map<std::string, std::vector<std::string>> argvs;
        std::pair<std::string, std::vector<std::string>> pair;
    public:
        parser(int argc, char** argv){
            std::string current_parse = "";
            for (int i=0; i< argc; ++i) {
                std::string str = argv[i];
                if (str.find("--") != std::string::npos){
                    current_parse = str.substr(2, str.size());
                } else {
                    if (current_parse != "")
                        argvs[current_parse].push_back(argv[i]);
                }
            }
        };
        
        template <typename T, typename ... Types>
        void process (std::string name, T& var, Types&& ... Args){
            static const std::size_t Tsize = sizeof...(Types);
    //            std::cout<< "Argsize: "<<Tsize << std::endl;
            
            process(name, var);
            for (int i=1; i<Tsize; ++i) {
                process(std::forward<Types>(Args)...);
            }
        };
        
        bool find_parse (int argc, char** argv, std::string target){
            bool found = false;
            ;
            for (int i=0;i<argc;++i) {
                std::string strtmp = argv[i];
                if (!target.compare(strtmp) | found) found = true;
            }
            return found;
        }

    private:
        template <typename T>
        void process(std::string name, T& t){
            bool found = false;
            for (auto m: argvs) {
                if (m.first == name) {
                    found = true;
                    t.resize(m.second.size());
                    for (size_t i=0; i< m.second.size();++i) t.at(i) = m.second[i];
                }
            }
            if (!found){
                std::cout << "Cannot process command \"" << name << "\""  << std::endl;
            }
        }
        void process(std::string name, std::vector<std::string>& t){
            for (auto m: argvs) {
                if (m.first == name) {
                    t.resize(m.second.size());
                    for (size_t i=0; i< m.second.size();++i) t.at(i) = m.second[i];
                }
            }
        }
        void process(std::string name, std::vector<int>& t){
            for (auto m: argvs) {
                if (m.first == name) {
                    t.resize(m.second.size());
                    for (size_t i=0; i< m.second.size();++i) t.at(i) = std::stoi(m.second[i]);
                }
            }
        }
        void process(std::string name, std::vector<float>& t){
            for (auto m: argvs) {
                if (m.first == name) {
                    t.resize(m.second.size());
                    for (size_t i=0; i< m.second.size();++i) t.at(i) = std::stof(m.second[i]);
                }
            }
        }
        
        void process( std::string name, std::string& var) {
            for (auto m: argvs) {
                if (m.first == name) var = m.second[0];
            }
        }
        void process( std::string name, int& var) {
            for (auto m: argvs) {
                if (m.first == name) var = std::stoi(m.second[0]);
            }
        }
        void process( std::string name, float& var) {
            for (auto m: argvs) {
                if (m.first == name) var = std::stof(m.second[0]);
            }
        }
        void process( std::string name, bool& var) {
            for (auto m: argvs) {
                if (m.first == name) var = std::stoi(m.second[0]);
            }
        }
    };
    
    
    template <typename T>
    class timer{
    private:
        typedef std::chrono::time_point<std::chrono::system_clock> ttype;
        ttype timer_start;
        ttype timer_ecl1;
        ttype timer_ecl2;
        ttype timer_end;
        std::time_t start_time;
        std::time_t end_time;
    public:
        timer(){
            start();
        }
        void record(ttype& timer){timer=std::chrono::system_clock::now();}
        void start(){
            record(timer_start);
            record(timer_ecl1);
            start_time=end_time = std::chrono::system_clock::to_time_t(timer_start);
        }
        void end(){
            record(timer_end);
            end_time = std::chrono::system_clock::to_time_t(timer_end);
        }
        void tap(){record(timer_ecl1);}
        
        std::chrono::duration<T> duration(ttype t1, ttype t2){return (t1 - t2);}
        
        T get_eclipse(){return duration(timer_ecl1, timer_start).count();}
        
        T get_eclipse_last(){return duration(timer_ecl2, timer_ecl1).count();}
        
        /** Get total wall time */
        T get_total(){return duration(timer_end, timer_start).count();}
        
        /** Get start world time */
        char* get_start(){return std::ctime(&start_time);}
        
        /** Get end world time */
        char* get_end(){return std::ctime(&end_time);}
        
        //!!!!: First Level
        /** Get the time between now and start */
        T update_get_eclipse(){
            record(timer_ecl1);
            return duration(timer_ecl1, timer_start).count();
        }
        
        /** Get the time between now and last eclipse */
        T update_get_eclipse_last(){
            record(timer_ecl2);
            auto eclipse = duration(timer_ecl2, timer_ecl1);
            timer_ecl1 = timer_ecl2;
            return eclipse.count();
        }
        
        /** Get the time between end and start*/
        T update_get_total(){
            record(timer_end);
            return duration(timer_end, timer_start).count();
        }
        
        /** Start and Get start time (wall time) */
        char* update_get_start(){
            start();
            return get_start();
        }
        
        /** Start and Get end time (wall time) */
        char* update_get_end(){
            end();
            return std::ctime(&end_time);
        }
        
        /** type==0: hms; ==1: ms; ==2: s */
        std::string display(T second, size_t type=0){
            
            if(type==0){
                int hour = (int)second / (int)3600;
                int min  = int(second - hour*3600)/60;
                int sec  = second - int(hour*60+min)*60;
                return std::to_string(hour)+"(H), "+std::to_string(min)+"(M)" + std::to_string(sec) + "(S)";
            } else if (type == 1){
                int min  = int(second)/60;
                int sec  = second - min*60;
                return std::to_string(min)+"(M)" + std::to_string(sec) + "(S)";;
            } else {
                return std::to_string(second)+" (S)";
            }
        }
    };
    
    
    class FileStorage{
    private:
        std::map<std::string, std::string> content_map;
        std::fstream file_;
        
        char* string2char (std::string string){
            char *cstr = new char[string.length() + 1];
            strcpy(cstr, string.c_str());
            // do stuff
            return cstr;
            delete [] cstr;//?
        }
        
        void process_file (){
            std::string line;
            while (getline(file_, line)) {
//            do {
                //read each line
//                getline(file_, line, '\n');
                //detect whether '=' exist
                size_t equal = line.find("=");
                //            std::cout<< line << std::endl;
                if (equal != std::string::npos){
                    std::string string1, string2;
                    int counter=0;
                    
                    //detect space as seperate signal
                    char* pch;
                    
                    pch =strtok(string2char(line), "=");
                    while (pch != NULL){
                        if (strncmp (pch, "=", 1) !=0){
                            //                        printf("pch = %s\n", pch);
                            if (counter==0){
                                string1 = pch;
                                erase_chracter(string1, "\"");
                                erase_chracter(string1, "\r");
                                replace_chracter(string1, "\\", "/");
                                string1.erase(0,string1.find_first_not_of(" "));
                                string1.erase(string1.find_last_not_of(" ")+1);
                                //std::cout<<string1<<std::endl;
                            } else if (counter==1){
                                string2 = pch;
                                erase_chracter(string2, "\"");
                                erase_chracter(string2, "\r");
                                replace_chracter(string2, "\\", "/");
                                string2.erase(0,string2.find_first_not_of(" "));
                                string2.erase(string2.find_last_not_of(" ")+1);
                                //std::cout<<string2<<std::endl;
                            } else if (counter>=2){
                                //std::cout<<pch<<std::endl;
                                //std::cout <<"input format incorrect" <<std::endl;
                                //exit(-1);
                            }
                            counter++;
                        }
                        pch =strtok(NULL, "=");
                    }
                    content_map[string1]=string2;
//                    std::cout<< "content_map[" << string1 << "]: " << content_map[string1] <<std::endl;
                }
            } //while (line != "");
        }
        
    public:
        enum openmode {
            read, write
        } mode;
        FileStorage(){};
        FileStorage(std::string path, openmode read_type = read){
            std::map<openmode, std::ios_base::openmode> mode_map;
            mode_map[read] = std::ios::in;
            mode_map[write] = std::ios::out;
            file_.open(path, mode_map[read_type]);
            assert(file_.is_open() && "Unable to open file");
            process_file();
            file_.close();
        };
        void open(std::string path, openmode read_type = read){
            std::map<openmode, std::ios_base::openmode> mode_map;
            mode_map[read] = std::ios::in;
            mode_map[write] = std::ios::out;
            file_.open(path, mode_map[read_type]);
            assert(file_.is_open() && "Unable to open file");
            process_file();
            file_.close();
        }
        std::string find (std::string name){
            return content_map[name];
        }
        void erase_chracter(std::string& input, std::string charact){
            size_t a = input.find(charact);
            do {
                a = input.find(charact);
                if (a != std::string::npos){
                    input.erase(a, charact.size());
                }
            } while (a != std::string::npos);
        }
        void replace_chracter(std::string& input, std::string charact, std::string with_this){
            size_t a = input.find(charact);
            do {
                a = input.find(charact);
                if (a != std::string::npos){
                    input.replace(a, charact.size(), with_this);
                }
            } while (a != std::string::npos);
        }
    };
    
    class IOBinary {
    public:
        /* || Save each input vector as a single binary file || */
        /* Format: data */
        template<typename T>
        void SaveAsBinary(const std::vector<T>& data, std::string path){
            std::ofstream outfile;
            outfile.open (path, std::ios::out | std::ios::trunc | std::ios::binary);
            assert(outfile.is_open());
            outfile.write(reinterpret_cast<const char *>(data.data()), sizeof(T)*data.size());
            if (outfile.bad()) throw std::runtime_error("Failed to write to outfile!");
        };
        template<typename T>
        void LoadAsBinary(std::vector<T>& data, std::string path){
            data.clear();
            std::ifstream infile;
            infile.open (path, std::ios::in | std::ios::binary);
            assert(infile.is_open());
            infile.seekg(0, infile.end);
            auto N = infile.tellg();
            infile.seekg(0, infile.beg);
            data.resize(N/sizeof(T));
            infile.read(reinterpret_cast<char *>(data.data()), sizeof(T)*N);
            if (infile.bad())throw std::runtime_error("Failed to read from infile!");
        };
        /* || Save given vector<vector> into a single binary file || */
        /* Format: total_size, {{size, data}, ...} */
        /** Save multiple data into one file. First number is total size, follows by {size, data} */
        template<typename T>
        void SaveAsBinary_inOne(const std::vector<std::vector<T>>& datas, std::string path){
            std::ofstream outfile;
            outfile.open (path, std::ios::out | std::ios::trunc | std::ios::binary);
            assert(outfile.is_open());
            size_t total_size = datas.size();
            outfile.write(reinterpret_cast<const char *>(&total_size), sizeof(size_t));
            for(auto& data: datas){
                size_t size = data.size();
                outfile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
                outfile.write(reinterpret_cast<const char *>(data.data()), sizeof(T)*size);
                if (outfile.bad()) throw std::runtime_error("Failed to write to outfile!");
            }
        }
        /** Read multiple data from one file. First number is total size, follows by {size, data} */
        template<typename T>
        void LoadAsBinary_inOne(std::vector<std::vector<T>>& datas, std::string path, bool clear_vec = false){
            if(clear_vec) datas.clear();
            std::ifstream infile;
            infile.open (path, std::ios::in | std::ios::binary);
            assert(infile.is_open());
            size_t total_size;
            infile.read(reinterpret_cast<char *>(&total_size), sizeof(size_t));
            if (infile.bad())throw std::runtime_error("Failed to read total size from infile!");
            datas.resize(total_size);
            for(size_t i=0; i<datas.size();++i){//TODO: test it
                size_t size;
                infile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
                datas[i].resize(size);
                infile.read(reinterpret_cast<char *>(datas[i].data()), sizeof(T)*size);
                if (infile.bad())throw std::runtime_error("Failed to read from infile!");
            }
        }
        /* || Gradually save given vector into the same binary file || */
        /* Format: {{size, data}, ...} */
        /** Create/Reset a binary file */
        void resetFile(std::string path){
            std::ofstream outfile;
            outfile.open (path, std::ios::out | std::ios::trunc | std::ios::binary);
            assert(outfile.is_open() && "The file cannot be opened.\n");
        }
        /** Append given data to the end of the file given by path */
        template<typename T>
        void SaveAsBinary_Append(std::vector<T>& data, std::string path){
            std::ofstream outfile;
            outfile.open (path, std::ios::app | std::ios::out | std::ios::binary);
            assert(outfile.is_open() && "The file cannot be opened.\n");
            size_t size = data.size();
            outfile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
            outfile.write(reinterpret_cast<const char *>(data.data()), sizeof(T)*size);
        }
        /** Read multiple data from one file. Format: {size, data}. if clear_vec is true, the input datas will be clear first. */
        template<typename T>
        void LoadAsBinary_inOne_noTotal(std::vector<std::vector<T>>& datas, std::string path, bool clear_vec = false){
            if(clear_vec) datas.clear();
            std::ifstream infile;
            infile.open (path, std::ios::in | std::ios::binary);
            assert(infile.is_open());
            while (infile){
                size_t size;
                infile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
                if (infile.eof()) break;//????: 好像在到底之後還要再讀一次才會到達eof，之前的程式碼可能要確認一下
                std::vector<T> tmp_vec;
                tmp_vec.resize(size);
                infile.read(reinterpret_cast<char *>(tmp_vec.data()), sizeof(T)*size);
                datas.push_back(tmp_vec);
//                std::cout<<  datas.size() << "]size: " << size << std::endl;
            }
        }
        /* || Save given vector<vector> into a single binary file || */
        /* Format: {{size, data}, ...} */
        /** Save multiple data into one file. First number is total size, follows by {size, data} */
        template<typename T>
        void SaveAsBinary_inOne_noTotal(const std::vector<std::vector<T>>& datas, std::string path){
            std::ofstream outfile;
            outfile.open (path, std::ios::out | std::ios::trunc | std::ios::binary);
            assert(outfile.is_open());
            for(auto& data: datas){
                size_t size = data.size();
                outfile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
                outfile.write(reinterpret_cast<const char *>(data.data()), sizeof(T)*size);
                if (outfile.bad()) throw std::runtime_error("Failed to write to outfile!");
            }
        }
    };
    
    struct centroid {
        std::vector<float> x, y, z;
        float cx, cy, cz; //centriod
        struct max { //max&min
            float x_max, y_max, z_max, x_min, y_min, z_min, xt, yt, zt, tt;
        } max;
        int size, nan=0;
        float x_len, y_len, z_len;
      
        void initialise(std::vector<float> input){
            size = (int)input.size()/3;
            assert(size > 0 && "Input size cannot be zero");
            x.resize(size);
            y.resize(size);
            z.resize(size);
            
            for (int i=0; i<size;++i){
                if(input[3*i+0] == input[3*i+0] && input[3*i+1] == input[3*i+1] && input[3*i+2] == input[3*i+2]){
                    x[i]=input[3*i+0];
                    y[i]=input[3*i+1];
                    z[i]=input[3*i+2];
                } else {
                    ++nan;
                }
            }
            x.resize(size-nan);
            y.resize(size-nan);
            z.resize(size-nan);
        };
        void find_centroid (){
            cx = std::accumulate(x.begin(), x.end(), 0.0)/x.size();
            cy = std::accumulate(y.begin(), y.end(), 0.0)/y.size();
            cz = std::accumulate(z.begin(), z.end(), 0.0)/z.size();
        };
        void find_max(){
            max.x_max = *std::max_element(x.begin(), x.end());
            max.x_min = *std::min_element(x.begin(), x.end());
            max.y_max = *std::max_element(y.begin(), y.end());
            max.y_min = *std::min_element(y.begin(), y.end());
            max.z_max = *std::max_element(z.begin(), z.end());
            max.z_min = *std::min_element(z.begin(), z.end());
            max.xt =(max.x_max>std::fabs(max.x_min))? max.x_max : std::fabs(max.x_min);
            max.yt =(max.y_max>std::fabs(max.y_min))? max.y_max : std::fabs(max.y_min);
            max.zt =(max.z_max>std::fabs(max.z_min))? max.z_max : std::fabs(max.z_min);
            max.tt = (max.xt<max.yt)? (max.yt<max.zt)? max.zt:max.yt :(max.xt<max.zt)? max.zt:max.xt;
            x_len = std::fabs(max.x_max-max.x_min);
            y_len = std::fabs(max.y_max-max.y_min);
            z_len = std::fabs(max.z_max-max.z_min);
        };
        
        void move_centroid_to_origion (std::vector<float>& input){
            size = (int)input.size()/3;
            for (int i=0;i<size;++i){
                input[3*i+0] -= cx;
                input[3*i+1] -= cy;
                input[3*i+2] -= cz;
            }
        }
        
        void show(){
            printf("[max.xt, max.yt, max.zt, max.tt]: [%5.3f, %5.3f, %5.3f, %5.3f]\n", max.xt, max.yt,max.zt,max.tt);
            printf("[max.x_max, max.x_min, max.y_max, max.y_min, max.z_max, max.z_min]: [%5.3f, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f]\n",max.x_max, max.x_min, max.y_max, max.y_min, max.z_max,max.z_min);
            printf("[cx, cy, cz]: [%5.3f, %5.3f, %5.3f]\n", cx, cy, cz);
            std::cout<<"size: "<<size<<std::endl;
        }
    };
    
    /*!
     * @discussion Normalise input data in to any rance. (Default is 0~1)
     * @param times: if the range will mul times. e.g. times=5, output range is 0~5.
     * @param bias : range will plus bias after mul times. e.g. times=2, bias=1, output is 1~3
     */
    template <typename T>
    void norm_to_range(std::vector<T> input, int times = 1, int bias = 0){
        std::pair<typename std::vector<T>::iterator, typename std::vector<T>::iterator> minmax = std::minmax_element(input.begin(), input.end());
        int min = input[minmax.first - input.begin()];
        int max = input[minmax.second - input.begin()];
        for (auto data:input) {
            data = times * (data - min) / (max - min) - bias;
        }
    }
    
    /*!
     * @discussion Normalise input data in to any rance. (Default is abs(0~1))
     * @param times: if theㄆ range will mul times. e.g. times=5, output range is 0~5.
     * @param bias : range will plus bias after mul times. e.g. times=2, bias=1, output is 1~3
     */
    
    /**
     Normalise input 3D point in to a sphere range (Default is unit circle)

     @param input Input vector with template type
     @param times This can be used to scale the raidua

     */
    template <typename T>
    void norm_to_range_3dpoint(std::vector<T>& input, int times = 1){
        int size = input.size()/3;
        float max=0, min=0;
        std::vector<float> dis(size);
        
        int j=0;
        for (int i=0; i<size;++i) {
            dis[i] = sqrtf(input[j]*input[j]+input[j+1]*input[j+1]+input[j+2]*input[j+2]);
            j += 3;
        }
        std::pair<std::vector<float>::iterator, std::vector<float>::iterator> minmax;
        minmax = std::minmax_element(dis.begin(), dis.end());
        min = dis[minmax.first  - dis.begin()];
        max = dis[minmax.second - dis.begin()];
        if (max != 0) {
            for (int i=0; i<size*3; ++i) {
                //input[j] = max != min? times * (input[j++] - min) / (max - min) + bias : 0;
                input[i] = max != min? times * (input[i] / max): 0;
            }
        }
    }
    
    class EIGEN{
    public:
        Eigen::Matrix3f getRotmatrixfromAtoB(const Eigen::Vector3f& vec1, const Eigen::Vector3f& vec2){
            if(vec1 == vec2) {
                return Eigen::Matrix3f::Identity();
            }
            Eigen::Vector3f crossProduct = vec1.cross(vec2);
            Eigen::Vector3f vector_X = (crossProduct / crossProduct.norm());
            float thetaAngleRad = MATH().Safeacos(vec1.dot(vec2)/(vec1.norm()*vec2.norm()));
#if 0
            Eigen::Matrix3f matrix_A = Eigen::Matrix3f::Identity();
            matrix_A(0,0) = 0.0;
            matrix_A(0,1) = -1.0 * (vector_X(2));
            matrix_A(0,2) = vector_X(1);
            matrix_A(1,0) = vector_X(2);
            matrix_A(1,1) = 0.0;
            matrix_A(1,2) = -1.0 * (vector_X(0));
            matrix_A(2,0) = -1.0 * (vector_X(1));
            matrix_A(2,1) = vector_X(0);
            matrix_A(2,2) = 0.0;
            
            Eigen::Matrix3f IdentityMat = Eigen::Matrix3f::Identity();
            Eigen::Matrix3f firstTerm = sin(thetaAngleRad) * matrix_A;
            Eigen::Matrix3f secondTerm = (1.0 - cos(thetaAngleRad)) * matrix_A * matrix_A;

            Eigen::Matrix3f matrix_R = IdentityMat + firstTerm + secondTerm;
#endif
            Eigen::AngleAxis<float> aa(thetaAngleRad, vector_X);
            Eigen::Matrix3f matrix_R = aa.toRotationMatrix();
            
            
            return matrix_R;
        };
        Eigen::Matrix4f convertRotmarixTo4f(const Eigen::Matrix3f& mat3f){
            Eigen::Matrix4f mat4f;
            mat4f <<
            mat3f(0,0), mat3f(0,1), mat3f(0,2), 0,
            mat3f(1,0), mat3f(1,1), mat3f(1,2), 0,
            mat3f(2,0), mat3f(2,1), mat3f(2,2), 0,
            0,0,0,1;
            return mat4f;
        };
        /** rotate input matrix based on the given angle and axis*/
        template <typename T>
        const Eigen::Matrix<float, 3, -1, 0, 3, -1> rotateMatFromAngleAxis
        (const T& matrix, float angle, Eigen::Vector3f axis = Eigen::Vector3f(0,0,1)){
            Eigen::AngleAxis<float> rot(angle,axis);
            Eigen::Affine3f transform = Eigen::Affine3f::Identity();
            transform.rotate(rot);
            return transform * matrix;
        };
    };

} //End of namespace Tools






#endif /* Tools_hpp */
