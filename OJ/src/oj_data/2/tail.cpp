void Test1(Solution& s){
    int ret = s.maximum(1,2);
    if(ret == 2){
        std::cout<<"Test1 OK"<<std::endl;
    }
    else{
        std::cout<<"Test1 failed"<<std::endl;
    }
}

void Test2(Solution& s){
    int ret = s.maximum(2,1);
    if(ret == 2){
        std::cout<<"Test2 OK"<<std::endl;
    }
    else{
        std::cout<<"Test2 failed"<<std::endl;
    }
}

void Test3(Solution& s){
    int ret = s.maximum(10, 99);
    if(ret == 99){
        std::cout<<"Test3 OK"<<std::endl;
    }
    else{
        std::cout<<"Test3 failed"<<std::endl;
    }
}

int main(){
    Solution s;
    Test1(s);
    Test2(s);
    Test3(s);
    return 0;
}