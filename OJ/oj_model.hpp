#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "tools.hpp"

//试题信息有四个部分需要维护
//试题ID、试题名称、试题路径、试题难度
//用一个结构体来维护
typedef struct Questions 
{
    std::string id_;
    std::string name_;
    std::string path_;
    std::string star_;
} QUES;

class OjModel
{
    public:
        OjModel()
        {
            LoadQuestions("./config_oj.cfg");
        }
        bool GetAllQuestions(std::vector<Questions>* ques)
        {
            for(const auto& kv : model_map_)
            {
                ques->push_back(kv.second);
            }
            std::sort(ques->begin(),ques->end(),[](const Questions& l, const Questions& r){
                      return std::atoi(l.id_.c_str()) < std::atoi(r.id_.c_str());
                      });
            return true;
        }        
    private:
        bool LoadQuestions(const std::string& configfile_path)
        {
            std::ifstream file(configfile_path.c_str());
            if(!file.is_open())
            {
                 return false;
            }

            std::string line;
            while(std::getline(file, line))
            {
                //一行数据
                //1、切割字符串
                std::vector<std::string> vec;
                StringTools::Split(line, "\t", &vec);
                if(vec.size() != 4)
                {
                    continue;
                }
                //2、切割后的内容保存到unordered_map中去
                Questions ques;
                ques.id_ = vec[0];
                ques.name_ = vec[1];
                ques.path_ = vec[2];
                ques.star_ = vec[3];
                model_map_[ques.id_] = ques;
            }
            file.close();
            return true;
        }
    private:
        //管理方式：
        //map（底层红黑树）:map<key(id),value(TestQues)> model_map;
        //unordered_map（底层哈希表）：<key,value>
        std::unordered_map<std::string, Questions> model_map_;
};
