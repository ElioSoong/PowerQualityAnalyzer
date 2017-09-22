/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Averager.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2014/12/12
** Modified by:                
** Descriptions:               平均功能接口，包括线性平均和指数平均功能
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2014/12/12
** Descriptions:               
** 
*********************************************************************************************************/
#include "Includes.h"
#include "math/Averager.h"


namespace math {

IAverager::~IAverager(void)
{
}

/*! 
 * 复位平均器
 */
void   IAverager::Reset(void)
{
    m_nUpdateCount = 0;
}

/*! 
 * 平均器对象的创建
 * @param[in]  type		平均功能实现类型（线性/指数）
 * @param[in]  depth	LIN窗口宽度/EXP衰减常数
 * @param[in]  size		参与平均的字段个数（用于大批量数据的快速平均运算）
 * @param[in]  step		与平均的各个字段之间的步进值
 * @return     创建的平均功能对象
 */
IAverager* IAverager::Create( UINT type, Real depth, UINT size, UINT step)
{
    IAverager* pAverager = NULL;

	if(size == 0)  return pAverager;

    switch (type)
    {
    case Lin:
        pAverager = new LinAverager(depth, size, step);
        break;

    case Exp:
        pAverager = new ExpAverager(depth, size, step);
        break;

	case IEC:
		pAverager = new IECAverager(depth, size, step);
		break;

    default:
        pAverager = NULL;
        break;
    }

    return pAverager;
}

/*! 
 * 历史平均值，将未进行本次平均操作前的历史平均值返回
 * @param[out]   pData     只作为输出，历史平均值
 * @return       操作成功返回OK
 */
Result IAverager::Get(Real* pData)
{
	UINT i = 0;

	if(pData == NULL || m_pAver == NULL || (!m_nUpdateCount))  return Failed;

	for (i = 0; i < m_nSize; i++)
	{
		pData[i] = m_pAver[i];
	}
	return OK;
}

LinAverager::LinAverager(Real depth, UINT size, UINT step) : 
    IAverager(depth, size, step)
{ 
	UINT nDepth = (UINT)(depth + 0.5);
	ASSERT(nDepth);
	m_pData = new Real[nDepth * size];
	m_pAver = new Real[size];
	Reset();
}

LinAverager::~LinAverager(void)
{
	delete[] m_pData;
	m_pData = NULL;
	delete[] m_pAver;
	m_pAver = NULL;
}

/*! 
 * 更新平均值，将输入的一个或多个Real型字段做平均处理后按原位置返回
 * @param[inout]  pData    待平均的一个或多个字段，如果为多个字段，需保证内部结构与平均器的Step一致
 * @return        操作成功返回OK
 */
Result LinAverager::Update(Real* pData)
{
	UINT i = 0;
	UINT j = 0;
	UINT nData  = 0;
	UINT nEnd   = 0;
	UINT nDepth =  0;

	if(pData == NULL || m_pData == NULL)  return Failed;

	nDepth = (UINT)(m_fDepth + 0.5);

	//添加本次采样数据，更新历史数据
	if (m_nUpdateCount != nDepth)
	{
		for (i =0; i < m_nSize; i++)
		{
			m_pData[m_nUpdateCount * m_nSize + i] = pData[i * m_nStep];
		}
		m_nUpdateCount++;
	}
	else 
	{
		nData = (nDepth - 1) * m_nSize;

		for(i = 0; i < nData; i++)
		{
			m_pData[i] = m_pData[i + m_nSize];
		}
		for (i =0; i < m_nSize; i++)
		{
			m_pData[nData + i] = pData[i * m_nStep];
		}
	}

	//计算平均值并保存
	if (m_nUpdateCount)
	{
		nEnd   = (m_nUpdateCount * m_nSize - m_nSize);
		
		for (i = 0; i < m_nSize; i++)
		{
			for(j = i; j < nEnd; j = j + m_nSize)
			{
				pData[i * m_nStep] += m_pData[j];
			}
			pData[i * m_nStep] = pData[i * m_nStep] / m_nUpdateCount;
			m_pAver[i]         = pData[i * m_nStep];
		}	

		return OK;
	}
 
    return Failed;
}

/*! 
 * 线性平均功能复位功能
 * @brief  复位当前历史数据，历史平均值为初始零状态
 */
void   LinAverager::Reset(void)
{
	 m_nUpdateCount = 0;

	 UINT nsize = sizeof(Real) * (UINT)(m_fDepth + 0.5) * m_nSize;
	 memset(m_pData, 0.0, nsize);
	 memset(m_pAver, 0.0, m_nSize * sizeof(Real));
}

ExpAverager::ExpAverager(Real depth, UINT size, UINT step) : 
    IAverager(depth, size, step)
{
    m_pAver = new Real[size];
    m_weight   = 1.0f / depth;
}

ExpAverager::~ExpAverager(void)
{
    delete[] m_pAver;
    m_pAver = NULL;
}

/*! 
 * 指数平均功能—更新数据（平均，历史采样数据，更新次数等）
 * @param[inout] pData	本次操作采样数据
 * @return			操作成功返回OK
 */
Result ExpAverager::Update(Real* pData)
{
    UINT i = 0;

	if(pData == NULL || m_pAver == NULL)  return Failed;

    if (m_nUpdateCount++ == 0)
    {
        for (i = 0; i < m_nSize; i++)
        {
            m_pAver[i] = pData[i * m_nStep];
        }
    }
    else
    {
        for (i = 0; i < m_nSize; i++)
        {
            pData[i * m_nStep] = m_pAver[i] + (pData[i * m_nStep] - m_pAver[i]) * m_weight;

			m_pAver[i] = pData[i * m_nStep];
        }
    }

    return OK;
}
/*! 
 * IEC频率专用平均器，更新平均值，将输入的一个或多个Real型字段做平均处理后按原位置返回
 * @param[inout]  pData    待平均的一个或多个字段，如果为多个字段，需保证内部结构与平均器的Step一致
 * @return        操作成功返回OK
 */
Result IECAverager::Update(Real* pData)
{
	UINT i = 0;
	UINT j = 0;
	UINT nData  = 0;
	UINT nEnd   = 0;
	UINT nDepth =  0;

	if(pData == NULL || m_pData == NULL)  return Failed;

	nDepth = (UINT)(m_fDepth + 0.5);

	for (i = 0; i < m_nSize; i++)
	{
		//添加本次采样数据，更新历史数据,如果小于1的值则不更新该值
		if (pData[i * m_nStep] < 1)  
		{
			continue;
		}
		
		if(m_pUpdateCount[i] != nDepth)
		{
			m_pData[m_pUpdateCount[i] * m_nSize + i] = pData[i * m_nStep];
			m_pUpdateCount[i]++;
		}
		else
		{
			nData = (nDepth - 1) * m_nSize;

			for(j = i; j < nData; j = j + m_nSize)
			{
				m_pData[j] = m_pData[j + m_nSize];
			}

			m_pData[nData + i] = pData[i * m_nStep];		
		}

		//计算平均值并保存
		if (m_pUpdateCount[i])
		{
			
			nEnd   = (m_pUpdateCount[i] * m_nSize - m_nSize);

			for(j = i; j < nEnd; j = j + m_nSize)
			{
				pData[i * m_nStep] += m_pData[j];
			}
			m_pAver[i] = pData[i * m_nStep] / m_pUpdateCount[i];
		}

	} 
   return OK;
}

Result IECAverager::Get(Real* pData)
{
	UINT i = 0;
	BOOL bUpdate = TRUE;

	for (i = 0; i < m_nSize; i++)
	{
		if (m_pUpdateCount[i] > 0)
		{
			bUpdate = FALSE;
			break;
		}
	}

	if(pData == NULL || m_pAver == NULL || bUpdate)  return Failed;

	for (i = 0; i < m_nSize; i++)
	{
		pData[i] = m_pAver[i];
	}
	return OK;
}

void IECAverager::Reset(void)
{
	m_nUpdateCount = 0;
	memset(m_pUpdateCount, 0, m_nSize * sizeof(UINT));

	UINT nsize = sizeof(Real) * (UINT)(m_fDepth + 0.5) * m_nSize;
	memset(m_pData, 0.0, nsize);
	memset(m_pAver, 0.0, m_nSize * sizeof(Real));
}

IECAverager::~IECAverager(void)
{
	delete[] m_pUpdateCount;
	m_pUpdateCount = NULL;
	delete[] m_pData;
	m_pData = NULL;
	delete[] m_pAver;
	m_pAver = NULL;
}

IECAverager::IECAverager(Real depth, UINT size, UINT step) : 
IAverager(depth, size, step)
{ 
	UINT nDepth = (UINT)(depth + 0.5);
	ASSERT(nDepth);
	m_pData = new Real[nDepth * size];
	m_pAver = new Real[size];
	m_pUpdateCount = new UINT[size];
	Reset();
}
/*! 
 * 平均功能单元测试函数
 * @brief 1到15的线性平均，1到300的指数平均
 */
#ifdef _DEBUG
IMPLEMENT_UNIT_TEST(IAverager)
{
	UINT i = 0;
 
	IAverager* pIAverager =  IAverager::Create(Exp,2,2,1);
 
	Real* pData = new Real[3];
	Real* pGet  = new Real[3];
	for(i = 0; i < 16; i++)
	{
		pData[0] = i + 1;
		pData[2] = i + 2;
		pData[1] = i + 3;
		INF("old pData[%d][0] = %.2f\n", i, pData[0]);
		INF("old pData[%d][2] = %.2f\n", i, pData[1]);
		pIAverager->Get(pGet);
		INF("pExpAverager pData[%d][0] = %.2f\n", i, pGet[0]);
		INF("pExpAverager pData[%d][2] = %.2f\n", i, pGet[1]);
		pIAverager->Update(pData);
		INF("pExpAverager pData[%d][0] = %.2f\n", i, pData[0]);
		INF("pExpAverager pData[%d][2] = %.2f\n", i, pData[1]);
		pIAverager->Get(pData);
		INF("pExpAverager pData[%d][0] = %.2f\n", i, pData[0]);
		INF("pExpAverager pData[%d][2] = %.2f\n", i, pData[1]);
	}
	 
	pIAverager =  IAverager::Create(Exp,256,2,1);
	Real* pLinData = new Real[2 * 1];
	INF("pLinAverager->m_fDepth = %.1f\n",    pIAverager->m_fDepth);
	for (i = 0; i < 300; i++)
	{
		pLinData[0] = i + 1;
		pLinData[1] = i + 2;
		pIAverager->Update(pLinData);
		INF("pLinAverager pLinData[%d][0] = %.2f\n", i, pLinData[0]);
		INF("pLinAverager pLinData[%d][2] = %.2f\n", i, pLinData[1]);
		INF("pLinAverager->m_fDepth = %.1f\n",    pIAverager->m_fDepth);
		INF("pLinAverager->m_nUpdateCount = %d\n",    pIAverager->m_nUpdateCount);
	} 
	pIAverager->Get(pLinData);
	INF("pExpAverager pData[%d][0] = %.2f\n", i, pLinData[0]);
	INF("pExpAverager pData[%d][2] = %.2f\n", i, pLinData[1]);
	return FALSE;
}
#endif

}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
