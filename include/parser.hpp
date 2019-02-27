//
//  Parser.hpp
//
//  Created by Shun-Cheng Wu on 07/11/2018.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <utility>

#define pkgname(var) pkgVar(#var, var)
template <typename T>
std::pair<std::string, T*> pkgVar(std::string name, T* var){
    if(name.find_first_of("&", 0, 1) != std::string::npos) {
        name.substr(1);
        name.erase(name.begin());
    }
    return std::make_pair(name, var);
}
namespace tools {
    class Parser{
        std::map<std::string, std::vector<std::string> > argvs;
        bool hasHelp_;
        struct Command {
            std::string explination;
            std::string value;
            bool required;
            bool handled;
        };
        std::map<std::string, Command> vRegisterd; // name : helper message
        std::vector<std::string> vOrder;
    public:
        Parser(int argc, char** argv);
        
        template <typename T>
        void addOption(std::pair<std::string, T*> pkg, std::string explaination = "", bool Required = false){
            const std::string& name = pkg.first;
            T* var = pkg.second;
            Command command;
            command.explination = explaination;
            command.required = Required;
            command.handled = false;
            
            if(hasHelp_ || find_switch("h") || find_switch("help")){
                hasHelp_ = true;
            } else {
                if(find_parse(name)){
                    process(name, *var);
                    command.handled = true;
                }
            }
            std::stringstream stn;
            stn << *var;
            command.value = stn.str();
            vRegisterd[name] = command;
            vOrder.push_back(name);
        }
        
        void addSwitch(std::pair<std::string, bool*> pkg, std::string explaination = "", bool Requied = false);
        
        /// 1: good, 0: warning, -1: error
        int showMsg();
        bool hasHelp();
    private:
        bool hasNotHandled();
        bool hasNotRegistered();
        
        template <typename T, typename ... Types>
        void process (std::string name, T& var, Types&& ... Args){
            static const std::size_t Tsize = sizeof...(Types);
            //            std::cout<< "Argsize: "<<Tsize << std::endl;
            
            process(name, var);
            for (int i=1; i<Tsize; ++i) {
                process(std::forward<Types>(Args)...);
            }
        };
        
        bool find_parse (std::string target);
        
        bool find_switch(std::string target);
        
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
        void process(std::string name, std::vector<std::string>& t);
        void process(std::string name, std::vector<int>& t);
        void process(std::string name, std::vector<float>& t);
        
        void process( std::string name, std::string& var);
        void process( std::string name, int& var) ;
        void process( std::string name, float& var);
        void process( std::string name, bool& var);
    };
}
#endif /* Parser_hpp */
