#include <gtest/gtest.h>
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
    tools::Parser parser(my_argc,my_argv);
    parser.addOption(pkgname(&foo), "foo",false);
    parser.addOption(pkgname(&too), "foo",false);

    printf("Foo: %s\n", foo?"True":"False");
    printf("too: %d\n", too);

}

//TEST(IO, get_files){
//std::string path = "/media/sc/SSD1TB/TrainingData/SceneNet/train";
//    size_t t = tools::PathTool::get_files_in_folder(path).size();
//}

int main(int argc, char ** argv){
    testing::InitGoogleTest(&argc,argv);
    my_argc = argc;
    my_argv = argv;
    return RUN_ALL_TESTS();
}