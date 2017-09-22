/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StorageThread.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               用于异步存储的后台线程
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/01/05
** Descriptions:               用于异步存储的后台线程
** 
*********************************************************************************************************/
#include "IStorage.h"
#include "StorageItem.h"
#include "StorageThread.h"
#include "IField.h"
#include <QDebug>
#include <iostream>
using namespace std;

#ifdef WIN32
#undef  _IOW
#undef  _IOR
#undef  _IO
#define _IOW(_a, _b, _c)            0
#define _IOR(_a, _b, _c)            0
#define _IO(_a, _b)                 0
#endif


/*********************************************************************************************************
** StorageIORequest
*********************************************************************************************************/
int StorageIORequest::s_requestCounter = 0;

StorageIORequest::StorageIORequest()
{
    Q_ASSERT(0);
}

StorageIORequest::StorageIORequest(StorageItem* item, const StorageItemPosition& position, bool writeNotRead, void* dataPtr, bool removeReq) :
    m_item(item),
    m_position(position),
    m_writeNotRead(writeNotRead),
    m_dataPtr(dataPtr),
    m_removeReq(removeReq)
{
    if (++s_requestCounter == 0)
    {
        s_requestCounter = 1;
    }
    m_requestId = s_requestCounter;
}

StorageIORequest::StorageIORequest(const StorageIORequest& other) :
    m_item(other.m_item),
    m_position(other.m_position),
    m_writeNotRead(other.m_writeNotRead),
    m_dataPtr(other.m_dataPtr),
    m_requestId(other.m_requestId),
    m_removeReq(other.m_removeReq)
{

}

StorageIORequest::~StorageIORequest()
{
    m_requestId = 0;
    m_item = NULL;
    m_dataPtr = NULL;
}

/*! 
 * 处理当前请求，该函数线程安全，可以在后台线程被调用，避免阻塞界面
 * @return     操作成功返回true
 */
bool StorageIORequest::process(void)
{
    if (m_item == NULL || m_requestId == 0)
    {
        qWarning() << "Invalid storage I/O request detected";
        return false;
    }
    if (m_writeNotRead && (!Storage::availableDiskSpace()))
    {
        qWarning() << " The DiskSpace is Fulled!! ";
        Storage * storagePtr = Storage::instance();
        emit storagePtr->diskFull();

        // 磁盘空间满后非flush信号不予存储
        int dataTypeFlag = reinterpret_cast<int>(m_dataPtr);
        if (!(dataTypeFlag & FlushBeforeClosing))
        {
            return true;
        }
    }

    QFile* file = m_item->seek(&m_position);
    if (!file)
        return false;
    bool ok = false;
    if (m_writeNotRead)
    {
        ok = m_item->write(file, m_dataPtr);
    }
    else
    {
        ok = m_item->read(file, m_dataPtr);
    }

    // TODO: 是否需要在此处自动seek到下一位置？暂时不需要
    if (file && file->isOpen())
    {
        file->flush();
        file->close();
    }
    if (m_writeNotRead && !ok) 
    {
        file->remove();
    }
    return ok;
}

/*! 
 * 初始化成员变量
 */
StorageThread::StorageThread(QObject *parent)
    : QThread(parent) 
    , m_interrupted(false)
{
    this->moveToThread(this);
    connect(this, &StorageThread::pushRequest, this, &StorageThread::processRequest, Qt::QueuedConnection);
    connect(this, &StorageThread::doRemoveItem, this, &StorageThread::processRemoveItem, Qt::QueuedConnection);
}

StorageThread::~StorageThread()
{
    disconnect(this, &StorageThread::pushRequest, this, &StorageThread::processRequest);
    disconnect(this, &StorageThread::doRemoveItem, this, &StorageThread::processRemoveItem);
}

void StorageThread::processRemoveItem(StorageItem* item)
{
    Storage* storage = Storage::instance();
    storage->remove(item);
}

/*!
 * 存储线程
 */
void StorageThread::run() 
{
    exec();
}

bool StorageThread::processRequest(StorageIORequest request)
{
    Storage * storagePtr = Storage::instance();
    if (isInterrupet() || request.isRemoveReq())
    { 
        if (request.isRemoveReq()) 
        {
            StorageItem *item = request.requestItem();
            emit storagePtr->itemToRemove(item);
        }
        resetInterrupetFlag();
        return true;
    }
    else
        return request.process(); 
}
