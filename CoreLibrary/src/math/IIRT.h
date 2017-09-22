/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  IIRT.h
** Latest Version:             V1.0.0
** Latest modified Date:       2014/04/24
** Modified by:                
** Descriptions:               IIR滤波器模板函数
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2014/04/24
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once


#include "math/Type.h"

namespace math
{

/*! 
 * 一阶IIR滤波器内核：
 * Y[n] = X[n]*b[0] + X[n-1]*b[1] - Y[n-1]*a[1]
 * @param[inout] xy     输入信号以及输出信号
 * @param[inout] z      初始反馈系数以及更新后的反馈系数
 * @param[in]    b      滤波器z域分子
 * @param[in]    a      滤波器z域分母
 * @param[in]    count  输入信号长度，单位：点
 * @return       无返回
 */
template<class FilterT>
void IIR1(
    const FilterT b[2],
    const FilterT a[2],
    FilterT       z[1],
    Real          xy[],
    int           count
    )
{
    int     i       = 0;
    FilterT y       = 0;
    FilterT ybuf[1] = {0};
    FilterT zbuf[1] = {0};

    if (z == NULL) z = zbuf;

    ybuf[0] = xy[0] * b[0] + z[0];

    for (i = 1; i < count; i++)
    {
        y         = xy[i] * b[0] + xy[i - 1] * b[1] - ybuf[0] * a[1];
        xy[i - 1] = (Real)ybuf[0];
        ybuf[0]   = y;
    }

    z[0] = xy[count - 1] * b[1] - ybuf[0] * a[1];
    xy[count - 1] = (Real)ybuf[0];
}

/*! 
 * 二阶IIR滤波器内核：
 * Y[n] = X[n]*b[0] + X[n-1]*b[1] + X[n-2]*b[2] - Y[n-1]*a[1] - Y[n-2]*a[2]
 * @param[inout] xy     输入信号以及输出信号
 * @param[inout] z      初始反馈系数以及更新后的反馈系数
 * @param[in]    b      滤波器z域分子
 * @param[in]    a      滤波器z域分母
 * @param[in]    count  输入信号长度，单位：点
 * @return       无返回
 */
template<class FilterT>
void IIR2(
    const FilterT b[3],
    const FilterT a[3],
    FilterT       z[2],
    Real          xy[],
    int           count
    )
{
    int     i       = 0;
    FilterT y       = 0;
    FilterT ybuf[2] = {0, 0};
    FilterT zbuf[2] = {0, 0};

    if (z == NULL) z = zbuf;

    ybuf[1] = b[0] * xy[0]  
            + z[0];
    ybuf[0] = b[0] * xy[1]  
            + b[1] * xy[0]  
            - a[1] * ybuf[1]  
            + z[1];

    for (i = 2; i < count; i++)
    {
        y = b[0] * xy[i]
          + b[1] * xy[i - 1]
          - a[1] * ybuf[0]
          + b[2] * xy[i - 2]
          - a[2] * ybuf[1];

        xy[i - 2] = (Real)ybuf[1];
        ybuf[1]   = ybuf[0];
        ybuf[0]   = y;
    }

    z[1] = b[2] * xy[count - 1] 
         - a[2] * ybuf[0];
    z[0] = b[1] * xy[count - 1] 
         + b[2] * xy[count - 2]
         - a[1] * ybuf[0]       
         - a[2] * ybuf[1];

    xy[count - 1] = (Real)ybuf[0];
    xy[count - 2] = (Real)ybuf[1];

    return;
}

/*! 
 * 任意阶IIR滤波器
 * @param[inout] xy     输入信号以及输出信号
 * @param[inout] z      各级二阶滤波器内核的初始反馈系数以及更新后的反馈系数
 * @param[in]    ba     各级二阶滤波器内核的滤波器z域分子以及分母
 * @param[in]    count  输入信号长度，单位：点
 * @param[in]    order  滤波器阶数
 * @return       操作成功返回math::OK
 * @note         需要注意的是，考虑到收敛性问题，这里的高阶滤波器必须拆解为多个二阶滤波器的级联。
 *               当滤波器的阶数为奇数时，可以将最后一阶滤波器通过填充0系数的方式扩充为二阶滤波器。
 */
template<class FilterT>
Result IIRFilter(
    const FilterT ba[][6],
    int           order,
    F32           xy[],
    int           count,
    FilterT       z[][2]
    )
{
    int i, j;

    if (ba == NULL || xy == NULL || count < order) return math::Failed;

    for (i = 0, j = 0; i < order - 1; i += 2, j++)
    {
        IIR2(&ba[j][0], &ba[j][3], &z[j][0], &xy[0], count);
    }
    if (i < order)
    {
        IIR1(&ba[i][0], &ba[i][3], &z[i][0], &xy[0], count);
    }

    return math::OK;    
}

/**
 *  @def   IIR_SP
 *  @brief 单精度浮点（Single-point）类型的IIR滤波器
 */
#define IIR_SP              IIRFilter<float>

/**
 *  @def   IIR_DP
 *  @brief 双精度浮点（Double-point）类型的IIR滤波器
 */
#define IIR_DP              IIRFilter<double>

}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
