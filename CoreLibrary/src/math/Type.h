/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Type.h
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
#pragma once


namespace math {

typedef int                                 Integer;                    //!< 数学整型
typedef unsigned int                        UInt;                       //!< 数学无符号整型
typedef unsigned long long                  ULong;                      //!< 数学无符号长整型

typedef float                               Float;                      //!< 数学单精度浮点数
typedef double                              Double;                     //!< 数学双精度浮点数

/**
 *  @enum 
 *  @brief 数学运算结果码（在全部math命名空间下使用）
 */
typedef enum
{
    OK = 0,                                                             //!< 数学运算成功
    Failed,                                                             //!< 数学运算失败
} Result;

/**
 *  @brief 特殊格式的单精度浮点值（NAN,INF+,INF-）
 *         使用时请通过MATH_PA_XXX对外公共宏定义引用以下值，否则可能存在一致性问题
 */
#if 0
/**
 *  @brief 使用横河WT3000的自定义格式
 */
static const unsigned int s_nNan  = 0x7e951beeu;                        //!< NAN
static const unsigned int s_nInfP = 0x7e94f56au;                        //!< INF+
static const unsigned int s_nInfN = 0x7e94f56au;                        //!< INF-
#else
/**
 *  @brief 使用IEEE754定义的标准格式    
 */
static const unsigned int s_nNan  = 0x7fffffffu;                        //!< NAN
static const unsigned int s_nInfP = 0x7f800000u;                        //!< INF+
static const unsigned int s_nInfN = 0xff800000u;                        //!< INF-
#endif

#define MATH_PA_NAN         (*((Float*)&s_nNan))                        //!< NAN
#define MATH_PA_INFP        (*((Float*)&s_nInfP))                       //!< INF+
#define MATH_PA_INFN        (*((Float*)&s_nInfN))                       //!< INF-
#define MATH_PA_INF         MATH_PA_INFP                                //!< INF（默认为INF+）

/**
 *  @brief 数学常用常量表，避免重复计算相同常量
 */
#define MATH_PI             3.1415926535897932384626433832795           //!< π
#define MATH_E              2.7182818284590452353602874713527           //!< e
#define MATH_SQRT2          1.4142135623730950488016887242097           //!< sqrt(2)
#define MATH_LOG2           0.30102999566398119521373889472449          //!< log10(2)
#define MATH_LN2            0.69314718055994530941723212145818          //!< ln(2)
#define MATH_180_D_PI       57.295779513082320876798154814105           //!< 180/π
#define MATH_PI_D_180       0.017453292519943295769236907684886         //!< π/180
#define MATH_SQRT3_D_2      0.86602540378443864676372317075294          //!< sqrt(3)/2
#define MATH_SQRT3_D_3      0.57735026918962576450914878050196          //!< sqrt(3)/3
#define MATH_PI_D_2SQRT2    1.1107207345395915617539702475152           //!< π/(2*sqrt(2))

/**
 *  @class FixedPointReal
 *  @brief 使用定点算法库模拟浮点运算（TI Fast Math）的数据类型
 */
class FixedPointReal
{
public:
    FixedPointReal(void);
    ~FixedPointReal(void);

    /**
     *  @brief 以下为使用不同输入类型进行初始化的构造函数
     */
    FixedPointReal(const Double&  d) {};
    FixedPointReal(const Float&   f) {};
    FixedPointReal(const Integer& i) {};

public:
    /**
     *  @brief 以下为数据类型隐式强制转换函数
     */
    operator Double (void) const;
    operator Float  (void) const;
    operator Integer(void) const;

    /**
     *  @brief 以下为基础运算符的重载函数
     *  @note  sizeof运算符不能重载
     */
    FixedPointReal& operator + (const FixedPointReal& a);
    FixedPointReal& operator - (const FixedPointReal& a);
    FixedPointReal& operator * (const FixedPointReal& a);
    FixedPointReal& operator / (const FixedPointReal& a);
    FixedPointReal& operator - (void) const;

    static const FixedPointReal EPS;                                    //!< 分辨率
    static const FixedPointReal MAX;                                    //!< 最大值
    static const FixedPointReal MIN;                                    //!< 最小值
};

/**
 *  @enum  MathRealType
 *  @brief 系统支持的实数类型列表（float/double/FixedPointReal）
 */
typedef enum
{
    RealIsFloat = 1,                                                    //!< 使用Float代表实数
    RealIsDouble,                                                       //!< 使用Double代表实数
    RealIsFixedPoint,                                                   //!< 使用定点数代表实数（TI Fast-math）
} MathRealType;

/**
 *  @def   MATH_REAL_TYPE
 *  @brief 定义数学库的Real类型的来源（float/double/FixedPointReal）
 */
#define MATH_REAL_TYPE                      RealIsDouble                //!< 默认实数类型为double

/**
 *  @typedef Real
 *  @brief   数学库的Real类型
 */
#if MATH_REAL_TYPE == RealIsFloat
typedef Float Real;
#elif MATH_REAL_TYPE == RealIsDouble
typedef Double Real;
#elif MATH_REAL_TYPE == RealIsFixedPoint
typedef FixedPointReal Real;
#else
#error Invalid MATH_REAL_TYPE macro definition. 
#endif

/**
 *  @class Complex
 *  @brief 复数类型以及复数运算
 */
class Complex 
{
public:
    Real r;                                                             //!< 实部
    Real i;                                                             //!< 虚部

    Complex(void) : r(0), i(0) {}
    ~Complex(void) {}

    Complex(Real _r, Real _i) : r(_r), i(_i) {}
    Complex(Real v[2]) : r(v[0]), i(v[1]) {}

public:
    /*! 
     * 复数加法：   return=this+a
     * @param[in]  a 加数
     * @return     运算结果
     */
    inline Complex operator + (const Complex& a)
    {
        return Complex(r + a.r, i + a.i);
    }

    /*! 
     * 复数减法：   return=this-a
     * @param[in]  a 减数
     * @return     运算结果
     */
    inline Complex operator - (const Complex& a)
    {
        return Complex(r - a.r, i - a.i);
    }

    /*! 
     * 复数乘常量： return=this*λ
     * @param[in]  a 乘数
     * @return     运算结果
     */
    inline Complex operator * (const Real& a)
    {
        return Complex(r*a, i*a);
    }

    /*! 
     * 复数乘复数： return=this*a
     * @param[in]  a 乘数
     * @return     运算结果
     */
    inline Complex operator * (const Complex& a)
    {
        /**
         *  @brief 复数乘法
         *  (a+bi)(c+di) = (ac-bd)+(ad+bc)i
         */
        return Complex(r*a.r-i*a.i, r*a.i+i*a.r);
    }

    /*! 
     * 复数除常量： return=this/λ
     * @param[in]  a 除数
     * @return     运算结果
     */
    inline Complex operator / (const Real& a)
    {
        return Complex(r/a, i/a);
    }

    /*! 
     * 复数取反：   return=-this
     * @return     运算结果
     */
    inline Complex operator - (void) const
    {
        return Complex(-r, -i);
    }

    /*! 
     * 复数共轭：   return=*this
     * @return     运算结果
     */
    inline Complex operator ~ (void) const
    {
        return Complex(r, -i);
    }

    /*! 
     * 复数模平方： return=|this|^2
     * @return     运算结果
     */
    inline Real SquareModulus(void) const
    {
        return (r * r + i * i);
    }

public:
    /*! 
     * 复数模：     return=|this|
     * @return     运算结果
     */
    Real Modulus(void) const;

    /*! 
     * 复数辅角：   return=∠this
     * @return     运算结果
     */
    Real Angle(void) const;

    /*! 
     * 向量夹角：   return=∠(b->a)
     * @param[in]  a 以复数形式表示的二维向量a
     * @param[in]  b 以复数形式表示的二维向量b
     * @return     运算结果
     * @note       需要注意的是这里只是以复数代表一个二维向量，并求取两个向量间的夹角
     *             但复数是复数，向量是向量，两者的运算规则是不同的。
     */
    static Real Angle(const Complex& a, const Complex& b);

    /*! 
     * 向量叉乘：   return=a×b
     * @param[in]  a 以复数形式表示的二维向量a
     * @param[in]  b 以复数形式表示的二维向量b
     * @return     运算结果
     * @note       需要注意的是这里只是以复数代表一个二维向量，并求取两个向量间的外积
     *             但复数是复数，向量是向量，两者的运算规则是不同的。
     */
    static Real CrossProduct(const Complex& a, const Complex& b);

    /*! 
     * 向量共轭：   return=a¤b
     * @param[in]  a 以复数形式表示的二维向量a
     * @param[in]  b 以复数形式表示的二维向量b
     * @return     运算结果
     * @note       需要注意的是这里只是以复数代表一个二维向量，并求取两个向量间的共轭
     *             但复数是复数，向量是向量，两者的运算规则是不同的。
     */
    static Real ConjProduct(const Complex& a, const Complex& b);
};


}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
