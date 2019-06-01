#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <stdint.h>
#include <string>

namespace uranium {

class Configs {
public:
    int32_t update();
    int32_t load();
    int32_t save();

public:
    int32_t construct();
    int32_t destruct();
    Configs();
    virtual ~Configs();

private:
    std::string mUserName;
    std::string mPassword;
    std::string mPath;

};

}

#endif