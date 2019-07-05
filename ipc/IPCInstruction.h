#ifndef _IPC_INSTRUCTION_H_
#define _IPC_INSTRUCTION_H_

namespace uranium {

#define GUI_SOCK_PORT   8081
#define CORE_SOCK_PORT  8084

#define CORE_INIT       "CORE_INIT"
#define CORE_START      "CORE_START"
#define CORE_STOP       "CORE_STOP"
#define CORE_GET_CONFIG "CORE_GET_CONFIG"
#define CORE_SET_CONFIG "CORE_SET_CONFIG"
#define CORE_EXIT       "CORE_EXIT"

#define GUI_DEBUG       "GUI_DEBUG"
#define GUI_SHELL       "GUI_SHELL"

#define GREETING_GUI    "Hi, GUI"
#define BYE_GUI         "Bye, GUI"

#define REPLY_SUCCEED   "Succeed"
#define REPLY_FAILED    "Failed"

#define BOOL_TRUE       "TRUE"
#define BOOL_FALSE      "FALSE"

}

#endif
