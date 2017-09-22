/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Math.h
** Latest Version:             V1.0.0
** Latest modified Date:       2014/01/17
** Modified by:                
** Descriptions:               统一数学公式接口，包括基本运算、IIR滤波器、CRC校验、最大公约数、象限坐标等
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2014/01/17
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

namespace math {
/**
 *  @class RealMath
 *  @brief 实数的基本数学运算
 *  @note  当算法在不同平台间移植时，请根据平台特性使用适当的方法替换默认数学运算。
 */
class RealMath
{
public:
    /**
     *  @brief  实数的基本数学运算，考虑到函数名称与数学运算名称一一对应，这里不再独立注释。
     *	       （绝对值、取倒数、余弦、正弦、反余弦、反正切、开平方、取对数、自然对数、幂次方、取整、取符号）
     */
    static Real Abs  (const Real& a);
    static Real Inv  (const Real& a);
    static Real Cos  (const Real& a);
    static Real Sin  (const Real& a);
    static Real Acos (const Real& a);
    static Real Atan2(const Real& r, const Real& i);
    static Real Sqrt (const Real& a);
    static Real Log  (const Real& a);
    static Real Log10(const Real& a);
    static Real Pow  (const Real& m, const Real& e);
    static int  Round(const Real& a);
    static int  Sign (const Real& a);

    static const Real EPS;					                            //!< 最小的正值 1.0+FLT_EPSILON != 1.0
    static const Real MAX;					                            //!< 很大数 
    static const Real MIN;					                            //!< 很小的负值
};
/**
 * @class MathUtils
 * @brief IIR滤波器、CRC校验、最大公约数、象限坐标等数学公式
 */
class MathUtils
{
public:
    static const unsigned char s_CRC8Tab [256];                         //!< CRC8系数表
    static const unsigned int  s_CRC32Tab[256];                         //!< CRC32系数表

public:
	/*! 
	 * 求取最大公约数
	 * @param[in] a  整数a
	 * @param[in] b  整数b 
	 * @return    a和b的最大公约数
	 */
    static Integer GCD(Integer a, Integer b);

	/*! 
	 * 根据幅值和向量（原点为起始点）与Y轴的夹角，计算向量终点的X轴横坐标
	 * @param[in] length  幅值
	 * @param[in] angle   与Y轴的夹角
	 * @return    向量终点的X轴坐标
     * @note      本方法仅供矢量图模式调用
	 */
    static Float  Coord_X(Float length, Float angle);

	/*! 
	 * 根据幅值和向量（原点为起始点）与Y轴的夹角，计算向量终点的Y轴纵坐标
	 * @param[in] length  幅值
	 * @param[in] angle   与Y轴的夹角
	 * @return    向量终点的Y轴纵坐标
     * @note      本方法仅供矢量图模式调用
	 */
    static Float  Coord_Y(Float length, Float angle);

	/*!
	 * 根据采样频率、截至频率、滤波阶数设计IIR滤波器
	 * @param[inout]    pCoefBuf     调用者提供的输出系数缓冲区
	 * @param[in]       nCoefCount   调用者提供的输出系数缓冲区的长度
	 * @param[in]	    fs			 采样率，单位：Hz
	 * @param[in]	    fc 			 截止频率，单位：Hz
	 * @param[in]	    nOrder       IIR滤波器的阶数
	 * @return          设计生成的滤波器系数
	 * @note            pCoefBuf[0]:滤波器总增益系数; 
     *                  pCoefBuf[1—3]:第i级滤波器分子系数；
     *                  pCoefBuf[4—6]:第i级滤波器分母系数。
     *                  生成的滤波器的一般数学表达式为：
	 *					pCoefBuf[6k+1] + pCoefBuf[6k+2]*z^-1 + pCoefBuf[6k+3]*z^-2 
	 * pCoefBuf[0]  *   -------------------------------------------------------------    //(k为滤波器级数，从0开始)
	 *					pCoefBuf[6k+4] + pCoefBuf[6k+5]*z^-1 + pCoefBuf[6k+6]*z^-2
	 */
	static bool DesignIIRFilter(
		double* pCoefBuf,
		int nCoefCount,
		double fs,
		double fc,
		int nOrder);

	/*! 
	 * CRC8校验值计算—查表法
	 * @param[in] pData		待校验的数据
	 * @param[in] length	待校验的数据长度
	 * @param[in] initial	CRC8初始值，用于多次级联调用
	 * @return    返回CRC8校验数据 
	 */
    static unsigned char CRC8(const void* pData, int length, unsigned char initial = 0);

    /*! 
     * CRC32校验值计算—查表法
     * @param[in] pData		待校验的数据
     * @param[in] length	待校验的数据长度
     * @param[in] initial	CRC32字节初始化
     * @return    返回CRC32校验数据 
     */
    static unsigned int CRC32(const void* pData, int length, unsigned int initial = 0);

	/*! 
	 * 角度格式化，使转化后的角度大小在0-360或者-180——180范围内
	 * @param[in] a					需要格式化的角度数据
	 * @param[in] b_0_360_type		置1则转换为0-360度，否则转换为0-180度
	 * @return   返回格式化后的角度数据
	 */
    static Real FormatAngle(Real a, Integer b_0_360_type = 0);
};

}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

