/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Type.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2014/01/17
** Modified by:                
** Descriptions:               数学运算类型以及复数/向量运算支持
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2014/01/17
** Descriptions:               
** 
*********************************************************************************************************/
#include "math/Type.h"
#include "math/Math.h"

#include <float.h>

namespace math {

const FixedPointReal FixedPointReal::EPS = 0;                           //!< 分辨率
const FixedPointReal FixedPointReal::MAX = FLT_MAX;                     //!< 最大值
const FixedPointReal FixedPointReal::MIN = FLT_MIN;                     //!< 最小值

FixedPointReal::FixedPointReal(void)
{
}

FixedPointReal::~FixedPointReal(void)
{
}

/*! 
 * 复数模：     return=|this|
 * @return     运算结果
 */
Real Complex::Modulus(void) const
{
    return RealMath::Sqrt(SquareModulus());
}

/*! 
 * 复数辅角：   return=∠this
 * @return     运算结果
 */
Real Complex::Angle(void) const
{
    return RealMath::Atan2(r, i);
}

/*! 
 * 向量夹角：   return=∠(b->a)
 * @param[in]  a 以复数形式表示的二维向量a
 * @param[in]  b 以复数形式表示的二维向量b
 * @return     运算结果
 * @note       需要注意的是这里只是以复数代表一个二维向量，并求取两个向量间的夹角
 *             但复数是复数，向量是向量，两者的运算规则是不同的。
 */
Real Complex::Angle(const Complex& a, const Complex& b)
{
    /**
     *  @TODO 实现复数相间角计算
     */
    Real r = Complex::CrossProduct(a, b);
    Real m = RealMath::Sqrt(a.SquareModulus() * b.SquareModulus()) + RealMath::EPS;

    return RealMath::Acos(r / m);
}

/*! 
 * 向量叉乘：   return=a×b
 * @param[in]  a 以复数形式表示的二维向量a
 * @param[in]  b 以复数形式表示的二维向量b
 * @return     运算结果
 * @note       需要注意的是这里只是以复数代表一个二维向量，并求取两个向量间的外积
 *             但复数是复数，向量是向量，两者的运算规则是不同的。
 */
Real Complex::CrossProduct(const Complex& a, const Complex& b)
{
    return (a.r * b.r + a.i * b.i);
}

/*! 
 * 向量共轭：   return=a¤b
 * @param[in]  a 以复数形式表示的二维向量a
 * @param[in]  b 以复数形式表示的二维向量b
 * @return     运算结果
 * @note       需要注意的是这里只是以复数代表一个二维向量，并求取两个向量间的共轭
 *             但复数是复数，向量是向量，两者的运算规则是不同的。
 */
Real Complex::ConjProduct(const Complex& a, const Complex& b)
{
    return (a.r * b.i - a.i * b.r);
}

}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
