//
//  Tools.hpp
//
//  Created by Shun-Cheng Wu on 18/10/2017.
//
//

#ifndef PathTool_hpp
#define PathTool_hpp

#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#ifdef WIN32 //For create/delete files
#include <direct.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <sstream>
#include <unistd.h>
#include <stdexcept>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <sstream>

namespace tools {
    class PathTool{
    public:
        PathTool(){}
        //Folder related
        std::string get_executable_path ();
        /// if times=0, return itself, 1 the first parent folder. In the case of given file path, 1 will return it's folder
        std::string find_parent_folder(std::string input, int times=1);
        /** if success? 0: -1 */
        int remove_directory(const char *path);
        bool checkfileexist(const std::string& filename);
        bool checkfolderexist(const std::string& output_db_name);
        std::string find_parent_folder_name(const std::string& path);
        std::string get_current_dir_name(std::string path);
        
        void get_files_include_name (std::string path, std::string name, std::vector<std::string>& files_with_name);
        void get_files_include_name_recursively (std::string path, std::string name, std::vector<std::string>& files_with_name);

        std::vector<std::string> get_files_include_name (std::string path, std::string name);
        void check_and_delete_folder (const std::string& path);
        void check_and_create_folder (const std::string& path);
        /** Check whether folder exist. If not, create it. */
        void create_folder(std::string name);
        
        char* string2char(std::string string);
        
        /**
         This function will search all the files with given type within path (including folder, if no type is given).
         @param path The path to the folder you want to search
         @param type The type you want to search
         @param return_full Set true to return full path
         @param sort Set true the sort the result
         @return Return a vector
         */
        std::vector<std::string> get_files_in_folder (std::string path, std::string type = "", bool return_full = false, bool sort = true);
        
        void erase_chracter(std::string& input, std::string charact);
        void erase_charecter(std::string &input, char ch);
        void replace_chracter(std::string& input, std::string charact, std::string with_this);
        /// remove file type. if type not given, remove the string after "."
        std::string remove_file_type (std::string path, std::string type="");
        /// get File type
        std::string getFileType(std::string pathIn);
        /// Get total lines in the file
        int GetTotalLines(std::string file_path);
        
        std::vector<std::string> splitbychar(std::string line);
        
        std::string getFileName(std::string pthIn);
        
        std::string CheckEnd(std::string path);
        /// Break input string into tokens using the given delimiter
        std::vector<std::string> splitLine(std::string s, char delimiter);

        std::string addWaterSerialNumber(std::string path);

        bool isFolder(std::string path);
    };
    
} //End of namespace Tools






#endif /* Tools_hpp */
