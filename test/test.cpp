#include <gtest/gtest.h>
#include <numeric>
#include "../include/PathTool.hpp"
#include "../include/parser.hpp"
int my_argc;
char** my_argv;

TEST(IO, find_folders_recursively_with_name){
    tools::PathTool pathTool;
    std::string path = "/media/sc/BackupDesk/SCSLAM/Result/ReprojectionError";
    std::string targetFolderName = "rgbd_dataset_freiburg1_360";
    std::vector<std::string> output;
    pathTool.get_folders_include_name_recursively(path, targetFolderName, output);

//    for(auto p : output){
//        printf("%s\n", p.c_str());
//    }
}

TEST(IO, get_targetFile_in_targetFolder_recursively){
    std::string path = "/media/sc/BackupDesk/SCSLAM/Result";
    std::string targetFolderName = "OFu";
    std::string targetFileName = "Summary.txt";
    std::vector<std::string> output;
    tools::PathTool::get_targetFile_in_targetFolder_recursively(path,targetFolderName,targetFileName, output);

    for(auto p : output){
        printf("%s\n", p.c_str());
    }
}

TEST(IO, sort) {
    /// Test for number order file/0.jpg file/1.jpg should be sorted correctly.
    auto paths = tools::PathTool::get_files_in_folder("/media/sc/SSD1TB/dataset/SceneNet/SceneNetRGBD-val/val/0/0/photo/","",true,true);
      for(const auto& path : paths)
          printf("%s\n", path.c_str());
}


TEST(IO, getFileType) {
    const std::vector<std::pair<std::string, std::string>> situations = {
            {".", ""},
            {"./", ""},
            {"./HELLO.cpp", ".cpp"},
            {"./Hello.io/main.cpp", ".cpp"}
    };

    for(const auto &st:situations) {
        EXPECT_EQ(tools::PathTool::getFileType(st.first), st.second);
    }
}

TEST(Parser, SWITCH) {
    bool foo = false;
    int too = 1;
    float f=1;
    std::string s="2";
    uint ui = 1;
    tools::Parser parser(my_argc,my_argv);
    parser.addOption(pkgname(&foo), "foo",false);
    parser.addOption(pkgname(&too), "foo",false);
    parser.addOption(pkgname(&f), "foo",false);
    parser.addOption(pkgname(&s), "foo",false);
    parser.addOption(pkgname(&ui), "foo",false);

    printf("Foo: %s\n", foo?"True":"False");
    printf("too: %d\n", too);

}

//TEST(IO, get_files){
//std::string path = "/media/sc/SSD1TB/TrainingData/SceneNet/train";
//    size_t t = tools::PathTool::get_files_in_folder(path).size();
//}

#include "../include/DataWorker.h"
//class SceneNetRGBD_Loader : public tools::DataLoader<int> {
//public:
//    explicit SceneNetRGBD_Loader(size_t max):iter_max(max), iter(0){}
//
//    std::shared_ptr<int> get_item() override{
////        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        if(iter >= iter_max) return nullptr;
//        std::shared_ptr<int> item(new int(iter));
//        return item;
//    }
//
//    size_t dataSize() override {
//        return iter_max;
//    }
//
//    int next() override {
//        return (iter++ < iter_max);
//    }
////    bool checkNext() override{return true;}
//private:
//    size_t iter_max, iter;
//};
//
//TEST(DataWorker, worker){
//    printf("\n");
//    size_t iter_max = 1e3;
//    SceneNetRGBD_Loader sceneNetRgbdLoader(iter_max);
//    tools::DataWorker<int> dataWorker(&sceneNetRgbdLoader, 8);
//
//    size_t iter=0;
//    while(true){
//        auto data = dataWorker.get();
//        if(data){
////            printf("iter, data: %zu %d\n", iter, *data);
//            EXPECT_EQ(iter++, *data);
//        } else break;
//    }
//}

class SceneNetRGBD_Loader_iter : public tools::DataLoader<int> {
public:
    explicit SceneNetRGBD_Loader_iter(size_t max):counter(0){
        data.resize(max);
        std::iota(data.begin(),data.end(),0);
//        iter = data.begin();
//        iter_max = data.end();
        size = data.size();
        printf("size: %zu\n", data.size());
        printf("iter+size+1 > iter_end: %d\n", data.begin()+data.size()+1 > data.end());
//        printf("iter_diff: %ld\n", iter_max-iter);
    }

    std::shared_ptr<int> get_item(int idx) override{
        auto iter = data.begin()+idx;
        if(iter >= data.end()) return nullptr;
        std::shared_ptr<int> item(new int( *iter ));
        return item;
    }

    size_t dataSize() override {
        return data.size();
    }

    int next() override {
        auto iter = data.begin()+counter;
        if(iter < data.end())
            return counter++;
        else
            return -1;
    }
//    bool checkNext() override{
//        bool state;
//        state = (iter++ != data.end());
//        iter+=5;
//        return state;
//    }
private:
    size_t counter;
    size_t size;
    std::vector<int> data;
//    std::vector<int>::iterator iter_max, iter;
};

TEST(DataWorker, worker_iter){
    printf("\n");
    size_t iter_max = 1e3;
    SceneNetRGBD_Loader_iter sceneNetRgbdLoader(iter_max);
    tools::DataWorker<int> dataWorker(&sceneNetRgbdLoader, 8);

    size_t iter=0;
    while(true){
        auto data = dataWorker.get();
        if(data){
            printf("iter, data: %zu %d\n", iter, *data);
            EXPECT_EQ(iter++, *data);
        } else break;
    }
}

int main(int argc, char ** argv){
    testing::InitGoogleTest(&argc,argv);
    my_argc = argc;
    my_argv = argv;
    return RUN_ALL_TESTS();
}