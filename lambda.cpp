// 标准的lambda表达式语法结构
// capture clause 捕捉上下文存在的变量供lambda表达式使用
// mutable取消const函数性质
// returntype可以省略，用auto自动获取
// [capture clause] (parameters) mutable -> returnType {
//     functionbody;
// }
#include <iostream>
#include <vector>
#include <algorithm>
auto l = [](){
    std::cout << "lambda\n";
};

//捕获方式：按值：创建拷贝，按引用：直接使用外部变量。

// auto f = [id]() mutable{};
// 等价于
// class Functor{
// private:
//     int id;//如果没有mutable，则这里是const int id;
// public:
//     void operator() (){}
// };
// [=]按值捕获,[&]按引用捕获

// 传递lambda表达式，将其转化为函数指针。使用std::function，或者命名直接调用
#include <functional>
void foo(std::function<bool(int)> f) {
    std::cout << f(3) << std::endl;
}

// 使用decltype推导类型，并且作为模板参数传递。
template <typename Fun>
void baz(Fun f) {
    std::cout << f(3,3) << std::endl;;
}

auto mul = [](int a, int b) {
    return a+b;
};



int main(){
    l();
    foo([](int i){
        return i == 0;
    });
    baz<decltype(mul)>(mul);


    //实现dfs算法
    std::vector<int> t1(199), t2(199);
    std::function<int(int, int)> dfs = [&](int i, int j) -> int {
        if (i == 0||j ==0) {
            return 0;
        }
        if (t1[i-1] == t2[j-1]) {
            return dfs(i-1, j-1) + 1;
        } else {
            return std::max(dfs(i-1, j), dfs(i, j-1));
        }
    };
    dfs(199,199);
    return 0;
}