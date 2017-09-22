/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  TrendStorageItem.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2015/12/01
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/12/01
** Descriptions:               Trend存储类型
** 
*********************************************************************************************************/
#include "TrendStorageItem.h"
#include "FieldLibrary/IField.h"

TrendStorageItem::TrendStorageItem()
    : AsyncFieldStorageItem()
{
    this->m_typeName = "TREND";
    this->m_info.ItemType = 0x00000008;
    m_dataTypeFlag = TrendDataType;
    m_isAsyncronuous = true;
}

TrendStorageItem::~TrendStorageItem()
{

}


bool TrendStorageItem::autoSaveItemInfo()
{
    // 保存缩略图
    bool ok = saveThumbnail();

    return ok && AsyncFieldStorageItem::autoSaveItemInfo();
}

/*!
 * 随时强制保存未保存的文件（尤其是StorageItemInfo和SessionItemInfo），避免意外掉电彻底损坏数据
 * @return     操作成功返回true
 */
bool TrendStorageItem::flush()
{
    // 保存缩略图
    bool ok = saveThumbnail();

    return AsyncFieldStorageItem::flush();
}

/*! 
* 保存趋势图缩略图文件
* @return     操作成功返回true
*/
bool TrendStorageItem::saveThumbnail() const
{
    QFile file(m_path + "/TUMBNAIL.DAT");
    if (!file.open(QFile::WriteOnly))
        return false;

    bool ok = true;
    foreach (QString xPath, m_whiteList)
    {
        if (xPath.isEmpty())
            return false;
        IField* field = IField::at(xPath);

        // 不平衡模式unb%需要特殊处理
        if (field->isVirtual())
        {
            foreach(IField* childField, field->children())
            {
                if (!childField->write(&file, ThumbnailDataType))
                {
                    ok = false;
                }
            }
        }
        else 
        {
            if (!field->write(&file, ThumbnailDataType))
            {
                ok = false;
            }
        }  
    }

    file.flush();
    file.close();
    return ok;
}

/*! 
* 从当前存储项的文件夹下载入趋势信息，缩略图信息
* @return     操作成功返回true
*/
bool TrendStorageItem::loadThumbnail()
{
    QFile file(m_path + "/TUMBNAIL.DAT");
    if (!file.open(QFile::ReadOnly))
        return false;

    bool ok = true;
    foreach (QString xPath, m_whiteList)
    {
        if (xPath.isEmpty())
            return false;
        IField* field = IField::at(xPath);
        // 不平衡模式unb%需要特殊处理
        if (field->isVirtual())
        {
            foreach(IField* childField, field->children())
            {
                if (!childField->read(&file, ThumbnailDataType))
                {
                    ok = false;
                }
            }
        }
        else
        {
            if (!field->read(&file, ThumbnailDataType))
            {
                ok = false;
            }
        }
    }

    file.close();
    return ok;
}

bool TrendStorageItem::reload(quint64 startTime /*= 0*/, quint64 endTime /*= 0*/, int type /*= 0*/)
{
    if (startTime == 0 && endTime == 0)
    {
        AsyncFieldStorageItem::reload(startTime, endTime, type);
        loadThumbnail();
    }
    return AsyncFieldStorageItem::reload(startTime, endTime, type);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
