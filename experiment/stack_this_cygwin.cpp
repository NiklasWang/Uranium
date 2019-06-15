#include <iostream>
#include <QLibrary>

#include "cygload.h"
#include "stack_this.h"

#define CYGWIN_DLL_NAME  "cygwin1.dll"
#define CYGWIN_INIT_FUNC "cygwin_dll_init"

int main()
{
    cygwin::padding padding;
    cygwin::padding::check(&std::cout);
    typedef int32_t (*InitFunc)();

    QLibrary lib(CYGWIN_DLL_NAME);
    if (lib.load()) {
        InitFunc func = (InitFunc)lib.resolve(CYGWIN_INIT_FUNC);
        if (func != nullptr) {
            func();
        } else {
            std::cout << "Failed to init cygwin dll." << std::endl;
        }
    } else {
        std::cout << "Failed to load cygwin dll." << std::endl;
    }

   A *a = 0;
   std::cout << __func__ <<" a=" << a << std::endl;
   a = new A();
   std::cout << __func__ <<" a=" << a << std::endl;
   a->PrintV();

    return 0;
}
