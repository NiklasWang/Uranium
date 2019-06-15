#include <iostream>

#include "stack_this.h"

using namespace std;

A::A()
{
    v = 0x1;
    cout << __func__ << " this = " << this << endl;
}

void A::PrintV()
{
    cout << __func__ << " value = " << v << endl;
    cout << __func__ << " this = " << this << endl;
}
