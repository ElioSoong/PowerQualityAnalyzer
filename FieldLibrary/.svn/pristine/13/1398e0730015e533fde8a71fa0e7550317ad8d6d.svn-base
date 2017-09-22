/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FileField.h
** Latest Version:             V1.0.0
** Latest modified Date:       2016/03/25
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang Wenbo
** Created date:               2016/03/25
** Descriptions:            设备文件对应Field字段(此处用于电池电量相关)   
** 
*********************************************************************************************************/
#pragma once

#include "fieldlibrary_global.h"
#include "IField.h"
#include <QFile>
#include <QDebug>

/**
 *  @class FileField
 *  @brief 用于访问设备文件的对应Field字段(此处用于电池电量相关)
 */
class FIELDLIBRARY_EXPORT FileField :public IField
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)

public:
    FileField();
    ~FileField();

    /*! 
     *  返回设备文件路径   
     */
    QString filePath() const
    {
        return m_filePath;
    }

    /*! 
     *  设置设备文件路径   
     */
    void setFilePath(const QString & path)
    {
        m_filePath = path;
    }

    /*! 
     * 将字段对应的数据类型QByteArray的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  QByteArray    转换输入的数据
     * @return     转换成功返回转换后的String，失败返回空
     */
    virtual QString toString(QByteArray& dataIn) const
    {
        QString stringOut;
        return stringOut;
    }

protected:
    QString m_filePath;             //!< 设备文件对应的路径
};

class FIELDLIBRARY_EXPORT TextFileField : public FileField
{
    Q_OBJECT
public:
    TextFileField(){};

    /*! 
     * 获取字段的数据，通常仅被FieldModel的同名函数遍历调用
     * @param[in]  与FieldModel的形参含义相同，详见FieldModel
     *
     * @note       注意IField以及其子类的flags|data|setData三个方法中的row和column参数不是给FieldModel使用的，
     *             而是给其子Field（通常是SubDataField）使用的。当FieldModel调用这三个方法时，需要传入-1。
     */
    virtual QVariant data(int element = 0) const   Q_DECL_OVERRIDE
    {
        QFile file(m_filePath);
        //qDebug() << "filePath:" << m_filePath;
        if (!file.open(QIODevice::ReadOnly))
        {
            //qDebug() << "open false";
            return QVariant();
        }
        QByteArray data = file.readAll();
        //qDebug() << "batteryData:" << data;
        file.close();
        QString stringOut = this->toString(data);
        return stringOut;
    }


    /*! 
     * 将字段对应的数据类型QByteArray的数据转换成字符串，本应为静态函数，考虑到不同子类实现不同，此处设置为虚函数
     * @param[in]  QByteArray    转换输入的数据
     * @return     转换成功返回转换后的String，失败返回空
     */
    QString toString(QByteArray& dataIn) const  Q_DECL_OVERRIDE
    {
        QString stringOut;
        stringOut = dataIn.data();
        return stringOut;
    }
};



/*********************************************************************************************************
** End of file
*********************************************************************************************************/
