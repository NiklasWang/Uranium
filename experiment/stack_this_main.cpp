#include <iostream>

#include "stack_this.h"

using namespace std;

int main()
{
   A *a = 0;
   cout << __func__ <<" a=" << a << endl;
   a = new A();
   cout << __func__ <<" a=" << a << endl;
   a->PrintV();

   return 0;
}