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

#include "../include/DataWorker.h"
class SceneNetRGBD_Loader_iter : public tools::DataLoader<int> {
public:
    explicit SceneNetRGBD_Loader_iter(size_t max):counter(0){
        for(size_t i=0;i<max;++i)
            data[i]=i;
        size = data.size();
    }

    std::shared_ptr<int> get_item(int idx) override{
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto iter = data.begin();
        std::advance(iter,idx);
        if(iter == data.end()) return nullptr;
        std::shared_ptr<int> item(new int( iter->second ));
        return item;
    }

    size_t dataSize() override {
        return data.size();
    }

    int next() override {
        return counter < data.size()? counter++ : -1;
    }
private:
    int counter;
    size_t size;
    std::map<int,int> data;
};

TEST(DataWorker, worker_iter){
    printf("\n");
    size_t iter_max = 1e3;
    size_t max_iter=10;
    SceneNetRGBD_Loader_iter sceneNetRgbdLoader(iter_max);
    tools::DataWorker<int> dataWorker(&sceneNetRgbdLoader, 8);
    size_t iter=0;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    while(true){
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto data = dataWorker.get();
        if(data){
//            printf("iter, data: %zu %d\n", iter, *data);
            EXPECT_EQ(iter++, *data);
//            if(iter==max_iter)break;
        } else break;

    }
    EXPECT_EQ(iter, iter_max);
}
TEST(DataWorker, multi_worker_iter){
    printf("\n");
    size_t iter_max = 1e3;
    size_t max_iter=10;
    SceneNetRGBD_Loader_iter sceneNetRgbdLoader(iter_max);
    tools::MultiDataWorker<int> dataWorker(&sceneNetRgbdLoader, 8);
    size_t iter=0;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    while(true){
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto data = dataWorker.get();
        if(data){
//            printf("iter, data: %zu %d\n", iter, *data);
            EXPECT_EQ(iter++, *data);
//            if(iter==max_iter)break;
        } else break;

    }
    EXPECT_EQ(iter, iter_max);
}
int main(int argc, char ** argv){
    testing::InitGoogleTest(&argc,argv);
    my_argc = argc;
    my_argv = argv;
    return RUN_ALL_TESTS();
}