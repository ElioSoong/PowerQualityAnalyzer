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
     * д���ݵ��ļ�IO
     * @param[in]  file          �򿪵��ļ�ָ��
     * @param[in]  dataTypeFlags ��������
     * return   �ɹ�����д��������Ŀ
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
     * ���ļ�IO�ж�ȡ����
     * @param[in]  file          �򿪵��ļ�ָ��
     * @param[in]  dataTypeFlags ��������
     * return   �ɹ����ض�����������Ŀ
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
     * ��ǰ�ֶε�ָ�����͵������Ƿ��д
     * @param[in]  dataTypeFlags ��������
     * @param[in]  channel ͨ����
     * @return     ��д
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
     * ��FieldData��������д���ݵ��ļ�IO
     * @param[in]  file          �򿪵��ļ�ָ��
     * return   �ɹ�����д��������Ŀ
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
     * ���ļ�IO�ж�ȡFileData��������
     * @param[in]  file          �򿪵��ļ�ָ��
     * return   �ɹ����ض�����������Ŀ
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
    * ��TrendData��������д���ݵ��ļ�IO
    * @param[in]  file          �򿪵��ļ�ָ��
    * return   �ɹ�����д��������Ŀ
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
    * ���ļ�IO�ж�ȡTrendData��������
    * @param[in]  file          �򿪵��ļ�ָ��
    * return   �ɹ����ض�����������Ŀ
    */
    quint64 readTrendData(QIODevice* file, int dataTypeFlags, void *data = NULL)
    {
        StorageItemPosition *position = reinterpret_cast<StorageItemPosition *>(data);
        if (!position)
            return 0;
        int toReadDataNum = position->m_pointCount;

        // ��ʼ����ȡ����
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

        // ��ȡ���ݵ�������
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

                // Ĭ��flush��֡
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
                // ��ȡ�㷨
                if (!extractTrendPoints(trendReadBuf, i))
                {
                    qDebug() << "RealField::extractTrendPoints Failed !! element=" << i;
                    return -1;
                }
#endif

                // ���㵱ǰ��ȡ�����������ź�֪ͨ��ͼ
                int val = (i + 1) * 100 * m_curCountRatio / channelCount;
                if (fabs(m_curCountRatio - 1) < 1E-6 && i == channelCount - 1)
                {
                    val = 100;
                }
                Storage* storagePtr = Storage::instance();
                emit storagePtr->readRatio(val);
            }
        }
        // ���һ�ζ�ȡ������ն�ȡ��������
        if (position->m_endFlag)
        {
            m_curCountRatio = 0;
            resetContextData();
        }

        return readLen;
    }

    /*!
    * ��Trend����ͼ��������д���ݵ��ļ�IO
    * @param[in]  file          �򿪵��ļ�ָ��
    * return   �ɹ�����д��������Ŀ
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
    * ���ļ�IO�ж�ȡTrend����ͼ��������
    * @param[in]  file          �򿪵��ļ�ָ��
    * return   �ɹ����ض�����������Ŀ
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
    *  �ӻ������г�ȡTrend�����㷨(ʵʱ��ȡ������߳�ȡ)
    * @param [in]  buffer  ��ȡTrend���ݵĻ�����
    * @param [in]  elemet  ͨ����
    * @return ��ȡ�ɹ�����true
    */
    bool extractTrendPoints(LoopedFrameBuffer<T>& buffer, int element)
    {
        //  ��ȡ�㷨
        LoopedFrameBuffer<T> rawTrendBuffer =
            m_field->data(element, 0, TrendDataRole).value<LoopedFrameBuffer<T> >();
        // ���ٳ�ȡ
        if (!rawTrendBuffer.isValid() || buffer.curElementCount() == 0)
        {
            // û������ֱ�Ӳ���ȡ����
            return true;
        }
        if (buffer.curElementCount() <= 0)
        {
            qDebug() << "Check the Buffer!!, buffer.curElementCount() <= 0";
            return false;
        }
        int framsize = buffer.curElementCount();

        // ��ȡÿ���ļ�(ÿ֡)��ȡ�����Ĳ���ֵ
        float step = 1 / m_extractRatio;
        // �����ٽ��������ƫ�Ƽ�¼offset, �������ݵ��Ż�����iterCount(��¼ѭ������)
        float offset = 0;
        int iterCount = 0;
        T temData = m_contextData[element];

        // ��ȡ���ļ���buffer����֡��ָ��
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

            // ��ȡ��ĳ�ȡ����������־
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

            // ����ȡ�ĵ��trendBuffer
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

    T     m_contextData[MAXCHANNELCOUNT];              //!< �����ļ��ٽ�����ĳ�ȡ��

    float m_contextOffset[MAXCHANNELCOUNT];            //!< �����ļ��ٽ�����ĳ�ȡƫ����

    float m_extractRatio;                                           //!< ÿ�ζ�ȡ���������ļ��ĳ�ȡ����
    float m_curCountRatio;                                          //!< ��ǰ��ȡ��������

    LoopedFrameBuffer<T>
        tempTrendBuf[MAXCHANNELCOUNT];                 //!< ��ȡTrend���ݵ���ʱ������
};