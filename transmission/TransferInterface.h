#ifndef __TRANSFERINTERFACE_H__
#define __TRANSFERINTERFACE_H__

namespace uranium
{
#if 0
typedef enum  TREANFER_EVENT_ENUM {
    CONFIG_EVT = 0,
    CONFIG_ACK,
    DIR_MO_EVT,
    DIR_MO_ACK
} TREANFER_EVENT_E;

typedef enum DIR_MO_EVT_ENUM {
    DIR_ASK_DIR_ISEMPTY = 0,
    DIR_ASK_DIR_NOEMPTY,
    DIR_ASK_DIR_LOAD_MD5INFOS,
    DIR_ASK_DIR_LOAD,
    DIR_ASK_DIR_STORE,
    DIR_ASK_DIR_MODATA
} DIR_MO_EVT_E;

typedef struct TRANSFER_HEADE_TAG {
    TREANFER_EVENT_E evtKey;
    uint32_t         evtValue;
} TRAN_HEADE_T
#endif
typedef enum TRANSFER_MODE_ENUM {
    TRAN_MODE_FEX,
    TRAN_MODE_SOCKET
} TRANSFER_MODE_E;

typedef enum TRANSFER_STATUS_ENUM {
    TRAN_CLINET,
    TRANS_SERVER
} TRANSFER_STATUS_E;

typedef struct  TRANSFER_BUFFER_TAG {
    TRANSFER_MODE_E mode;
    // TRAN_HEADE_T    *tranHead;
    void            *buffer;
    uint32_t        length;
} TRANSFER_BUFFER_T;

class TransferManager
{
public:
    TransferManager();
    virtual ~TransferManager();
    virtual uint32_t pushData(TRANSFER_BUFFER_T &cmd) = 0;
    virtual uint32_t pullData(TRANSFER_BUFFER_T &cmd) = 0;
    TRANSFER_BUFFER_T* createTransferBuffer(void);
    TRANSFER_BUFFER_T* createTransferBuffer(TRANSFER_MODE_E mode,  void *pBuffer, uint32_t length);
    void destoryTransferBuffer(const TRANSFER_BUFFER_T* pTransBuf);

private:
    ModuleType          mModule;
    TRANSFER_STATUS_E   mTranDirct;
};

extern int __TransferMain(int argc, char **argv);

}; /* namespace uranium */
#endif //__TRANSFERINTERFACE_H__
