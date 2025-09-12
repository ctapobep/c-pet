#include "../hello-lib/Hello.cpp"
using namespace std;

void blah(std::string** p) {
    std::cout << *p << '\n';
    p = nullptr;
}

int main() {
    string p = "blah";
    string* p1 = &p;
    cout << &p << '\n';
    blah(&((std::string*)&p));
    std::cout << &p << '\n';
    std::cout << &p1 << '\n';

    hello::WorldClass hiObj;
//    hiObj.hello(SOME_MACRO_NAME);
    return 0;
}
