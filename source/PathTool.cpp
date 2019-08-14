//
//  Tools.cpp
//  pointnet_cpp
//
//  Created by Shun-Cheng Wu on 18/10/2017.
//
//

#include <PathTool.hpp>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>

namespace tools{
    std::string PathTool::get_executable_path()
    {
        unsigned int bufferSize = 512;
        std::vector<char> buffer(bufferSize + 1);
        
#if defined(_WIN32)
        ::GetModuleFileName(NULL, &buffer[0], bufferSize);
        
#elif defined(__linux__)
        // Get the process ID.
        int pid = getpid();
        
        // Construct a path to the symbolic link pointing to the process executable.
        // This is at /proc/<pid>/exe on Linux systems (we hope).
        std::ostringstream oss;
        oss << "/proc/" << pid << "/exe";
        std::string link = oss.str();
        
        // Read the contents of the link.
        int count = readlink(link.c_str(), &buffer[0], bufferSize);
        if(count == -1) throw std::runtime_error("Could not read symbolic link");
        buffer[count] = '\0';
        
#elif defined(__APPLE__)
        if(_NSGetExecutablePath(&buffer[0], &bufferSize))
        {
            buffer.resize(bufferSize);
            _NSGetExecutablePath(&buffer[0], &bufferSize);
        }
        
#else
#error Cannot yet find the executable on this platform
#endif
        
        std::string s = &buffer[0];
        return s;
    }

    std::string PathTool::find_parent_folder(std::string input, int times){
        assert(input.size()!=0 && "input size is zero");
        std::string output;
        if(input[input.size()-1] == '/') //input.pop_back();
        input = input.substr(0, input.length()-1);
        //        int count=2;//it's 2 because of '/' at the end and the wanted position
        //        int num=0;
        //        for (int i = input.size() - 1; i !=0; --i){
        //            if(input[i-1] != '/') {
        //                count++;
        //            } else {
        //                ++num;
        //            }
        //            if (num == times) {
        //                break;
        //            }
        //        }
        //        return output.assign(input, 0, input.size()-count);
        output = input;
        for (int i = 0; i < times; ++i){
            size_t temp = output.find_last_of("/");
            if (temp == std::string::npos){
                printf("[warning] Reach the most previous folder.");
                break;
            }
            output.assign(output, 0, output.find_last_of("/"));
        }
        return output;
    }
    
    int PathTool::remove_directory(const char *path)
    {
        DIR *d = opendir(path);
        size_t path_len = strlen(path);
        int r = -1;
        
        if (d)
        {
            struct dirent *p;
            
            r = 0;
            
            while (!r && (p=readdir(d)))
            {
                int r2 = -1;
                char *buf;
                size_t len;
                
                /* Skip the names "." and ".." as we don't want to recurse on them. */
                if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                {
                    continue;
                }
                
                len = path_len + strlen(p->d_name) + 2;
                buf = (char*)malloc(len);
                
                if (buf)
                {
                    struct stat statbuf;
                    
                    snprintf(buf, len, "%s/%s", path, p->d_name);
                    
                    if (!stat(buf, &statbuf))
                    {
                        if (S_ISDIR(statbuf.st_mode))
                        {
                            r2 = remove_directory(buf);
                        }
                        else
                        {
                            r2 = unlink(buf);
                        }
                    }
                    
                    free(buf);
                }
                
                r = r2;
            }
            
            closedir(d);
        }
        
        if (!r)
        {
            r = rmdir(path);
        }
        
        return r;
    }
    
    bool PathTool::checkfileexist(const std::string& filename)
    {
        struct stat buf;
        if (stat(filename.c_str(), &buf) != -1)
        {
            return true;
        }
        return false;
    }
    
    bool PathTool::checkfolderexist(const std::string& output_db_name){
#ifdef WIN32
        DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
        if (ftyp == INVALID_FILE_ATTRIBUTES)
            return false;  //something is wrong with your path!
        
        if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
            return true;   // this is a directory!
        
        return false;    // this is not a directory!
#else
        //        const char* folderr = output_db_name;
        //        folderr = output_db_name;
        struct stat sb;
        if (stat(output_db_name.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        {
            //            printf("YES\n");
            //            remove(output_db_name.c_str());
            //            remove_directory(output_db_name.c_str());
            return true;
        }
        else
        {
            return false;
            //            printf("NO\n");
        }
#endif
    }
    
    std::string PathTool::find_parent_folder_name(const std::string& path){
        std::string output, tmp;
        tmp = find_parent_folder(path, 1);
        output = path.substr(tmp.size()+1, path.size() - tmp.size());
        return output;
    }
    
    std::string PathTool::get_current_dir_name(std::string path){
        std::string output, tmp;
        tmp = find_parent_folder(path, 1);
        if(path[path.size()-1] == '/') //path.pop_back();
            path = path.substr(0, path.length()-1);
        output = path.substr(tmp.size()+1, path.size() - tmp.size());
        return output;
    }
    
    void PathTool::get_files_include_name (std::string path, std::string name, std::vector<std::string>& files_with_name) {
        auto files_all = get_files_in_folder(path, "");
        for (auto file: files_all) {
            auto boo = file.find(name);
            if (boo != std::string::npos) files_with_name.push_back(file);
        }
    }
    
    std::vector<std::string> PathTool::get_files_include_name (std::string path, std::string name){
        std::vector<std::string> files_with_name;
        auto files_all = get_files_in_folder(path, "");
        for (auto file: files_all) {
            auto boo = file.find(name);
            if (boo != std::string::npos) files_with_name.push_back(file);
        }
        return files_with_name;
    }
    
    void PathTool::check_and_delete_folder (const std::string& path){
        std::string tmp = path;
        bool isFolder = this->isFolder(tmp);
        if(!isFolder) tmp = this->find_parent_folder(tmp);
        if(checkfolderexist(tmp)) remove_directory(tmp.c_str());
    }
    
    void PathTool::check_and_create_folder (const std::string& path){
        std::string tmp = path;
        bool isFolder = this->isFolder(tmp);
        if(!isFolder) tmp = this->find_parent_folder(tmp);
        if(!checkfolderexist(tmp))create_folder(tmp);
    }
    
    void PathTool::create_folder(std::string name){
        bool isFolder = this->isFolder(name);
        if(!isFolder) {
            printf("[Warning][PathTool::create_folder] Input path has file type! Remove file type first! You can use remove_file_type() function.\n");
            return;
        }
        name = this->CheckEnd(name);

//#ifdef WIN32
//        _mkdir(name.c_str());
//#else
//        mkdir(name.c_str(), 0755);
//#endif
        auto first = name.find_first_of('/');
        auto second = name.find_last_of('/');
        if(first != second) {
            std::string motherFolder = name.substr(0, first+1);
            std::string rest = name.substr(first+1, name.length());
            while(true){
                auto f = rest.find_first_of('/');
                std::string folder;
                if(f == std::string::npos){
                    folder = motherFolder + rest;
                } else {
                    folder = motherFolder + rest.substr(0, f+1);
                }
                rest = rest.substr(f+1, rest.length());
#if defined _MSC_VER
                _mkdir(folder.c_str());
#else
                mkdir(motherFolder.c_str(), 0777);
#endif
                motherFolder = folder;
                if(f == std::string::npos){
                    break;
                }
            }
        } else {
            mkdir(name.substr(0, name.find_first_of('/')).c_str(), 0777);
        }
    }
    
    char* PathTool::string2char (std::string string){
        char *cstr = new char[string.length() + 1];
        strcpy(cstr, string.c_str());
        // do stuff
        return cstr;
        delete [] cstr;//?
    }
    
    std::vector<std::string> PathTool::get_files_in_folder (std::string path, std::string type, bool return_full, bool sort){
        std::vector<std::string> file_vec;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path.c_str())) != NULL) {
            path = CheckEnd(path);
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_name[0] != '.') {
                    /* print all the files and directories within directory */
                    //printf ("%s\n", ent->d_name);
                    file_vec.push_back(return_full? path+ent->d_name:ent->d_name);
                }
            }
            closedir (dir);
        } else {
            /* could not open directory */
            perror ("");
            EXIT_FAILURE;
        }
        if (sort) std::sort(file_vec.begin(),file_vec.end());
        
        if(type == "") return file_vec;
        
        std::vector<std::string> filtered;
        for (auto name: file_vec) {
            if(name.size() > type.size()) {
                std::string tmp = name.substr(name.size()-type.size(), type.size());
                if (tmp == type) filtered.push_back(name);
            }
        }
        return filtered;
    }
    
    void PathTool::erase_chracter(std::string& input, std::string charact){
        size_t a = input.find(charact);
        do {
            a = input.find(charact);
            if (a != std::string::npos){
                input.erase(a, charact.size());
            }
        } while (a != std::string::npos);
    }
    
    void PathTool::replace_chracter(std::string& input, std::string charact, std::string with_this){
        size_t a = input.find(charact);
        do {
            a = input.find(charact);
            if (a != std::string::npos){
                input.replace(a, charact.size(), with_this);
            }
        } while (a != std::string::npos);
    }
    
    std::string PathTool::remove_file_type (std::string path, std::string type) {
        if (type != "") {
            //std::string::size_type has_type = path.find_last_of(type);
            //assert(has_type != std::string::npos);
            return path.substr(0, path.size()-type.size());
        } else {
            std::string::size_type has_type = path.find_last_of(".");
            //assert(has_type != std::string::npos);
            return path.substr(0, has_type);
        }
    }
    
    std::string PathTool::getFileType(std::string pathIn){
        std::string::size_type has_type = pathIn.find_last_of(".");
        if(has_type == std::string::npos) {
            if(pathIn.back() == '/') return "folder";
            return "";
        }
        return pathIn.substr(has_type, pathIn.size());
    }
    
    int PathTool::GetTotalLines(std::string file_path){
        std::string line;
        std::fstream myfile;
        myfile.open(file_path.c_str());
        int counter=-1;
        assert(myfile.is_open()==true && "Unable to open file");
        do {
            counter++;
            getline(myfile,line,'\n');
        } while (!line.empty());
        return counter;
    }
    
    std::vector<std::string> PathTool::splitbychar(std::string line){
        std::vector<std::string> data;
        char* token;
        erase_chracter(line, "\r");
        token = strtok(string2char(line), "\t ,");
        while (token!=NULL) {
            data.push_back(token);
            token = strtok(NULL, "\t ,");
        }
        return data;
    }
    
    std::string PathTool::getFileName(std::string pthIn){
        return remove_file_type(get_current_dir_name(pthIn));
    }
    
    std::string PathTool::CheckEnd(std::string path){
        if(strcmp(&path[path.length()-1], "/")!=0){
            return path + "/";
        } else {
            return path;
        }
        return path;
    }


    std::vector<std::string> PathTool::splitLine(std::string s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            if(token != "")
                tokens.push_back(token);
        }
        return tokens;
    }

    std::string PathTool::addWaterSerialNumber(std::string path){
        std::string path_new = path;
        bool exist = false;
        std::string type = getFileType(path);
        bool isFolder = type == "";

        size_t counter=0;
        if(isFolder) {
            exist = checkfolderexist(path_new);
            while (exist) {
                path_new = CheckEnd(path); // check and add "/"
                path_new = path_new.substr(0, path_new.length() - 1); // remove "/"
                path_new += std::to_string(counter++);
                exist = checkfolderexist(path_new);
            }
        } else {
            exist = checkfileexist(path);
            while(exist) {
                path_new = remove_file_type(path);
                path_new += std::to_string(counter++) + type;
                exist = checkfileexist(path_new);
            }
        }
        return  path_new;
    }

    bool PathTool::isFolder(std::string path){
        std::string type = getFileType(path);
        return type == "";
    }
}
