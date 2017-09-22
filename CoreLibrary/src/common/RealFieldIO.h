/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  RealFieldIO.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/04/27
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhang WenBo
** Created date:               2017/04/27
** Descriptions:               END
** 
*********************************************************************************************************/
#pragma once

#include "FieldLibrary/RealField.h"
#include "LoopedFrameBuffer.h"
#include "storage/TrendStorageItem.h"
#include "storage/IStorage.h"

template<typename T, int FRAMESIZE, int MAXCHANNELCOUNT>
class RealFieldIO : public FieldIO
{
public:
    RealFieldIO(IField* field = NULL) 
        : FieldIO(field)
        , m_extractRatio(0)
        , m_curCountRatio(0)
    {
        resetContextData();
    }

public:

    /*!
     * 写数据到文件IO
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回写入数据数目
     */
    quint64 write(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE
    {
        if (dataTypeFlags & FieldDataType)
            return writeFieldData(file);
        if (dataTypeFlags & TrendDataType)
            return writeTrendData(file, dataTypeFlags, data);
        if (dataTypeFlags & ThumbnailDataType)
            return writeThumbnailData(file, dataTypeFlags);
        return 0;
    }


    /*!
     * 从文件IO中读取数据
     * @param[in]  file          打开的文件指针
     * @param[in]  dataTypeFlags 内容类型
     * return   成功返回读出的数据数目
     */
    quint64 read(QIODevice* file, int dataTypeFlags = -1, void *data = NULL) Q_DECL_OVERRIDE
    {
        if (dataTypeFlags & FieldDataType)
            return readFieldData(file);
        if (dataTypeFlags & TrendDataType)
            return readTrendData(file, dataTypeFlags, data);
        if (dataTypeFlags & ThumbnailDataType)
            return readThumbnailData(file);
        return 0;
    }

    /*!
     * 当前字段的指定类型的内容是否可写
     * @param[in]  dataTypeFlags 内容类型
     * @param[in]  channel 通道号
     * @return     可写
     */
    bool isWritable(int dataTypeFlags = -1, int channel = 0) Q_DECL_OVERRIDE
    {
        Q_ASSERT(m_field);
        IField* channelField = m_field->child(channel);
        LoopedFrameBuffer<T> trendBuffer =
            channelField->data(-1, -1, TrendDataRole).value<LoopedFrameBuffer<T> >();
        if (dataTypeFlags & TrendDataType)
        {
            if (!(dataTypeFlags & FlushBeforeClosing))
            {
                bool ok;
                if (trendBuffer.isValid())
                {
                    ok = trendBuffer.curElementCount()
                        >= (trendBuffer.frameSize() * 2);
                }
                else
                {
                    ok = false;
                }
                return ok;
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    void resetContextData()
    {
        for (int i = 0; i < MAXCHANNELCOUNT; ++i)
        {
            m_contextOffset[i] = 0;
            resetContextData(m_contextData[i]);
        }
    }

protected:

    virtual void resetContextData(T & contextData)
    {
        memset(&contextData, 0, sizeof(T));
    }

    virtual bool extractAPoint(T &outData ,T* framePtr, int pointCount)
    {
        if (framePtr)
        {
            for (int i = 0; i < pointCount; ++i)
            {
                if (i == pointCount / 2)
                {
                    outData = *(framePtr + i);
                    return true;
                }
            }
        }
        return false;
    }

    /*!
     * 将FieldData类型数据写数据到文件IO
     * @param[in]  file          打开的文件指针
     * return   成功返回写入数据数目
     */
    quint64 writeFieldData(QIODevice* file)
    {
        Q_ASSERT(m_field);
        RealField *dataField = static_cast<RealField *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        int countLenth = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            float data = dataField->data(i, 0, FieldDataRole).toFloat();
            countLenth += file->write((char *)(&data), sizeof(float));
        }
        return countLenth;
    }

    /*!
     * 从文件IO中读取FileData类型数据
     * @param[in]  file          打开的文件指针
     * return   成功返回读出的数据数目
     */
    quint64 readFieldData(QIODevice* file)
    {
        Q_ASSERT(m_field);
        RealField *dataField = static_cast<RealField *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        int countLenth = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            float data = 0;
            countLenth += file->read((char *)(&data), sizeof(float));
            m_field->setData(i, 0, data, FieldDataRole);
        }
        return countLenth;
    }

    /*!
    * 将TrendData类型数据写数据到文件IO
    * @param[in]  file          打开的文件指针
    * return   成功返回写入数据数目
    */
    quint64 writeTrendData(QIODevice* file, int dataTypeFlags, void *data = NULL)
    {
        Q_ASSERT(m_field);
        RealField *dataField = static_cast<RealField *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        LoopedFrameBuffer<T> trendBuffer =
            m_field->data(0, 0, TrendDataRole).value<LoopedFrameBuffer<T> >();

        T* frame = NULL;
        int framSize = FRAMESIZE;
        int validElementCount = FRAMESIZE;

        int witeLen = 0;

        static T emptyFrame[FRAMESIZE];
        for (int i = 0; i < channelCount; ++i)
        {
            trendBuffer =
                m_field->data(i, 0, TrendDataRole).value<LoopedFrameBuffer<T> >();
            if (dataTypeFlags & FlushBeforeClosing)
            {
                framSize = 2 * FRAMESIZE;
                static QByteArray flushFrameBuf;
                flushFrameBuf.clear();
                if (trendBuffer.isValid())
                {
                    validElementCount = trendBuffer.curElementCount();
                    if (validElementCount > FRAMESIZE)
                    {
                        flushFrameBuf.append((char *)(trendBuffer.popFrame()), FRAMESIZE * sizeof(T));
                        flushFrameBuf.append((char *)(trendBuffer.frontFrame()), FRAMESIZE * sizeof(T));
                    }
                    else
                    {
                        flushFrameBuf.append((char *)(trendBuffer.frontFrame()), FRAMESIZE * sizeof(T));
                        flushFrameBuf.append((char *)(emptyFrame), FRAMESIZE * sizeof(T));
                    }
                }
                else
                {
                    flushFrameBuf.append((char *)(emptyFrame), FRAMESIZE * sizeof(T));
                    flushFrameBuf.append((char *)(emptyFrame), FRAMESIZE * sizeof(T));
                }
                frame = reinterpret_cast<T *>(flushFrameBuf.data());
            }
            else
            {
                if (trendBuffer.isValid())
                {
                    frame = trendBuffer.popFrame();
                }
                else
                {
                    frame = emptyFrame;
                }
            }

            char *tempData = reinterpret_cast<char *>(frame);
            if (tempData)
            {
                witeLen += file->write((char *)(&validElementCount), sizeof(int));
                witeLen += file->write(tempData, sizeof(T)*framSize);
            }
        }
        return witeLen;
    }

    /*!
    * 从文件IO中读取TrendData类型数据
    * @param[in]  file          打开的文件指针
    * return   成功返回读出的数据数目
    */
    quint64 readTrendData(QIODevice* file, int dataTypeFlags, void *data = NULL)
    {
        StorageItemPosition *position = reinterpret_cast<StorageItemPosition *>(data);
        if (!position)
            return 0;
        int toReadDataNum = position->m_pointCount;

        // 初始化抽取比例
        float count = TrendStorageItem::fileCount();

        if (count == 0)
        {
            qDebug() << "TrendStorageItem::fileCount() Error!!";
            return -1;
        }
        if (fabs(m_extractRatio - (1 / count)) > 1E-6)
        {
            m_extractRatio = 1 / count;
        }

        if (m_curCountRatio < 1)
        {
            m_curCountRatio += m_extractRatio;
        }

        Q_ASSERT(m_field);
        RealField *dataField = static_cast<RealField *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();

        // 读取数据到缓存区
        LoopedFrameBuffer<T> trendReadBuf;
        trendReadBuf.clear();
        trendReadBuf.resize(FRAMESIZE, 2);
        if (!trendReadBuf.isValid())
            return 0;
        int readLen = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            if (m_curCountRatio == m_extractRatio)
            {
                resetContextData(m_contextData[i]);
                tempTrendBuf[i].clear();
                tempTrendBuf[i].resize(FRAMESIZE, 2);
                LoopedFrameBuffer<T> rawTrendBuffer =
                    m_field->data(i, 0, TrendDataRole).value<LoopedFrameBuffer<T> >();
                if (rawTrendBuffer.isValid())
                {
                    rawTrendBuffer.reset();
                    m_field->setData(i, 0,
                        QVariant::fromValue<LoopedFrameBuffer<T> >(rawTrendBuffer), TrendDataRole);
                }
            }

            char *tempData = reinterpret_cast<char *>(trendReadBuf.frontFrame());
            int validElementCount = 0;
            if (tempData)
            {
                trendReadBuf.reset();

                readLen += file->read((char *)(&validElementCount), sizeof(int));

                // 默认flush两帧
                if (dataTypeFlags & FlushBeforeClosing)
                {
                    toReadDataNum = position->m_pointIndex;
                    if (toReadDataNum <= FRAMESIZE)
                    {
                        readLen += file->read(tempData, sizeof(T)*(toReadDataNum));
                        T invalidData[FRAMESIZE * 2];
                        readLen += file->read((char *)(invalidData), sizeof(T)*((FRAMESIZE)* 2 - toReadDataNum));
                    }
                    else
                    {
                        T popFrameBuf[FRAMESIZE];
                        readLen += file->read((char *)(popFrameBuf), sizeof(T)*(FRAMESIZE));
                        trendReadBuf.pushFrame(popFrameBuf);
                        T frontFrameBuf[FRAMESIZE];
                        readLen += file->read((char *)(frontFrameBuf), sizeof(T)*(FRAMESIZE));
                        trendReadBuf.pushFrame(frontFrameBuf);
                    }
                }
                else
                {
                    if (!position->m_endFlag)
                    {
                        toReadDataNum = position->m_pointCount - position->m_pointIndex;
                        if (toReadDataNum != FRAMESIZE)
                        {
                            T invalidData[FRAMESIZE];
                            readLen += file->read((char *)(invalidData), sizeof(T)*(FRAMESIZE - toReadDataNum));
                        }
                        readLen += file->read(tempData, sizeof(T)*(toReadDataNum));
                    }
                    else
                    {
                        toReadDataNum = position->m_pointIndex;
                        if (toReadDataNum <= FRAMESIZE)
                        {
                            readLen += file->read(tempData, sizeof(T)*(toReadDataNum));
                            T invalidData[FRAMESIZE];
                            readLen += file->read((char *)(invalidData), sizeof(T)*(FRAMESIZE - toReadDataNum));
                        }
                    }
                }

                if (toReadDataNum < validElementCount || validElementCount <= 0)
                {
                    validElementCount = toReadDataNum;
                }
                trendReadBuf.setValidElementCount(validElementCount);

                if (m_curCountRatio > 1)
                {
                    m_curCountRatio = 1;
                }

#if 1
                // 抽取算法
                if (!extractTrendPoints(trendReadBuf, i))
                {
                    qDebug() << "RealField::extractTrendPoints Failed !! element=" << i;
                    return -1;
                }
#endif

                // 计算当前读取比例并发送信号通知视图
                int val = (i + 1) * 100 * m_curCountRatio / channelCount;
                if (fabs(m_curCountRatio - 1) < 1E-6 && i == channelCount - 1)
                {
                    val = 100;
                }
                Storage* storagePtr = Storage::instance();
                emit storagePtr->readRatio(val);
            }
        }
        // 最后一次读取结束清空读取比例计数
        if (position->m_endFlag)
        {
            m_curCountRatio = 0;
            resetContextData();
        }

        return readLen;
    }

    /*!
    * 将Trend缩略图类型数据写数据到文件IO
    * @param[in]  file          打开的文件指针
    * return   成功返回写入数据数目
    */
    quint64 writeThumbnailData(QIODevice* file, int dataTypeFlags)
    {
        Q_ASSERT(m_field);
        RealField *dataField = static_cast<RealField *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        LoopedFrameBuffer<T> thumbnailBuffer =
            m_field->data(0, 0, ThumbnailDataRole).value<LoopedFrameBuffer<T> >();
        int framSize = thumbnailBuffer.frameSize();
        T* frame = NULL;
        int validElementCount = 0;
        int writeLen = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            thumbnailBuffer =
                m_field->data(i, 0, ThumbnailDataRole).value<LoopedFrameBuffer<T> >();
            if (thumbnailBuffer.isValid())
            {
                frame = thumbnailBuffer.frontFrame();
                validElementCount = thumbnailBuffer.curElementCount();
            }

            char *tempData = reinterpret_cast<char *>(frame);
            if (tempData)
            {
                writeLen += file->write((char *)(&validElementCount), sizeof(int));
                writeLen += file->write(tempData, sizeof(T)*framSize);
            }
        }

        return writeLen;
    }

    /*!
    * 从文件IO中读取Trend缩略图类型数据
    * @param[in]  file          打开的文件指针
    * return   成功返回读出的数据数目
    */
    quint64 readThumbnailData(QIODevice* file)
    {
        Q_ASSERT(m_field);
        RealField *dataField = static_cast<RealField *>(m_field);
        Q_ASSERT(dataField);
        int channelCount = dataField->channelCount();
        LoopedFrameBuffer<T> thumbnailBuffer =
            m_field->data(0, 0, ThumbnailDataRole).value<LoopedFrameBuffer<T> >();
        int framSize = thumbnailBuffer.frameSize();
        T* frame = NULL;
        int validElementCount = 0;
        int readLen = 0;
        for (int i = 0; i < channelCount; ++i)
        {
            thumbnailBuffer =
                m_field->data(i, 0, ThumbnailDataRole).value<LoopedFrameBuffer<T> >();
            if (thumbnailBuffer.isValid())
            {
                framSize = thumbnailBuffer.frameSize();
                frame = thumbnailBuffer.frontFrame();
            }

            char *tempData = reinterpret_cast<char *>(frame);
            if (tempData)
            {
                readLen += file->read((char *)(&validElementCount), sizeof(int));
                readLen += file->read(tempData, sizeof(T) * framSize);
            }
            if (thumbnailBuffer.isValid())
            {
                thumbnailBuffer.setValidElementCount(validElementCount);
            }
            m_field->setData(i, 0,
                QVariant::fromValue<LoopedFrameBuffer<T> >(thumbnailBuffer), ThumbnailDataRole);
        }

        return readLen;
    }

#if 1
    /*!
    *  从缓存区中抽取Trend数据算法(实时抽取，变读边抽取)
    * @param [in]  buffer  读取Trend数据的缓存区
    * @param [in]  elemet  通道号
    * @return 抽取成功返回true
    */
    bool extractTrendPoints(LoopedFrameBuffer<T>& buffer, int element)
    {
        //  抽取算法
        LoopedFrameBuffer<T> rawTrendBuffer =
            m_field->data(element, 0, TrendDataRole).value<LoopedFrameBuffer<T> >();
        // 加速抽取
        if (!rawTrendBuffer.isValid() || buffer.curElementCount() == 0)
        {
            // 没有数据直接不抽取返回
            return true;
        }
        if (buffer.curElementCount() <= 0)
        {
            qDebug() << "Check the Buffer!!, buffer.curElementCount() <= 0";
            return false;
        }
        int framsize = buffer.curElementCount();

        // 获取每个文件(每帧)抽取点数的步进值
        float step = 1 / m_extractRatio;
        // 步进临界区域点数偏移记录offset, 访问数据点优化变量iterCount(记录循环次数)
        float offset = 0;
        int iterCount = 0;
        T temData = m_contextData[element];

        // 获取该文件中buffer数据帧的指针
        T *frame = buffer.frontFrame();
        QByteArray flushFrameBuf;
        if (framsize > FRAMESIZE)
        {
            flushFrameBuf.clear();
            flushFrameBuf.append((char *)(buffer.popFrame()), FRAMESIZE * sizeof(T));
            flushFrameBuf.append((char *)(buffer.frontFrame()), FRAMESIZE * sizeof(T));
            frame = reinterpret_cast<T*>(flushFrameBuf.data());
        }

        for (offset = m_contextOffset[element]; offset <= framsize; offset = ++iterCount * step + m_contextOffset[element])
        {
            int index = qMax((int)floor(offset), 0);
            int nextIndex = floor(offset + step);

            // 抽取点的抽取区间完整标志
            bool isPointCompleted = true;
            if (nextIndex > framsize)
            {
                nextIndex = framsize;
                isPointCompleted = false;
            }

            T* baseFramePtr = frame;
            int pointCount = 0;
            for (int j = index; j < nextIndex; ++j, ++frame)
            {
                ++pointCount;
            }
            if (!extractAPoint(temData, baseFramePtr, pointCount))
            {
                qDebug() << "extractAPoint Error!";
                return false;
            }

            // 将抽取的点给trendBuffer
            if (isPointCompleted)
            {
                LoopedFrameBuffer<T> rawTrendBuffer =
                    m_field->data(element, 0, TrendDataRole).value<LoopedFrameBuffer<T> >();
                if (rawTrendBuffer.isValid())
                {
                    tempTrendBuf[element].pushElement(temData);
                    m_field->setData(element, 0,
                        QVariant::fromValue<LoopedFrameBuffer<T> >(tempTrendBuf[element]), TrendDataRole);
                }

                resetContextData(temData);
            }
        }
        m_contextData[element] = temData;

        int index = floor(offset);

        m_contextOffset[element] = index % framsize + (offset - index) - step;

        int nextIndex = floor(m_contextOffset[element] + step);
        if (nextIndex == 0)
        {
            m_contextOffset[element] = 0;
        }

        return true;
    }
#endif

protected:

    T     m_contextData[MAXCHANNELCOUNT];              //!< 保存文件临界区域的抽取点

    float m_contextOffset[MAXCHANNELCOUNT];            //!< 保存文件临界区域的抽取偏移量

    float m_extractRatio;                                           //!< 每次读取趋势数据文件的抽取比例
    float m_curCountRatio;                                          //!< 当前抽取比例计数

    LoopedFrameBuffer<T>
        tempTrendBuf[MAXCHANNELCOUNT];                 //!< 抽取Trend数据的临时缓存区
};