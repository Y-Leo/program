#include <stdio.h>
#include <string>
#include <string.h>

#include "httplib.h"
#include "oj_model.hpp"
#include "oj_view.hpp"
#include "oj_log.hpp"

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

             /*
             //原始方法:
             //<html>id.name star</html>
             char buf[10240] = {"\0"};
             printf("%d\n", ques.size());
             if(ques.size() == 1)
             {
                snprintf(buf, sizeof(buf) - 1, "<html>%s.%s %s</html>", ques[0].id.c_str(), ques[0].name_.c_str(), ques[0].star_.c_str());
             }
             std::string html;
             html.assign(buf, strlen(buf));

             //stringstream ss;
             */

             //正确方法：使用模板技术去填充html界面
             
             std::string html;
             OjView::ExpandAllQuestionshtml(&html, ques);

             LOG(INFO, html);

             resp.set_content(html, "text/html; charset=UTF-8");
             });

     //正则表达式
     //    \b：单词的分界
     //    *：匹配任意字符串
     //    \d：匹配一个数字
     // 源码转译：特殊字符就按照特殊字符源码来编译
     // R"(str)"
     svr.Get(R"(/question/\d)", [&ojmodel](const Request& req, Response& resp){
             std::string html = "1";  
             resp.set_content(html, "text/html; charset=UTF-8");
             });
     svr.listen("0.0.0.0", 19999);
     return 0;
}
