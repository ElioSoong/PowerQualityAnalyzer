/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SnapshotStorageItem.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2017/03/07
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/12/01
** Descriptions:               Snapshot存储类型
** 
*********************************************************************************************************/
#include "SnapshotStorageItem.h"
#include <QDebug>

SnapshotStorageItem::SnapshotStorageItem()
    : StorageItem()
    , m_snapshot()
{

}

SnapshotStorageItem::~SnapshotStorageItem()
{

}

/*! 
 * 载入当前存储项，不同的存储项的载入方法不同
 * @return     操作成功返回true
 */
bool SnapshotStorageItem::read(QFile* file, void* data)
{
    QPixmap snapPix;
    quint64 fileSize = file->bytesAvailable();
    const bool ok = snapPix.load(file->fileName(), "JPG");
    DEVICE_INFO deviceInfo;
    file->seek(fileSize - sizeof(deviceInfo));
    int readLength = file->read((char*)(&deviceInfo), sizeof(deviceInfo));
    if (ok)
    {
        setSnapshot(snapPix);
    }
    qDebug() << "Load snapshot: filename: " << file->fileName()
        << ", openmode: " << file->openMode()
        << ", exist: " << QFile::exists(file->fileName())
        << ", permissions: " << QFile::permissions(file->fileName())
        << ", size: 1." << (file->seek(0), file->bytesAvailable()) << ", 2." << file->size()
        << ", isSequential: " << file->isSequential()
        << ", loaded: " << ok;
    return ok;
}

/*! 
 * 保存当前存储项，不同的存储项的保存方法不同
 * @return     操作成功返回true
 */
bool SnapshotStorageItem::write(QFile* file, void* data)  
{
    QPixmap * curSnapeShot = reinterpret_cast<QPixmap *>(data);
    if (curSnapeShot) 
    {
        QPixmap snapPix = *curSnapeShot;
        bool saveOk = snapPix.save(file, "JPG");
        if (!saveOk)
        {
            qDebug() << "save pixmap failed!";
        }
        DEVICE_INFO deviceInfo;
        deviceInfo.magic = 0x56785678;
        deviceInfo.equipSerial = m_info.DeviceSerial;
        deviceInfo.time = m_info.CreateTime;
        memcpy(deviceInfo.equipName, m_info.DeviceName, sizeof(m_info.DeviceName));
        memcpy(deviceInfo.guiSoftwareVersion, m_info.GUIVersion, sizeof(m_info.GUIVersion));
        memcpy(deviceInfo.dspSoftwareVersion, m_info.DSPVersion, sizeof(m_info.DSPVersion));
        memcpy(deviceInfo.fpgaSoftwareVersion, m_info.FPGAVersion, sizeof(m_info.FPGAVersion));

        int writeLength = file->write((const char*)(&deviceInfo), sizeof(deviceInfo));
        if (writeLength < sizeof(deviceInfo))
        {
            qDebug() << "SnapshotStorageItem::Write append DEVICE_INFO failed!";
            return false;
        }
        return true;
    }
    return false;
}

/*! 
 * 根据文件序号（存储项文件夹内的文件编号）生成对应的文件名，默认格式是nnnnn.DAT
 * @param[in]  fileIndex 文件序号（存储项文件夹内的文件编号）
 * @return     操作成功返回文件序号对应的文件名
 */
QString SnapshotStorageItem::fileNameFromIndex(int fileIndex) const
{
    return QString("screen.jpg");
}

/*! 
 * 设置截图文件（仅用于保存截图时）
 * @param[in]  snapshot 刚截图完成的位图
 */
void SnapshotStorageItem::setSnapshot(const QPixmap& snapshot)
{
    m_snapshot = snapshot;
}

/*! 
 * 获取截图文件（仅用于载入保存的截图时）
 * @return     截图文件
 */
QPixmap SnapshotStorageItem::snapshot() const
{
    return m_snapshot;
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
