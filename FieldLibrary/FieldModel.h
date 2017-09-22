/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  FieldModel.h 
** Latest Version:             V1.0.0
** Latest modified Date:       2017/2/16
** Modified by:                ZhangWenBo
** Descriptions:               字段模型
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2015/07/09
** Descriptions:               Field模型
** 
*********************************************************************************************************/
#pragma once

#include "IField.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QMutex>


/**
 *  @class FieldModel
 *  @brief 字段模型
 */
class FIELDLIBRARY_EXPORT FieldModel : public QAbstractItemModel 
{
    Q_OBJECT
    Q_PROPERTY(QString rootPath  WRITE setRootPath  MEMBER m_rootPath)
    Q_PROPERTY(IField* rootField READ curRootField WRITE setRootField MEMBER m_rootField)



public:
    FieldModel(IField * rootField = NULL, const QString& rootPath = "");
    FieldModel(const FieldModel& other)
    {
        Q_UNUSED(other);
    }
    ~FieldModel();

    /**
    * 这里只需在静态数组models中遍历，逐一比较指定的路径是否包含当前Model的根路径。举例说明：
    * 输入path为：/packet/Voltage/Peak/A
    * 静态数组中存在4个已经创建的model，根路径分别为：
    * /config
    * /packet
    * /sys/status
    * /sys/config
    * 则注意比较以上四个哪一个是被输入path完整包含的，查找到即退出
    */
    /*! 
     * 根据路径名获取相应Model
     * @param[in]  wildPath 路径名
     * @return  成功则返回数据域类型model   
     */
    static FieldModel* getModel(QString wildPath);

    /*! 
     * 设置根字段路径
     * @param[in] path  根字段路径       
     */
    void setRootPath(const QString& path);

    /*! 
     * 设置根字段
     * @param[in] root  根字段对象      
     */
    void setRootField(IField* root);

    /*! 
     * 获取根字段
     * @return  根字段对象      
     */
    IField* curRootField()
    {
        return m_rootField;
    };

    /*! 
     *  更新模型各个字段所对应的原始数据（RawData=>FieldTree）
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  element   原始数据包对应的接线单元的序号，通常为0-7
     */
    void updateRawData(const void* basePtr, int element);

    /*! 
     *  提交模型各个字段所对应的原始数据（FieldTree=>RawData）
     * @param[in]  basePtr   原始数据包的基地址（字段对应的原始数据的基地址=数据包基地址+字段全局数据偏移量）
     * @param[in]  element   原始数据包对应的接线单元的序号，通常为0-7
     */
    void commitRawData(void* basePtr, int element) const;

    /*! 
     * 复位当前字段模型，仅需且必须在启动一次新的测量活动前调用
     */
    void reset();

    /*! 
     * 仅复位模型本身，从而导致各个绑定视图彻底重绘，不影响模型内部的字段树
     * @return     
     */
    void resetModel()
    {
        beginResetModel();
        endResetModel();
    }

    /*! 
     * 查询当前数据模型是否已经被修改
     */
    bool isDirty() const
    {
        return m_dirty;
    }

    /*! 
     * 清除当前数据模型“已经被修改”的状态
     */
    void clearDirtyStatus(void)
    {
        m_dirty = false;
    }

    /*! 
     * 置位当前数据模型“已经被修改”的状态
     */
    void setDirtyStatus(void)
    {
        m_dirty = true;
    }

    /*! 
     * 查询当前数据模型是否已经被修改
     */
    bool isLimitDirty() const
    {
        return m_limitDirty;
    }

    /*! 
     * 清除当前数据模型“已经被修改”的状态
     */
    void clearLimitDirtyStatus(void)
    {
        m_limitDirty = false;
    }

    /*! 
     * 置位当前数据模型“已经被修改”的状态
     */
    void setLimitDirtyStatus(void)
    {
        m_limitDirty = true;
    }

    /*! 
     * 重写Qt类库的部分虚函数（详见Qt帮助文档-QAbstractItemModel）：
     * [1] 获取数据flags
     * [2] 获取数据
     * [3] 设置数据
     * 
     * @note 这些函数的仅在内部调用对应的IField对象的同名函数，详见IField
     */
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    /*! 
     * 根据指定字段的XPath来生成QModelIndex，仅用于获取单个字段的值，无法用来跟View和Delegate交互
     * @param[in]  fieldPath 指定字段的XPath
     * @return     生成的QModelIndex
     */
    QModelIndex index(const QString& fieldPath) const;

    /*! 
     * 根据行号和列号来确定相应的index，本函数属于Qt库函数的重写，可参考Qt库
     *
     * @note 此处的rowCount和columnCount仅提供FieldModel的默认实现，部分表格型*Model子类可能根据需要重写:
     *       [1] Model的rowCount对应Field的childCount
     *       [2] Model的columnCount恒为1（除非表格类Model重写该函数）
     *       [3] Field的rowCount恒为0（除非表格类Field重写该函数）
     *       [4] Field的columnCount恒为0（除非表格类Field重写该函数）
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE; 
    int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent  = QModelIndex()) const Q_DECL_OVERRIDE;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;


    /*!
    * 将QString类型的role字段解析成对应的FieldRole
    * @param[in]  role 远程通信输入的string类型的role
    * @return     IField指针
    */
    static int parseTheRole(const QString & role);

signals:
    /*! 
     * 在PQA线程中发出此信号，表明原始数据已经被刷新
     */
    void rawDataUpdated();
protected slots:
    /*! 
     * 在主线程中使用此内部槽函数响应rawDataUpdated信号，并发出dataChanged信号以刷新各个视图
     */
    void notifyRawDataUpdated();

protected:
    /*! 
     * 尝试将当前模型对应的根节点挂载至当前模型对应的xpath处
     */
    void mountRootField();

    /*! 
     * 通过QModelIndex获取其对应的IField指针，如果IField不存在则返回model对应的rootField
     * @param[in]  index 输入的QModelIndex
     * @return     IField指针
     */
    IField* fieldFromIndex(const QModelIndex& index) const;

public slots:
    /**
    *  @brief 以下槽主要用于远程通信调用服务
    */

    /*! 
     * 字段树的写操作响应槽
     */
    bool writeFieldData(const QString &path, const QString& role, QVariant data);
    
    /*! 
     * 字段树的读操作响应槽
     */
    QVariant readFieldData(const QString &path, const QString& role);

private:

    static QStringList s_roleLists;                                      //!< 所有role的存放列表

protected:
    static QVector<FieldModel*> s_models;                               //!< 所有的model 存放列表
    IField*                     m_rootField;                            //!< model所在的根节点
    QString                     m_rootPath;                             //!< model所挂载的路径位置
    bool                        m_dirty;                                //!< 当前数据模型是否已经被修改（主要用于配置视图）
    bool                        m_limitDirty;                          //!< 限值设定是否已经修改
    mutable QMutex              m_lock;                                 //!< 跨线程操作FieldModel时使用的互斥锁
                                                                        //!< 例如一个调用reset，另一个调用updateRawData

    int                         m_updateCount;                          //!< 原始数据的更新次数，用于性能分析等
    quint64                     m_lastUiNotifyTime;                     //!< 上次刷新界面的时间
};

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
