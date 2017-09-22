/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  includes.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/06/29
** Modified by:                
** Descriptions:               全局包含文件，为了保持一致性，每个cpp文件必须包含此文件，即使不需要此文件中的内容
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/06/29
** Descriptions:               
** 
*********************************************************************************************************/
#ifndef __INCLUDES_H
#define __INCLUDES_H
#include <QObject>

/*
 * @brief 包含公共头文件
 */
#define TEST_DRIVE 0
#if defined(_DEBUG) || 1
#include <QDebug>
#endif

/**
 *  @brief 此处用于处理VS2010编译器的utf-8编码问题，
 *         必须保证每一个包含中文字符的源码文件均使用此pragma
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1600)  
# pragma execution_character_set("utf-8")  
#endif


/**
 *  @brief 找到传入的字符串对应翻译且返回，若没找到则返回的是原字符串
 */
#include <QApplication>
static QString myTr(const QString str)
{
    std::string tempStr = str.toStdString();
    const char* ch = tempStr.c_str();
    return qApp->translate("myTsFile", ch);
}


/**
 *  @brief 解决指针转换问题，规则：所有存储方式的指针，全部以QObject* 方式存储,以T类型返回
 */
#include <QObject>
template <class T>
inline T custom_cast(void *pointer)
{
    QObject * obj = reinterpret_cast<QObject*>(pointer); 
    return dynamic_cast<T>(obj);
}

/**
 *  @macro Q_ENABLE_COPY
 *  @brief 表示一个QObject的子类可以被拷贝（对应Q_DISABLE_COPY系统宏），仅用于支持元对象系统的对象动态创建
 *  @note  本宏仅需要在基类的头文件中使用一次，推荐使用在Q_OBJECT宏的下方一行，或者其他类似的显著位置
 */
#ifndef Q_ENABLE_COPY
#define Q_ENABLE_COPY(_type)                                                                            \
    public: _type(const _type& other) {                                                                 \
        Q_ASSERT_X(false, #_type" Meta Copy-Constructor", #_type" Meta-Objects can NOT be copied.");    \
    } 
#endif

#endif                                                                  /* __INCLUDES_H                 */
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
