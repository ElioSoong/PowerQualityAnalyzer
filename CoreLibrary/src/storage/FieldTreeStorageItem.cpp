/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldTreeStorageItem.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2015/12/31
** Descriptions:               
** 
*********************************************************************************************************/
#include "FieldLibrary/IField.h"
#include "FieldTreeStorageItem.h"
#include "FieldProxyModel.h"
#include "IStorage.h"
#include "SessionStorageItem.h"

#include <QAbstractItemModel>
#include <QFile>
#include <QDataStream>
#include <QStack>

#define WHITELIST_PATH       "/FieldItem.WL"
#define FIELDINFO_PATH       "/FieldItem.INFO"

FieldTreeStorageItem::FieldTreeStorageItem()
    : StorageItem()
    , m_whiteList()
    , m_fieldInfo()
{

}

FieldTreeStorageItem::~FieldTreeStorageItem()
{

}

void FieldTreeStorageItem::setFieldInfo(IField* field)
{
    if (field->isVirtual())
    {
        field = field->child(0);
    }
    m_fieldInfo.FrameSize = field->data(field->child(0)->childIndex(), -1, FrameSizeRole).toInt();
    m_fieldInfo.PointSize = field->data(field->child(0)->childIndex(), -1, PointSizeRole).toInt();
    m_fieldInfo.ChannelCount = field->data(field->child(0)->childIndex(), -1, ChannelCountRole).toInt();
    m_fieldInfo.FlushNumber = field->data(field->child(0)->childIndex(), -1, FlushNumberRole).toInt();
}

void FieldTreeStorageItem::setDataPtr(void* data)
{
    QAbstractItemModel*theModel = reinterpret_cast<QAbstractItemModel*>(data);
    if (theModel)
    {
        FieldProxyModel* model = qobject_cast<FieldProxyModel*>(m_model);
        if (model)
        {
            QStringList theList = model->whiteList();
            setModel(model);
            setWhiteList(theList);
            if (theList.size() > 0)
            {
                IField* field = IField::at(theList.at(0));
                setFieldInfo(field);
            }
        }
    }
    return StorageItem::setDataPtr(data);
}

bool FieldTreeStorageItem::reload(quint64 startTime /*= 0*/, quint64 endTime /*= 0*/, int type /*= 0*/)
{
    Q_UNUSED(startTime);
    Q_UNUSED(endTime);
    Q_UNUSED(type);
    if (!loadItemInfo(m_path, m_info))
    {
        qDebug() << "FieldTreeStorageItem loadItemInfo Failed !!";
        return false;
    }

    if (!loadWhiteList())
    {
        qDebug() << "FieldTreeStorageItem loadWhiteList Failed !!";
        return false;
    }

    if (!loadFieldInfo())
    {
        qDebug() << "FieldTreeStorageItem loadFieldInfo Failed !!";
        return false;
    }
    return true;
}

bool FieldTreeStorageItem::saveWhiteList() const
{
    QFile file(m_path + WHITELIST_PATH);
    if (!file.open(QFile::WriteOnly))
        return false;

    QDataStream stream(&file);
    stream << m_whiteList;

    file.flush();
    file.close();

    return true;
}

bool FieldTreeStorageItem::loadWhiteList()
{
    QFile file(m_path + WHITELIST_PATH);
    if (!file.open(QFile::ReadOnly))
        return false;

    QDataStream stream(&file);
    stream >> m_whiteList;

    file.close();

    return true;
}

bool FieldTreeStorageItem::saveFieldInfo() const
{
    QString infoPath = m_path + FIELDINFO_PATH;
    QFile infoFile(infoPath);
    if (!infoFile.open(QFile::WriteOnly))
        return false;

    int writeLength = infoFile.write((char*)&m_fieldInfo, sizeof(m_fieldInfo));
    infoFile.flush();
    infoFile.close();
    if (writeLength < sizeof(m_fieldInfo))
        return false;

    return true;
}

bool FieldTreeStorageItem::loadFieldInfo()
{
    QString infoPath = m_path + FIELDINFO_PATH;
    QFile infoFile(infoPath);
    if (!infoFile.open(QFile::ReadOnly))
        return false;

    int readLength = infoFile.read((char*)&m_fieldInfo, sizeof(m_fieldInfo));
    infoFile.close();
    if (readLength < sizeof(m_fieldInfo))
        return false;

    bool ok = m_fieldInfo.isValid();
    return ok;
}


/*********************************************************************************************************
** SyncFieldStorageItem
*********************************************************************************************************/

bool SyncFieldStorageItem::saveTree(const QString & filePath, const QString & fieldXPath)
{
    IField* field = IField::at(fieldXPath);
    if (filePath.isEmpty() || !field)
    {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly))
        return false;

    bool ok = this->write(&file, field);

    file.flush();
    file.close();

    return ok;
}

bool SyncFieldStorageItem::loadTree(const QString & filePath, const QString & fieldXPath)
{
    IField* field = IField::at(fieldXPath);
    if (filePath.isEmpty() || !field)
    {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return false;
    bool ok = this->read(&file, field);

    file.close();

    if (m_model)
    {
        emit m_model->dataChanged(QModelIndex(), QModelIndex());
    }

    return ok;
}

bool SyncFieldStorageItem::read(QFile* file, void* data /*= NULL*/)
{
    IField * field = reinterpret_cast<IField*>(data);
    if (field)
    {
        // 递归遍历所有子字段，子字段保存各自的内容
        QStack<IField*> stack;
        stack.push(field);
        while (!stack.empty())
        {
            field = stack.pop();

            if (field == NULL)
                continue;

            if (!field->isVirtual())
            {
                if (field->read(file, FieldDataType) < 0)
                    return false;
            }

            for (int i = 0; i < field->childCount(); ++i)
            {
                IField * child = field->child(i);
                stack.push(child);
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}



bool SyncFieldStorageItem::write(QFile* file, void* data /*= NULL*/)
{
    IField * field = reinterpret_cast<IField*>(data);
    if (field)
    {
        // 递归遍历所有子字段，子字段保存各自的内容
        QStack<IField*> stack;
        stack.push(field);
        while (!stack.empty())
        {
            field = stack.pop();

            if (field == NULL)
                continue;

            if (!field->isVirtual())
            {
                if (field->write(file, FieldDataType) < 0)
                    return false;
            }

            for (int i = 0; i < field->childCount(); ++i)
            {
                IField * child = field->child(i);
                stack.push(child);
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

/*********************************************************************************************************
** AsyncFieldStorageItem
*********************************************************************************************************/

float AsyncFieldStorageItem::m_fileCount = 0;
bool AsyncFieldStorageItem::reload(quint64 startTime /*= 0*/, quint64 endTime /*= 0*/, int type /*= 0*/)
{
    if (!FieldTreeStorageItem::reload(startTime, endTime, type))
    {
        qDebug() << "FieldTreeStorageItem reload Failed !!";
        return false;
    }

    Storage* storagePtr = Storage::instance();

    FieldProxyModel* theModel = qobject_cast<FieldProxyModel*>(m_model);
    if (theModel)
    {
        theModel->setWhiteList(m_whiteList);
    }

    // 首次加载
    if (startTime == 0 && endTime == 0)
    {
        foreach(QString xpath, m_whiteList)
        {
            IField* field = IField::at(xpath);
            if (field)
                field->resetBuffer(field);
        }

        storagePtr->setTotalPage(m_whiteList.size());
        storagePtr->setCurPage(0);

        Q_ASSERT_X(m_parentItem, "AsyncFieldStorageItem::reload", "m_parentItem is NULL, please check SessionStorageItem::initsubItems");

        SessionStorageItem * item = qobject_cast<SessionStorageItem *>(m_parentItem);
        if (!item)
        {
            qDebug() << "CurrentItem is not a SessionStorageItem";
            return false;
        }

        SessionInfo theInfo = item->sessionInfo();

        startTime = theInfo.StartTime;
        endTime = theInfo.EndTime;

        // 显示缩略图或者实时抽取图根据用户切换
        storagePtr->setStartTime(startTime);
        storagePtr->setEndTime(endTime);
    }

    // getStorageFilter
    StorageFilter filter = storagePtr->getStorageFilter();

    // 当前index在当前文件中的偏移帧数
    int fileOffset = 0;

    // 一个文件中存在的数据帧数(写的时候一个文件中保存了白名单所有field的一帧数据)
    int framCount = 1;
    if (filter.TotalPage == m_whiteList.size())
    {
        fileOffset = filter.CurPage;
        framCount = filter.TotalPage;
    }
    else
    {
        fileOffset = 0;
        framCount = filter.TotalPage;
    }

    StorageItemPosition startPos;
    StorageItemPosition endPos;
    startPos.m_pointCount = m_fieldInfo.FrameSize;
    endPos.m_pointCount = m_fieldInfo.FrameSize;
    if (this->toItemPosition(startTime, startPos, endTime, endPos))
    {
        for (StorageItemPosition pos = startPos; pos <= endPos; pos++)
        {
            pos.m_fileFrameCount = framCount;
            pos.m_frameIndex = fileOffset;
            if (pos == endPos)
            {
                pos.m_pointIndex = endPos.m_pointIndex;
                pos.m_pointCount = endPos.m_pointCount;
                pos.m_endFlag = endPos.m_endFlag;
            }
            asyncRead(&pos, NULL);
        }
        return true;
    }
    else
    {
        qDebug() << "AsyncFieldStorageItem toItemPosition Failed!! > Please Check startTime and endTime!";
        return false;
    }
}

void AsyncFieldStorageItem::setDataPtr(void* data) 
{
    m_filePosition.reset();
    m_curPosition.reset();
    m_flushDataFlag = m_dataTypeFlag | FlushBeforeClosing;
    return FieldTreeStorageItem::setDataPtr(data);
}

bool AsyncFieldStorageItem::autoSaveItemInfo()
{
    bool ok1 = saveWhiteList();

    bool ok2 = saveFieldInfo();

    bool ok3 = StorageItem::saveItemInfo(m_path, m_info);

    return ok1 && ok2 && ok3;
}

bool AsyncFieldStorageItem::flush()
{
    bool ok = saveWhiteList();

    // 发送异步写信号   
    StorageItemPosition pos = m_curPosition;
    asyncWrite(&pos, (void *)m_flushDataFlag);

    m_info.FileCount = pos.m_fileIndex + 1;
    m_fieldInfo.FrameCount = pos.m_fileIndex + 1;

    // 保存后需要与数据Model断开连接
    m_model->disconnect(this);

    if (m_whiteList.size() > 0)
    {
        setFieldInfo(IField::at(m_whiteList.at(0)));
    }
    bool ok2 = saveFieldInfo();

    return StorageItem::flush() && ok && ok2;
}

bool AsyncFieldStorageItem::read(QFile* file, void* data /*= NULL*/)
{
    int fileBytes = file->bytesAvailable();
    if (fileBytes == 0)
        return false;

    // getStorageFilter
    StorageFilter filter = Storage::instance()->getStorageFilter();

    bool ok = true;

    int dataTypeFlag = 0;

    // flush 数据
    if (m_filePosition.m_fileIndex == m_info.FileCount - 1)
    {
        dataTypeFlag = m_flushDataFlag;
    }
    else
    {
        dataTypeFlag = m_dataTypeFlag;
    }

    m_locker.lock();
    QStringList theWhiteList = m_whiteList;
    m_locker.unlock();

    Q_ASSERT_X(!theWhiteList.isEmpty(), "AsyncFieldStorageItem::read", "m_whiteList is Empty.");

    if (filter.TotalPage == m_whiteList.size())
    {

        if (!(filter.CurPage < theWhiteList.count()))
            return false;

        QString xPath = theWhiteList.at(filter.CurPage);

        IField * field = IField::at(xPath);
        // 不平衡模式unb%需要特殊处理
        if (field->isVirtual())
        {
            foreach(IField* childField, field->children())
            {
                if (!childField->read(file, dataTypeFlag, &m_filePosition))
                    ok = false;
            }
        }
        else
        {
            if (!field->read(file, dataTypeFlag, &m_filePosition))
                ok = false;
        }
    }
    else
    {
        foreach(QString xPath, m_whiteList)
        {
            IField * field = IField::at(xPath);
            // 不平衡模式unb%需要特殊处理
            if (field->isVirtual())
            {
                foreach(IField* childField, field->children())
                {
                    if (!childField->read(file, dataTypeFlag, &m_filePosition))
                        ok = false;
                }
            }
            else
            {
                if (!field->read(file, dataTypeFlag, &m_filePosition))
                    ok = false;
            }
        }
    }

    if (m_filePosition.m_endFlag)
    {
        setStatus(ItemLoaded);
    }

    return ok;
}

bool AsyncFieldStorageItem::write(QFile* file, void* data /*= NULL*/)
{
    // 获取异步信号读取的TypeFlags(可能是flush后的数据)
    int dataTypeFlag = reinterpret_cast<int>(data);

    bool ok = true;
    foreach(QString xPath, m_whiteList)
    {
        if (xPath.isEmpty())
            return false;
        IField* field = IField::at(xPath);
        // 不平衡模式unb%需要特殊处理
        if (field->isVirtual())
        {
            foreach(IField* childField, field->children())
            {
                if (!childField->write(file, dataTypeFlag))
                {
                    ok = false;
                }
            }
        }
        else
        {
            if (!field->write(file, dataTypeFlag))
            {
                ok = false;
            }
        }

    }

    m_locker.lock();
    m_curPosition = m_curPosition.nextFrame();
    m_positionChanged = true;
    m_locker.unlock();

    if (dataTypeFlag & FlushBeforeClosing)
    {
        setStatus(ItemSaved);

        qDebug() << "AsyncFieldStorageItem::write FlushBeforeClosing excuted!!";

        return ok;
    }
    if (file && file->isOpen() && m_parentItem)
    {
        this->autoSaveItemInfo();
        m_parentItem->autoSaveItemInfo();
    }

    return ok;
}

int AsyncFieldStorageItem::fileSizeInBytes() const
{
    quint64 defaultFileSize = (m_fieldInfo.PointSize * m_fieldInfo.FrameSize + sizeof(int)) * m_fieldInfo.ChannelCount;
    if (m_filePosition.m_endFlag)
    {
        if (m_filePosition.m_fileIndex == m_fieldInfo.FrameCount - 1)
        {
            // 最后一帧Flush的时候默认flush两帧
            defaultFileSize = 2 * defaultFileSize - sizeof(int) * m_fieldInfo.ChannelCount;
        }
    }
    return defaultFileSize;
}

bool AsyncFieldStorageItem::toItemPosition(quint64 startTime, StorageItemPosition & startPos, quint64 endTime, StorageItemPosition & endPos)
{
    int fileCount = m_info.FileCount;

    SessionStorageItem * item = qobject_cast<SessionStorageItem *>(m_parentItem);
    if (!item)
    {
        qDebug() << "CurrentItem is not a SessionStorageItem";
        return false;
    }
    SessionInfo theInfo = item->sessionInfo();

    quint64 infoStartTime = theInfo.StartTime;
    quint64 infoEndTime = theInfo.EndTime;

    // 计算每个DAT文件时间戳的时间间隔
    quint64 fileTime = (infoEndTime - infoStartTime) / fileCount;

    // 起始时间大于或等于结束时间，则转换没有意义，直接返回false
    if (endTime <= startTime)
        return false;

    // 合理处理起始和结束时间
    if (endTime > infoEndTime)
    {
        endTime = infoEndTime;
    }
    if (startTime < infoStartTime)
    {
        startTime = infoStartTime;
    }

    if (fileTime == 0)
    {
        startPos.m_fileIndex = 0;
        endPos.m_fileIndex = 0;
    }
    else
    {
        startPos.m_fileIndex = (startTime - infoStartTime) * fileCount / (infoEndTime - infoStartTime);
        startPos.m_pointIndex = startPos.m_pointCount * (startTime - infoStartTime) * fileCount / (infoEndTime - infoStartTime);
        startPos.m_endFlag = false;
        if (startPos.m_pointIndex > startPos.m_pointCount)
        {
            startPos.m_pointIndex = 0;
        }

        if (endTime == infoEndTime)
        {
            endPos.m_fileIndex = m_info.FileCount - 1;
            endPos.m_pointCount = m_fieldInfo.FrameSize * 2;
            endPos.m_pointIndex = endPos.m_pointCount;
        }
        else
        {
            endPos.m_fileIndex = (endTime - infoStartTime) * fileCount / (infoEndTime - infoStartTime);
            endPos.m_pointIndex = endPos.m_pointCount * (endTime - infoStartTime) * fileCount / (infoEndTime - infoStartTime);
        }
        endPos.m_endFlag = true;
    }

    // flush数据
    if (endPos.m_fileIndex == m_info.FileCount - 1)
    {
        endPos.m_pointCount = m_fieldInfo.FlushNumber;
        if (m_fieldInfo.FlushNumber < endPos.m_pointIndex)
        {
            endPos.m_pointIndex = m_fieldInfo.FlushNumber;
        }
    }
    float startOffset = float(startPos.m_pointIndex) / (float)(startPos.m_pointCount);
    float endOffset = float(endPos.m_pointIndex) / (float)(endPos.m_pointCount);
    m_fileCount = (endPos.m_fileIndex + endOffset) - (startPos.m_fileIndex + startOffset);

    if (m_fileCount < 1)
    {
        m_fileCount = 1;
        startPos = endPos;
    }


    // 根据时间计算的pos偏移超限处理
    if (startPos.m_pointIndex > startPos.m_pointCount)
    {
        startPos.m_pointIndex = startPos.m_pointCount;
    }
    if (endPos.m_pointIndex > endPos.m_pointCount)
    {
        endPos.m_pointIndex = endPos.m_pointCount;
    }


    if (startPos.m_fileIndex >= 0 && endPos.m_fileIndex >= 0)
        return true;
    else
        return false;
}

void AsyncFieldStorageItem::onModelDataChanged(const QModelIndex &topLeft,
    const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (m_whiteList.size() <= 0)
    return;

    if (Storage::instance()->currentMode() != Storage::RunningMode)
        return;

    IField* field = IField::at(m_whiteList.at(0));
    if (field->isVirtual())
        field = field->child(0);
    if (!field || !field->isWritable(m_dataTypeFlag))
        return;

    if (!m_positionChanged)
        return;
    m_locker.lock();
    StorageItemPosition pos = m_curPosition;
    m_positionChanged = false;
    m_locker.unlock();

    asyncWrite(&pos, &m_dataTypeFlag);
}

void AsyncFieldStorageItem::onStorageStatusChanged()
{
    if (m_model && m_status == ItemLoaded)
    {
        emit m_model->dataChanged(QModelIndex(), QModelIndex());
    }
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/

