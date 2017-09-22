/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Averager.h
** Latest Version:             V1.0.0
** Latest modified Date:       2014/07/24
** Modified by:                
** Descriptions:               平均功能接口，包括线性平均和指数平均功能
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2014/07/24
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include "../Type.h"
#include "../Misc.h"
#include "math/Type.h"

namespace math {
/**
 *  @class IAverager
 *  @brief 平均器，用以实现对各类数据的平均计算功能
 */
class IAverager
{
protected:
    Real m_fDepth;                      //!< LIN窗口宽度/EXP衰减常数
    UINT m_nSize;                       //!< 参与平均的字段个数（用于大批量数据的快速平均运算）
    UINT m_nStep;                       //!< 参与平均的各个字段之间的步进值（单位：Real）

    UINT m_nUpdateCount;                //!< 已经更新的次数
	Real* m_pAver;						//!< 保存各个字段历史平均值的缓冲区

public:
    IAverager(Real depth, UINT size, UINT step) : 
        m_fDepth(depth),
        m_nSize(size),
        m_nStep(step)
    {
        Reset();
    }
    virtual ~IAverager(void);

	/**
	 *  @enum
	 *  @brief 平均功能选择
	 */
    enum
    {
        None = 0,                        //!< 禁用平均
        Lin,                             //!< 线性平均（移动平均）
        Exp,                             //!< 指数平均（平滑滤波）
		IEC								 //!< IEC专用平均（过小不更新）
    };
	/*! 
	 * 平均功能对象的创建
	 * @param[in]  type		平均功能实现类型（线性/指数）
	 * @param[in]  depth	LIN窗口宽度/EXP衰减常数
	 * @param[in]  size		参与平均的字段个数（用于大批量数据的快速平均运算）
	 * @param[in]  step		与平均的各个字段之间的步进值
	 * @return     创建的平均功能对象
	 */
    static IAverager* Create(UINT type, Real depth, UINT size = 1, UINT step = 1);

public:
    /*! 
     * 更新平均值，将输入的一个或多个Real型字段做平均处理后按原位置返回
     * @param[inout] pData  待平均的一个或多个字段，如果为多个字段，需保证内部结构与平均器的Step一致
     * @return       操作成功返回OK
     */
    virtual Result Update(Real* pData) = 0;

	/*! 
     * 历史平均值，将未进行本次操作的历史平均值返回
     * @param[out] pData       只作为输出，历史平均值
     * @return	   操作成功返回OK
     */
	virtual Result Get(Real* pData);

    /*! 
     * 复位平均器
     */
    virtual void   Reset(void);
	/*! 
	 * 平均功能单元测试函数
	 * @brief    1到15的线性平均，1到300的指数平均
	 */
	DECLARE_UNIT_TEST(IAverager)
};

/**
 *  @class LinAverager
 *  @brief 线性平均器，采用线性平均的方法
 */
class LinAverager : public IAverager
{
protected:
	Real* m_pData;							//!< 保存各个字段历史采样值的缓冲区

public:
    LinAverager(Real depth, UINT size, UINT step);
    virtual ~LinAverager(void);

	 /*! 
      * 更新平均值，将输入的一个或多个Real型字段做平均处理后按原位置返回
      * @param[inout] pData	待平均的一个或多个字段，如果为多个字段，需保证内部结构与平均器的Step一致
      * @return       操作成功返回OK
      */
    virtual Result Update(Real* pData);
	 /*! 
      *	@brief   复位线性平均器
      */
    virtual void   Reset(void);

};

/**
 *  @class ExpAverager
 *  @brief 指数平均器，采用指数平均的方法
 */
class ExpAverager : public IAverager
{
protected:
    Real  m_weight;                         //!< 本次测量值在最终平均值中的比重

public:
    ExpAverager(Real depth, UINT size, UINT step);
    virtual ~ExpAverager(void);
	 /*! 
      * 更新平均值，将输入的一个或多个Real型字段做平均处理后按原位置返回
      * @param[inout]   pData     待平均的一个或多个字段，如果为多个字段，需保证内部结构与平均器的Step一致
      * @return         操作成功返回OK
      */
    virtual Result Update(Real* pData);
};


/**
 *  @class LinAverager
 *  @brief IEC专用线性平均器，采用线性平均的方法，用于同步源、电压电流频率过小的时候不计入平均
 */
class IECAverager : public IAverager
{
protected:
	UINT* m_pUpdateCount;                //!< 专用于IEC同步源、电压、电流，设置各自的更新次数
	Real* m_pData;						 //!< 保存各个字段历史采样值的缓冲区

public:
    IECAverager(Real depth, UINT size, UINT step);
    virtual ~IECAverager(void);

	 /*! 
      * 更新平均值，将输入的一个或多个Real型字段做平均处理后按原位置返回
      * @param[inout] pData	待平均的一个或多个字段，如果为多个字段，需保证内部结构与平均器的Step一致
      * @return       操作成功返回OK
      */
    virtual Result Update(Real* pData);
	 /*! 
      *	@brief   复位线性平均器
      */
    virtual void   Reset(void);
	/*! 
     * 历史平均值，将未进行本次操作的历史平均值返回
     * @param[out] pData       只作为输出，历史平均值
     * @return	   操作成功返回OK
     */
	virtual Result Get(Real* pData);

};
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
