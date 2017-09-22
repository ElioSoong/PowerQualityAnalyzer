/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  StorageThread.h
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
#pragma once
#include <QThread>
#include <QFile>
#include <QMutex>

#include "StorageItem.h"


class StorageItem;
class StorageItemPosition;

class STORAGELIBRARY_EXPORT StorageIORequest
{
protected:
    int                 m_requestId;        //!< 递增的请求ID号，仅用于调试
    bool                m_writeNotRead;     //!< 当前请求为写请求
    StorageItem*        m_item;             //!< 实际完成读写操作的StorageItem
    void*               m_dataPtr;          //!< 读写操作对应的附加数据指针
    bool                m_removeReq;        //!< 当前请求为删除请求
    
    StorageItemPosition m_position;         //!< 读写起始位置

    static int          s_requestCounter;   //!< 请求计数器，仅用于调试

public:
    StorageIORequest();
    StorageIORequest(StorageItem* item, const StorageItemPosition& position, bool writeNotRead, void* dataPtr, bool removeReq = false);
    StorageIORequest(const StorageIORequest& other);
    ~StorageIORequest();

public:
    /*! 
     * 处理当前请求，该函数线程安全，可以在后台线程被调用，避免阻塞界面
     * @return     操作成功返回true
     */
    bool process(void);

    StorageItem * requestItem()
    {
        return m_item;
    }

    bool isRemoveReq()
    {
        return m_removeReq;
    }
};

Q_DECLARE_METATYPE(StorageIORequest)

/**
*  @class 
*  @brief 此线程的全部信号/槽均需要以排队的形式与其他信号/槽连接
*/
class STORAGELIBRARY_EXPORT StorageThread : public QThread
{
    Q_OBJECT

public:
    StorageThread(QObject *parent = 0);
    ~StorageThread();

public:
    void request(StorageIORequest& request)
    {
        emit pushRequest(request);
    }

    bool isInterrupet()
    {
        m_lock.lock();
        bool theInterrupted = m_interrupted;
        m_lock.unlock();
        return theInterrupted;
    }

    void interrupetRequest()
    {
        m_lock.lock();
        m_interrupted = true;
        m_lock.unlock();
    }

    void resetInterrupetFlag()
    {
        m_lock.lock();
        m_interrupted = false;
        m_lock.unlock();
    }

protected:
    Q_SIGNAL bool pushRequest(StorageIORequest request);
    Q_SLOT bool processRequest(StorageIORequest request);

public:
    Q_SIGNAL void doRemoveItem(StorageItem* item);
    Q_SLOT   void processRemoveItem(StorageItem* item);

    void run() Q_DECL_OVERRIDE;

protected:
    QMutex m_lock;                          //!< I/O请求队列安全访问所使用的锁
    bool  m_interrupted;                    //!< Loading请求中断标志
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

