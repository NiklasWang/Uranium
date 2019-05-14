using namespace std;

#include <iostream>
#include <functional>

void Print(function<void()> f) {
  f();
}

template<typename T>
T Print2(function<T()> f) {
  return f();
}

template<typename T>
void Print3(T f) {
  f();
}

template <typename T>
class A {
public:
  A(T f) {
    func = f;
  }
  void exe() {
    func("class A");
  }

private:
  T func;
};

class B {
public:
  B(function<void (const string &)> f) {
    func = f;
  }
  void exe() {
    func("class B");
  }

private:
  function<void (const string &)> func;
};

class C {
public:
  void doTask(string str) {
    cout << "I am " << str << endl;
  }
};

int main()
{
  string str1 = " Yes,";
  string str2 = " I";
  string str3 = " support";
  string str4 = " Lambda.";
  string str5 = "\n";

  [&]() { cout << str1; } ();

  [&](const string &s) { cout << s; } (str2);

  Print([&str3]() { cout << str3; });

  Print2<int>([=]() -> int { cout << str4; return 0; });

  Print3([str5]() { cout << str5; });

  C caller;
  A<function<void(string)> > a([&](string str) { caller.doTask(str); });
  B b([&](const string &str) { caller.doTask(str); });
  a.exe();
  b.exe();

  return 0;
}
