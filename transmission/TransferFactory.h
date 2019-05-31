/**
 * @file TransferFactory.h
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-05-31
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */

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
private:
    TransferFactory() = default;
    ~TransferFactory();
    TransferFactory(const TransferFactory&) = delete;
    TransferFactory& operator=(const TransferFactory&) = delete;
};

};
#endif //__TRANSFERINTERFACE_H__
