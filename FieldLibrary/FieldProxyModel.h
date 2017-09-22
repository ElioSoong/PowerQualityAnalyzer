/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldProxyModel.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Han ZhenFei
** Created date:               2015/10/15
** Descriptions:               标准化的proxyModel, 提供了白名单过滤，正则表达式过滤两种方式
** 
*********************************************************************************************************/
#pragma once

#include <QObject>
#include <QAbstractProxyModel>
#include <QRegExp>
#include <QStringList>
#include <QModelIndex>
#include <QHash>
#include "FieldModel.h"
/**
*  @class 
*  @brief Filed的代理模型，用于管理过滤DataField
*/
class FIELDLIBRARY_EXPORT FieldProxyModel : public QAbstractProxyModel
{
       Q_OBJECT
       Q_PROPERTY(QRegExp filterRegExp READ filterRegExp WRITE setFilterRegExp)
       Q_PROPERTY(QStringList whiteList READ whiteList WRITE setWhiteList NOTIFY whiteListChanged);                 //!<  白名单设置
       Q_PROPERTY(QString rootName READ rootName WRITE setRootName);                       //!<  根节点设置
       Q_PROPERTY(QString filterReg WRITE setReg READ filterRegValue);                         //!<  正则表达式设置

public:
     FieldProxyModel(QObject *parent = 0);
     ~FieldProxyModel();
     FieldProxyModel(const FieldProxyModel& other);

     void setSourceModel(QAbstractItemModel * sourceModel) Q_DECL_OVERRIDE
     {
         QAbstractProxyModel::setSourceModel(sourceModel);

         if (sourceModel)
         {
             connect(sourceModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                 this, SLOT(sourceDataChanged(QModelIndex, QModelIndex, QVector<int>)));
             FieldModel *model = (FieldModel *)(sourceModel);
             m_rootIndex = model->index(m_Xpath);
         }
         resetProxyModel();
     }

     QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

     /*! 
      * @brief 返回过滤用的正则表达式
      */
     QRegExp filterRegExp() const;

     /*! 
      * @brief 设置正则表达式
      */
     void setFilterRegExp(const QRegExp &regExp);

     /*! 
      * @brief 设置正则表达式模式
      */
     void setReg(const QString& pattern );

     /*
      * @brief 获取根节点名称
      */
     QString rootName() const
     {
         return m_rootName;
     }

      /*! 
      * @brief 设置根节点名称，不一定是绝对路径，可以给相对路径，或者直接给名称
      */
     void setRootName(const QString& rootName);

     /*! 
      * @brief 重新复位model，清除Mapping缓存
      */
     void resetProxyModel();

      /*! 
      * @brief 获取白名单
      */
     const QStringList &whiteList() const;

      /*! 
      * @brief 设置白名单
      */
     void setWhiteList(const QStringList& list);
     
     /*! 
      * @brief 返回过滤用的正则表达式模式
      */
     QString filterRegValue() const;

     /*! 
      * @brief 参考QT帮助文档
      */
     QModelIndex mapFromSource(const QModelIndex & sourceIndex) const Q_DECL_OVERRIDE;

       /*! 
      * @brief 参考QT帮助文档
      */
     QModelIndex mapToSource(const QModelIndex & proxyIndex) const Q_DECL_OVERRIDE;

       /*! 
      * @brief 参考QT帮助文档
      */
     QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
     
       /*! 
      * @brief 参考QT帮助文档
      */
     int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

      /*! 
      * @brief 参考QT帮助文档
      */
     int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

       /*! 
      * @brief 参考QT帮助文档
      */
     QModelIndex parent(const QModelIndex& proxIndex) const Q_DECL_OVERRIDE;

protected:
    // 映射结构体
    struct Mapping 
    {
        int  srcRow;            // 保存源的行
        int  srcColum;            // 保存源的列
        int  proxyRow;          // 新Index的行
        int  proxyColum;
        QHash<QModelIndex, Mapping*>::const_iterator map_iter;      
    };
 
protected slots:
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
    {
        QAbstractProxyModel::dataChanged(topLeft, bottomRight, roles);
        emit dataChanged(index(0, 0), index(rowCount(QModelIndex()) - 1, columnCount(QModelIndex()) - 1));
    }

private:
     inline QModelIndex srcToProxy(int row, int column, const QModelIndex& srcIndex) const;
     inline QModelIndex getRootIndex(int row, int column) const;
     QVector<int> filterSrcRow(const QModelIndex &srcParent) const;
     inline bool  filterRegExpRow (const QRegExp& regExp, const QModelIndex& index) const;
     inline QModelIndex createIndexMapping(int row, int column, QModelIndex srcParent) const;
     bool isLastParent(const QModelIndex &srcParent) const;

signals:
     void whiteListChanged();

protected:
    QString m_Xpath;                                       //!< rootName 所对应的m_Xpath, model会将此XPath剥开
    QString m_rootName;                                    //!< 根节点名称
    QRegExp m_reg;                                         //!< 用于筛选的正则表达式
    QString m_regExpValue;                                 //!< 正则表达式的值
    QStringList m_whiteList;                               //!< 白名单列表
    bool m_regFlag;                                        //!< 正则表达模型不为空，此值为真

    mutable QVector<QModelIndex> m_rootList;               //!< 根节点列表 
    mutable QModelIndex m_rootIndex;                         //!< 根节点index
    mutable QHash<QModelIndex, Mapping*> source_index_mapping;   
    typedef QHash<QModelIndex, FieldProxyModel::Mapping*> IndexMap;
}; 















/*********************************************************************************************************
** End of file
*********************************************************************************************************/