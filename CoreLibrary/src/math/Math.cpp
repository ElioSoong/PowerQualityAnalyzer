/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Math.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2014/12/12
** Modified by:                统一数学公式接口，包括基本运算、IIR滤波器、CRC校验、最大公约数、象限坐标等
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2014/12/12
** Descriptions:               
** 
*********************************************************************************************************/
#include "../Type.h"
#include "math/Type.h"
#include "math/Math.h"
#include "math/tspl/include/iir.h"

#include <float.h>
#include <math.h>
#include <stdio.h>

#if F260_TARGET_TI_DSP
#include <c6x.h>
#endif

using namespace splab;

namespace math {

#if MATH_REAL_TYPE == RealIsFloat
const Real RealMath::EPS = FLT_EPSILON;                                 //!< 分辨率
const Real RealMath::MAX = FLT_MAX;                                     //!< 最大值
const Real RealMath::MIN = FLT_MIN;                                     //!< 最小值
#elif MATH_REAL_TYPE == RealIsDouble
const Real RealMath::EPS = DBL_EPSILON;                                 //!< 分辨率
const Real RealMath::MAX = DBL_MAX;                                     //!< 最大值
const Real RealMath::MIN = DBL_MIN;                                     //!< 最小值
#elif MATH_REAL_TYPE == RealIsFixedPoint
const Real RealMath::EPS = FixedPointReal::EPS;                         //!< 分辨率
const Real RealMath::MAX = FixedPointReal::MAX;                         //!< 最大值
const Real RealMath::MIN = FixedPointReal::MIN;                         //!< 最小值
#else
#error Invalid MATH_REAL_TYPE macro definition. 
#endif


#if MATH_REAL_TYPE == RealIsFloat || MATH_REAL_TYPE == RealIsDouble

/**
 *  @brief 直接使用对应精度的数学库实现数据运算接口，支持带f和不带f后缀的两个版本
 */
#if MATH_REAL_TYPE == RealIsFloat
#define MATH_FUNC_CAT(_name)                _name ## f
#else
#define MATH_FUNC_CAT(_name)                _name
#endif

#define MATH_FUNC(_name)                    MATH_FUNC_CAT(_name)

/**
 *  @brief CRC8全局系数表
 */
const unsigned char MathUtils::s_CRC8Tab[256] = 
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, 
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a, 
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, 
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35 
};

/**
 *  @brief CRC32全局系数表
 */
const unsigned int MathUtils::s_CRC32Tab[256] = 
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3, 
    0x0EDB8832, 0x79DCB8A4,	0xE0D5E91E, 0x97D2D988, 
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,	
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 
    0x14015C4F, 0x63066CD9,	0xFA0F3D63, 0x8D080DF5, 
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,	
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 
    0x26D930AC, 0x51DE003A,	0xC8D75180, 0xBFD06116, 
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F, 
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,	
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433, 
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD, 
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B, 
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79, 
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D, 
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45, 
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF, 
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D, 
};

/**
 *  @brief  实数的基本数学运算，考虑到函数名称与数学运算名称一一对应，这里不再独立注释。
 *	       （绝对值、取倒数、余弦、正弦、反余弦、反正切、开平方、取对数、自然对数、幂次方、取整、取符号）
 */
Real RealMath::Abs  (const Real& a)
{
    return MATH_FUNC(::fabs)(a);
}

Real RealMath::Inv  (const Real& a)
{
#if F260_TARGET_TI_DSP && MATH_REAL_TYPE == RealIsFloat
    float r;

    /**
     *  @brief 目前只有Motor::Update调用此方法，
     *         由于其仅用于波形显示，因此所需精度不高，可考虑有限精度的快速浮点算法
     */
    r = _rcpsp(a);          // mantissa error is less than 2^-8
    r = r * (2.0f - a * r); // mantissa error is less than 2^-16
    //r = r * (2.0f - v * r); // mantissa accuracy is the full 23 bits

    return r;
#else
    return MATH_FUNC(1.0) / a;
#endif
}

Real RealMath::Cos  (const Real& a)
{
    return MATH_FUNC(::cos)(a);
}

Real RealMath::Sin  (const Real& a)
{
    return MATH_FUNC(::sin)(a);
}

Real RealMath::Acos (const Real& a)
{
    return MATH_FUNC(::acos)(a);
}

Real RealMath::Atan2(const Real& r, const Real& i)
{
    return MATH_FUNC(::atan2)(i/*Y*/, r/*X*/);
}

Real RealMath::Sqrt (const Real& a)
{
    return MATH_FUNC(::sqrt)(a);
}

Real RealMath::Log  (const Real& a)
{
    return MATH_FUNC(::log)(a);
}

Real RealMath::Log10(const Real& a)
{
    return MATH_FUNC(::log10)(a);
}

Real RealMath::Pow(const Real& m, const Real& e)
{
    return MATH_FUNC(::pow)(m, e);
}

int  RealMath::Round(const Real& a)
{
    return (int)(a + (Real)0.5f);
}

int  RealMath::Sign (const Real& a)
{
    return a < 0 ? -1 : +1;
}

#else
#error FixedPointReal is not supported in current version.
/**
 *  @TODO 根据FixedPointReal类型实现数学函数库
 */
#endif

/*! 
 * 求取最大公约数
 * @param[in] a   整数a
 * @param[in] b   整数b 
 * @return    a和b的最大公约数
 */
Integer MathUtils::GCD(Integer a, Integer b)
{
    Integer  temp; 

    if (a < b) 
    { 
        temp = a;
        a    = b; 
        b    = temp;
    } 
    else if (a == b) 
    {
        return a;
    }

    while (b) 
    { 
        temp = a % b; 
        a = b;
        b = temp; 
    } 

    return (a);
}

/*! 
 * 根据幅值和向量（原点为起始点）与Y轴的夹角，计算向量终点的X轴横坐标
 * @param[in] length  幅值
 * @param[in] angle   与Y轴的夹角， 
 * @return    向量终点的X轴坐标
 * @note      本方法仅供矢量图模式调用
 */
Float MathUtils::Coord_X(Float length, Float angle)
{
	Float fCoord = 0.0;

	if(((angle - 90.0) < FLT_EPSILON) && (angle > -FLT_EPSILON))
	{
		return fCoord =  length * cosf((90 - angle) / MATH_180_D_PI);
	}
	else if(((angle - 180) < FLT_EPSILON) && ((angle - 90.0) > -FLT_EPSILON))
	{
		return fCoord =  length * cosf((angle - 90) / MATH_180_D_PI);
	}
	else if(((angle - 270) < FLT_EPSILON) && ((angle - 180.0) > -FLT_EPSILON))
	{
		return fCoord =  - length * cosf((270 - angle) / MATH_180_D_PI);
	}
	else if(((angle - 360) < FLT_EPSILON) && ((angle - 270) > -FLT_EPSILON))
	{
		return fCoord =  - length * cosf((angle - 270) / MATH_180_D_PI);
	}

    return fCoord;
}
/*! 
 * 根据幅值和向量（原点为起始点）与Y轴的夹角，计算向量终点的Y轴纵坐标
 * @param[in] length  幅值
 * @param[in] angle   与Y轴的夹角， 
 * @return    向量终点的Y轴纵坐标
 * @note      本方法仅供矢量图模式调用
 */
Float MathUtils::Coord_Y(Float length, Float angle)
{
	Float fCoord = 0.0;

	if(((angle - 90.0) < FLT_EPSILON) && (angle > -FLT_EPSILON))
	{
		return fCoord =  length * sinf((90 - angle) / MATH_180_D_PI);
	}
	else if(((angle - 180) < FLT_EPSILON) && ((angle - 90.0) > -FLT_EPSILON))
	{
		return fCoord =  - length * sinf((angle - 90) / MATH_180_D_PI);
	}
	else if(((angle - 270) < FLT_EPSILON) && ((angle - 180.0) > -FLT_EPSILON))
	{
		return fCoord =  - length * sinf((270 - angle) / MATH_180_D_PI);
	}
	else if(((angle - 360) < FLT_EPSILON) && ((angle - 270) > -FLT_EPSILON))
	{
		return fCoord =  length * sinf((angle - 270) / MATH_180_D_PI);
	}

    return fCoord;
}

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
bool MathUtils::DesignIIRFilter(
	double* pCoefBuf,
	int		nCoefCount,
	double  fs,
	double  fc,
	int		nOrder)
{
	if((!pCoefBuf) || nCoefCount < 4 || nOrder < 1  || 
		fs < fc * 2  || fc < 0.001 || 
		((UINT)((nOrder + 1) / 2) * 6 + 1 != nCoefCount))
		return FALSE;

    UINT i = 0,  j= 0, n = 0;;
	char  type[]  = "lowpass";	
	char  method[] = "Butterworth";

	//通带波纹最大衰减  && 阻带衰减
	double  apass  = -3,  astop = -20; 

	//为保证最小阶数小于nOrder，根据上述参数计算阻带截至频率
	double kernel = (RealMath::Pow(10.0, -0.1 * astop) - 1 ) / ( RealMath::Pow(10.0, -0.1 * apass) - 1 );
	double factor = RealMath::Log10(kernel) / 2 / nOrder;
	double fstop  = RealMath::Pow(10.0, factor) * fc;

	if((fstop < fc) || (fstop > fs / 2))   return FALSE;
 
	IIR  IIr_Butter(type, method);
	IIr_Butter.setParams(fs, fc, apass, fstop, astop);
	IIr_Butter.design();
 
	pCoefBuf[0] = IIr_Butter.getGain();
	for(i = 0; i < (UINT)((nOrder + 1) / 2); ++i)
	{
		j = i * 3;
		n = i * 6;
		pCoefBuf[n + 1] = IIr_Butter.getNumCoefs()[j + 0];
		pCoefBuf[n + 2] = IIr_Butter.getNumCoefs()[j + 1];
		pCoefBuf[n + 3] = IIr_Butter.getNumCoefs()[j + 2];
		pCoefBuf[n + 4] = IIr_Butter.getDenCoefs()[j + 0];
		pCoefBuf[n + 5] = IIr_Butter.getDenCoefs()[j + 1];
		pCoefBuf[n + 6] = IIr_Butter.getDenCoefs()[j + 2]; 
	}
	return TRUE;
}

/*! 
 * CRC8校验值计算—查表法
 * @param[in] pData		待校验的数据
 * @param[in] length	待校验的数据长度
 * @param[in] initial	CRC8字节初始化
 * @return    返回CRC8校验数据 
 */
unsigned char MathUtils::CRC8(const void* pData, int length, unsigned char initial)
{
    // 查表计算方法
    unsigned   char  *ucPtr    = (unsigned char*)pData;
    unsigned   char   ucIndex  = 0;           // CRC8校验表格索引   
    unsigned   char   ucCRC8   = initial;     // CRC8字节初始化

    // 进行CRC8位校验   
    while (length--)
    {   
        ucIndex = ucCRC8 ^ (*ucPtr++);   
        ucCRC8  = s_CRC8Tab[ucIndex];   
    }   

    return ucCRC8;    
}

/*! 
 * CRC32校验值计算—查表法
 * @param[in] pData		待校验的数据
 * @param[in] length	待校验的数据长度
 * @param[in] initial	CRC32字节初始化
 * @return    返回CRC32校验数据 
 */
unsigned int MathUtils::CRC32(const void* pvData, int length, unsigned int initial)
{
    BYTE* pData = (BYTE*)pvData;
    int   i     = 0;

    initial = initial ^ 0xFFFFFFFF;
    for (i = 0; i < length; i++)
    {
        initial = (initial >> 8) ^ s_CRC32Tab[(initial & 0xFF) ^ pData[i]];
    }
    initial = (initial ^ 0xFFFFFFFF);

    return initial; 
}

/*! 
 * 角度格式化，使转化后的角度大小在0-360或者-180——180范围内
 * @param[in] a					需要格式化的角度数据
 * @param[in] b_0_360_type		置1则转换为0-360度，否则转换为0-180度
 * @return   返回格式化后的角度数据
 */
Real MathUtils::FormatAngle(Real a, Integer b_0_360_type)
{
#if F260_TARGET_TI_DSP
#define MATH_1_D_PI                         0.31830988618379067153776752674503f
#define MATH_2PI                            6.283185307179586476925286766559f
#define FORMAT_ANGLE(a, b_0_360_type)                                   \
{                                                                       \
    int   n = _spint(a * MATH_1_D_PI);                                  \
    float m = ((_abs(n) + 1) >> 1) * MATH_2PI;                          \
    a = n > 0 ? (a - m) : (a + m);                                      \
    if (b_0_360_type && a <= 0) a += MATH_2PI;                          \
}

    FORMAT_ANGLE(a, b_0_360_type);

#else
    Integer n = (Integer)(a / MATH_PI);

    if (n > 0)
    {
        n = (n + 1) / 2;
    }
    else if (n < 0)
    {
        n = (n - 1) / 2;
    }

    if (n)
    {
        a -= n * (MATH_PI * 2);
    }
#endif
    if (b_0_360_type && a <= 0)
    {
        a += (MATH_PI * 2);
    }

    return a;
}

}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
