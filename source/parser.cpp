//
//  Parser.hpp
//
//  Created by Shun-Cheng Wu on 07/11/2018.
//
#include <parser.hpp>

namespace tools{
    Parser::Parser(int argc, char** argv){
        hasHelp_ = false;
        std::string current_parse = "";
        for (int i=1; i< argc; ++i) {
            std::string str = argv[i];
            if (str.compare(0, 2, "--") == 0){
                current_parse = str.substr(2, str.size());
                argvs[current_parse].resize(0);//if size 0, it is an option
            } else if (str.compare(0, 1, "-") == 0) {
                current_parse = str.substr(1, str.size());
                argvs[current_parse].resize(0);//if size 0, it is an option
            } else {
                if (current_parse != "")
                    argvs[current_parse].push_back(argv[i]);
                
            }
        }
    };

    void Parser::addSwitch(std::pair<std::string, bool*> pkg, std::string explaination, bool Requied){
        const std::string& name = pkg.first;
        bool* var = pkg.second;
        Command command;
        command.explination = explaination;
        command.required = Requied;
        command.handled = false;
        
        if(hasHelp_ || find_switch("h") || find_switch("help")){
            hasHelp_ = true;
        } else {
            if(find_switch(name)) {
                *var = true;
                command.handled = true;
            }
        }
        std::stringstream stn;
        stn << *var;
        command.value = stn.str();
        vRegisterd[name] = command;
        vOrder.push_back(name);
    }

    /// 1: good, 0: warning, -1: error
    int Parser::showMsg(){
        int maxLengh = 0;
        for(auto& name:vOrder){
            if((int)name.size()>maxLengh) maxLengh = (int)name.size();
        }
        if(find_switch("h") || find_switch("help")){
            for (auto& name : vOrder) {
                auto& command = vRegisterd[name];
                if(command.required)
                    printf("--%-*s \t(REQUIRED) %s (default: %s)\n", maxLengh, name.c_str(), command.explination.c_str(), command.value.c_str());
                else
                    printf("--%-*s \t%s (default: %s)\n", maxLengh, name.c_str(), command.explination.c_str(), command.value.c_str());
            }
            return 1;
        }
        if(hasNotHandled()) return -1;
        if(hasNotRegistered())return 0;
        {
            for (auto& name : vOrder) {
                auto& command = vRegisterd[name];
                printf("--%-*s \t%s\n", maxLengh, name.c_str(), command.value.c_str());
            }
        }
        return 1;
    }

    bool Parser::hasHelp(){return hasHelp_;}

    bool Parser::hasNotHandled(){
        std::vector<std::string> vNotHandled;
        for (auto& pair : vRegisterd) {
            if(pair.second.required && !pair.second.handled){
                vNotHandled.push_back(pair.first);
            }
        }
        if(vNotHandled.size()){
            printf("[Error] The following argument(s) should be given. Pass -h for help\n");
            for(auto& noth : vNotHandled){
                printf("\t\t %s\n", noth.c_str());
            }
            return true;
        }
        return false;
    }
    bool Parser::hasNotRegistered(){
        std::vector<std::string> vNotRegistered;
        for (auto& arg : argvs) {
            if(vRegisterd.count(arg.first) == 0){
                vNotRegistered.push_back(arg.first);
            }
        }
        if(vNotRegistered.size()){
            printf("[Warning] Unknown argument(s) given. Please check the input arguments.\n");
            for(auto& noth : vNotRegistered){
                printf("\t\t %s\n", noth.c_str());
            }
            return true;
        }
        return false;
    }

    bool Parser::find_parse (std::string target){
        if(argvs.count(target)){
            if(argvs[target].size() > 0)
                return true;
        }
        return false;
    }

    bool Parser::find_switch(std::string target){
        if(argvs.count(target)){
            if(argvs[target].size() == 0)
                return true;
        }
        return false;
    }

    void Parser::process(std::string name, std::vector<std::string>& t){
        for (auto m: argvs) {
            if (m.first == name) {
                t.resize(m.second.size());
                for (size_t i=0; i< m.second.size();++i) t.at(i) = m.second[i];
            }
        }
    }
    void Parser::process(std::string name, std::vector<int>& t){
        for (auto m: argvs) {
            if (m.first == name) {
                t.resize(m.second.size());
                for (size_t i=0; i< m.second.size();++i) t.at(i) = std::stoi(m.second[i]);
            }
        }
    }
    void Parser::process(std::string name, std::vector<float>& t){
        for (auto m: argvs) {
            if (m.first == name) {
                t.resize(m.second.size());
                for (size_t i=0; i< m.second.size();++i) t.at(i) = std::stof(m.second[i]);
            }
        }
    }

    void Parser::process( std::string name, std::string& var) {
        for (auto m: argvs) {
            if (m.first == name) var = m.second[0];
        }
    }
    void Parser::process( std::string name, int& var) {
        for (auto m: argvs) {
            if (m.first == name) var = std::stoi(m.second[0]);
        }
    }
    void Parser::process( std::string name, float& var) {
        for (auto m: argvs) {
            if (m.first == name) var = std::stof(m.second[0]);
        }
    }
    void Parser::process( std::string name, bool& var) {
        for (auto m: argvs) {
            if (m.first == name) var = std::stoi(m.second[0]);
        }
    }
}