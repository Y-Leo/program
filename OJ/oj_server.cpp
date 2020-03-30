#include "httplib.h"
#include "oj_model.hpp"

int main()
{
     //httplib的时候，需要使用httplib提供的命名空间
     using namespace httplib;
     Server svr;
     OjModel ojmodel;
     //lambda[] 表示的引出符，lambda 函数的捕获列表
     //[var]:值传递方式将var变量传递到lambda函数当中去
     //[=]:值传递方式捕获所有父作用域当中的变量
     //[&var]:引用传递var
     //[&]:引用传递方式捕获所有父作用域当中的变量
     //[this]:值传递this指针
     
     //给http传递lambda表达式是一回调函数的方式
     //1、要获取试题的信息
     //   试题的信息来源于文件当中（读试题文件，获取信息）
     svr.Get("/all_questions", [&ojmodel](const Request& req, Response& resp){
             std::vector<Questions> ques;
             ojmodel.GetAllQuestions(&ques);
             //想使用模板技术去填充html界面
             resp.set_content("", "text/html");
             });
     svr.listen("0.0.0.0", 19999);
     return 0;
}
