#pragma once
//直接包含，直接使用，缺点：可能会找不到头文件，所以报错 解决：在编译时使用-I选项指定头文件路径就可以了
//设置环境变量 CPLUS_INCLUDE_PATH
#include <ctemplate/template.h> 

#include <string>
#include <vector>
#include "oj_model.hpp"

class OjView
{
public:
    //渲染html界面，并且将该页面返还给调用
    static void ExpandAllQuestionshtml(std::string* html, std::vector<Questions>& ques)
    {
        //1、获取数据字典--->将拿到的试题数据按照一定顺序保存到内存当中
        ctemplate::TemplateDictionary dict("all_questions");

        for(const auto& que : ques)
        {
            ctemplate::TemplateDictionary* section_dict = dict.AddSectionDictionary("question");
            section_dict->SetValue("id", que.id_);
            section_dict->SetValue("id", que.id_);
            section_dict->SetValue("name", que.name_);
            section_dict->SetValue("star", que.star_);
        }
        //2、获取模板类指针，加载预定义的html界面到内存当中
        ctemplate::Template* t1 = ctemplate::Template::GetTemplate("./template/all_questions.html", ctemplate::DO_NOT_STRIP);
        //3、渲染，拿着模板类的指针，将数据字典中的数据更新到html页面的内存当中
        t1->Expand(html, &dict);
    }
};
