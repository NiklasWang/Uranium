#ifndef __TRANSFERFACTORY_H__
#define __TRANSFERFACTORY_H__

namespace uranium
{

class TransferFactory
{
public:
    static TransferFactory* create();
    void destory();
    TransferManager* createTransferObject(const TRANSFER_MODE_ENUM transModes, TRANSFER_STATUS_E tranDirct);
    TransferManager* createTransferObject(const TRANSFER_MODE_ENUM transModes, TRANSFER_STATUS_E tranDirct, std::string name, std::string passWd);
    virtual ~TransferFactory();

private:
    TransferFactory() = default;
    TransferFactory(const TransferFactory&) = delete;
    TransferFactory& operator=(const TransferFactory&) = delete;
};

};
#endif //__TRANSFERINTERFACE_H__
