/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SnapshotStorageItem.h
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
#ifndef SNAPSHOTSTORAGEITEM_H
#define SNAPSHOTSTORAGEITEM_H

#include "storagelibrary_global.h"
#include "StorageItem.h"
#include <QPixmap>

typedef struct DEVICE_INFO_S
{
    quint32     magic;                            //!< 校验字段
    quint32     equipSerial;                       //!< 序列号
    quint64     time;                              //!< 截图时间

    char        guiSoftwareVersion[128];            //!< gui版本号
    char        dspSoftwareVersion[128];            //!< dsp版本号
    char        fpgaSoftwareVersion[128];           //!< fpga版本号

    char        equipName[256];                    //!< 设备名称
}DEVICE_INFO;

/**
 *  @class SnapshotStorageItem
 *  @brief Snapshot存储类型
 */
class STORAGELIBRARY_EXPORT SnapshotStorageItem : public StorageItem
{
    Q_OBJECT

public:
    SnapshotStorageItem();
    ~SnapshotStorageItem();

    /*! 
     * 设置截图文件（仅用于保存截图时）
     * @param[in]  snapshot 刚截图完成的位图
     */
    void setSnapshot(const QPixmap& snapshot);

    /*! 
     * 获取截图文件（仅用于载入保存的截图时）
     * @return     截图文件
     */
    QPixmap snapshot() const;

    ItemNameProvider* getItemNameProvider() const Q_DECL_OVERRIDE
    {
        static ItemNameProvider nameProvider;
        return &nameProvider;
    }

protected:
    /*! 
     * 载入当前存储项，不同的存储项的载入方法不同
     * @return     操作成功返回true
     */
    bool read(QFile* file, void* data = NULL) override;

    /*! 
     * 保存当前存储项，不同的存储项的保存方法不同
     * @return     操作成功返回true
     */
    bool write(QFile* file, void* data = NULL) override;

    /*! 
     * 根据文件序号（存储项文件夹内的文件编号）生成对应的文件名，默认格式是nnnnn.DAT
     * @param[in]  fileIndex 文件序号（存储项文件夹内的文件编号）
     * @return     操作成功返回文件序号对应的文件名
     */
    QString fileNameFromIndex(int fileIndex) const override;

protected:
    QPixmap                m_snapshot;                    //!< 截图文件，若尚未载入则为NULL
};

#endif // SNAPSHOTSTORAGEITEM_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

