/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  LoopedFrameBuffer.h
** Latest Version:             V1.0.0
** Latest modified Date:       2015/07/15
** Modified by:                
** Descriptions:               环形帧缓冲区
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2015/07/15
** Descriptions:               
** 
*********************************************************************************************************/
#pragma once

#include <QSharedData>
#include <QMutex>
#include <QtDebug>

/**
 *  @macro DISABLE_WRITE_ON_OVERFLOW
 *  @brief 在缓冲区溢出之后是否允许继续写入数据，并自动覆盖最旧的数据
 */
#define DISABLE_WRITE_ON_OVERFLOW           0

/**
 *  @class Frame
 *  @brief 环形帧缓冲区的数据帧
 */
template<class T>
class Frame
{
public:
    T*          m_buffer;                   //!< 当前数据帧的元素缓冲区
    int         m_count;                    //!< 当前数据帧的缓冲区中的元素个数（数组长度）
    Frame*      m_next;                     //!< 下一个数据帧的指针
    Frame*      m_prev;                     //!< 上一个数据帧的指针

    /*! 
     * 创建一个数据帧，为其分配内存并链接到帧链表
     * @param[in]  elementCount 当前数据帧的元素缓冲区中的元素个数（数组长度）
     * @param[in]  next         下一个数据帧的指针
     * @param[in]  prev         上一个数据帧的指针
     * @return     创建的数据帧对象，如果创建失败（内存不足）则返回NULL
     */
    static Frame* create(int elementCount, Frame* next = NULL, Frame* prev = NULL)
    {
        // 此处应考虑内存对齐问题：使用Frame和T类型中较大的一个作为对齐标准;
        //int n = (sizeof(Frame) + sizeof(T) - 1) / sizeof(T);
        
        Frame* p = new Frame;
        if (p)
        {
            p->m_buffer = new T[elementCount];//(T*)((char*)p + sizeof(Frame));
            p->m_count  = elementCount;
            p->m_next   = next;
            p->m_prev   = prev;

            if (next) 
            {
                next->m_prev = p;
            }
            if (prev) 
            {
                prev->m_next = p;
            }
        }

        return p;
    }

    /*! 
     * 销毁一个数据帧
     * @param[in]  frame 待销毁的数据帧
     */
    static void destroy(Frame* frame)
    {
        if (frame) 
        {
            delete[] frame->m_buffer;
            frame->m_buffer = NULL;

            delete   frame;
        }
    }

private:
    // Call create & destroy methods instead of constructor and destructor
    Frame() {}
    ~Frame() {}
};

/**
 *  @class FrameCursor
 *  @brief 环形帧缓冲区的数据帧游标，用于指示某个迭代器的位置，实际上就是迭代器的基类
 */
template<class T>
class FrameCursor
{
public:
    Frame<T>*   m_frame;                    //!< 游标当前所在的数据帧
    int         m_offset;                   //!< 游标在当前数据帧中的偏移量

public:
    FrameCursor() : m_frame(NULL), m_offset(0) {}

    /**
     *  operator++: ++a
     */
    inline FrameCursor& operator++()
    {
        if (++m_offset >= m_frame->m_count)
        {
            m_frame  = m_frame->m_next;
            m_offset = 0;
        }

        return *this;
    }

    /**
     *  operator++: a++
     */
    inline const FrameCursor operator++(int)
    {
        // TODO: 支持*p++混合操作
        FrameCursor old = *this;
        ++(*this);
        return old;
    }

    /**
     *  operator--: --a
     */
    inline FrameCursor& operator--()
    {
        if (--m_offset < 0)
        {
            m_frame  = m_frame->m_prev;
            m_offset = m_frame->m_count - 1;
        }

        return *this;
    }

    /**
     *  operator--: a--
     */
    inline const FrameCursor operator--(int)
    {
        // TODO: 支持*p--混合操作
        FrameCursor old = *this;
        --(*this);
        return old;
    }

    /**
     *  operator*: *a
     */
    inline T& operator*()
    {
        return m_frame->m_buffer[m_offset];
    }

    /**
     *  operator+: a+step
     */
    inline friend FrameCursor operator+(const FrameCursor& a, int step)
    {
        FrameCursor result = a;
        int count = result.m_frame->m_count;
        while (step > count)
        {
            result.m_frame  = result.m_frame->m_next;
            step      -= count;
        }
        if ((result.m_offset += step) >= count)
        {
            result.m_frame   = result.m_frame->m_next;
            result.m_offset -= count;
        }

        return result;
    }

    /**
     *  operator+=: a+=step
     */
    inline friend FrameCursor& operator+=(FrameCursor& a, int step)
    {
        return (a = a + step);
    }

    /**
     *  operator-: a-step
     */
    inline friend FrameCursor operator-(const FrameCursor& a, int step)
    {
        FrameCursor result = a;
        int count = result.m_frame->m_count;
        while (step > count)
        {
            result.m_frame  = result.m_frame->m_prev;
            step      -= count;
        }
        if ((result.m_offset -= step) < 0)
        {
            result.m_frame   = result.m_frame->m_prev;
            result.m_offset += count;
        }

        return result;
    }

    /**
     *  operator-=: a-=step
     */
    inline friend FrameCursor& operator-=(FrameCursor& a, int step)
    {
        return (a = a - step);
    }

    /*! 
     * operator==: a==b
     * @return     a==b ? true : false
     */
    inline friend bool operator==(const FrameCursor& a, const FrameCursor& b)
    {
        return (a.m_frame == b.m_frame && a.m_offset == b.m_offset);
    }

    /*! 
     * operator==: a!=b
     * @return     a!=b ? true : false
     */
    inline friend bool operator!=(const FrameCursor& a, const FrameCursor& b)
    {
        return (a.m_frame != b.m_frame || a.m_offset != b.m_offset);
    }

    /*! 
     * 将输入缓冲区中的数据写入当前的FrameCursor所指向的frame
     * @param[in]  data   输入缓冲区基地址
     * @param[in]  length 输入缓冲区长度（元素个数）
     * @return     操作成功返回已写入的长度，否则返回非正整数
     */
    int write(const T* data, int length)
    {
        int len   = length;
        int count = m_frame->m_count;
        int chunk = count - m_offset;

        if (len < chunk)
        {
            if (data)
                memcpy(&m_frame->m_buffer[m_offset], data, len * sizeof(T));
            m_offset += len;
            return len;
        }

        // 首先将首个未填满的帧填满
        if (data)
        {
            memcpy(&m_frame->m_buffer[m_offset], data, chunk * sizeof(T));
        }
        m_frame = m_frame->m_next;
        m_offset = 0;
        len -= chunk;
        data += chunk;

        while (len > count)
        {
            if (data)
            {
                memcpy(m_frame->m_buffer, data, count * sizeof(T));
            }
            m_frame  = m_frame->m_next;
            len     -= count;
            data    += count;
        }
        if (len > 0)
        {
            if (data)
            {
                memcpy(m_frame->m_buffer, data, len * sizeof(T));
            }
            m_offset += len;
        }


        return length;
    }

    /*! 
     * 将当前的FrameCursor所指向的frame处的数据读出至输出缓冲区中
     * @param[out] data   输出缓冲区基地址
     * @param[in]  length 输出缓冲区长度（元素个数）
     * @return     操作成功返回已读取的长度，否则返回非正整数
     */
    int read(T* data, int length)
    {
        int len   = length;
        int count = m_frame->m_count;
        int chunk = count - m_offset;

        if (len < chunk)
        {
            memcpy(data, &m_frame->m_buffer[m_offset], len * sizeof(T));
            m_offset += len;
            return len;
        }

        memcpy(data, &m_frame->m_buffer[m_offset], chunk * sizeof(T));
        m_frame = m_frame->m_next;
        m_offset = 0;
        len -= chunk;
        data += chunk;

        while (len > count)
        {
            memcpy(data, m_frame->m_buffer, count * sizeof(T));
            m_frame  = m_frame->m_next;
            len     -= count;
            data    += count;
        }
        if (len > 0)
        {
            memcpy(data, m_frame->m_buffer, len * sizeof(T));
            m_offset += len;
        }


        return this->length;
    }

    /*! 
     * 复位游标至指定的帧的起始位置，若未指定帧，则复位游标为空（NULL）
     * @param[in]  pos 指定的帧
     */
    void reset(Frame<T>* pos = NULL)
    {
        m_frame = pos;
        m_offset = 0;
    }
};

/**
 *  @class LoopedFrameBufferData
 *  @brief 环形帧缓冲区包含的的显示共享数据
 */
template<class T>
class LoopedFrameBufferData : public QSharedData
{
public:
    LoopedFrameBufferData(void) : 
        m_frameList(NULL), 
        m_frameSize(0),
        m_frameCount(0),
        m_curElementCount(0),
        m_totalElementCount(0),
        m_overflow(0)
    {

    }

    /*! 
     * 带参构造函数
     * @param[in]  frameSize  缓冲区内部每个数据帧的元素个数
     * @param[in]  frameCount 缓冲区内部数据帧的个数
     */
    LoopedFrameBufferData(int frameSize, int frameCount = 3)
    {
        Frame<T>* ptr = NULL;

        if (frameCount <= 0) return;

        m_frameList = ptr = Frame<T>::create(frameSize);
        for (int i = 1; i < frameCount; i++)
        {
            ptr = Frame<T>::create(frameSize, NULL, ptr);
        }
        ptr->m_next  = m_frameList;
        m_frameList->m_prev = ptr;

        m_frameSize  = frameSize;
        m_frameCount = frameCount;
        m_curElementCount = 0;
        m_totalElementCount = frameSize * frameCount;
        m_overflow = false;

        m_writeCursor.reset(m_frameList);
        m_readCursor.reset(m_frameList);
    }

    LoopedFrameBufferData(const LoopedFrameBufferData& other) : QSharedData(other), 
        m_frameList(other.m_frameList),
        m_frameSize(other.m_frameSize),
        m_frameCount(other.m_frameCount),
        m_curElementCount(other.m_curElementCount),
        m_totalElementCount(other.m_totalElementCount),
        m_overflow(other.m_overflow),
        m_writeCursor(other.m_writeCursor),
        m_readCursor(other.m_readCursor)
    {

    }

    ~LoopedFrameBufferData(void) 
    {
        if (m_frameList)
        {
            Frame<T>* i = m_frameList;

            do 
            {
                Frame<T>* j = i;
                i = i->m_next;
                Frame<T>::destroy(j);
            } while (i != m_frameList);
        }

        m_frameList = NULL;
        m_frameSize = 0;
        m_frameCount = 0;
        m_curElementCount = 0;
        m_totalElementCount = 0;
        m_overflow = false;
    }

    Frame<T>*       m_frameList;            //!< 帧链表首个节点
    int             m_frameSize;            //!< 每个帧的元素总个数
    int             m_frameCount;           //!< 当前缓冲区的总帧数
    int             m_curElementCount;      //!< 当前有效元素个数
    int             m_totalElementCount;    //!< 缓冲区中能够存储的元素总个数（缓冲区容量）
    bool            m_overflow;             //!< 缓冲区是否已经溢出
    
    FrameCursor<T>  m_writeCursor;          //!< 缓冲区内部记录的写游标（写迭代器初始值）
    FrameCursor<T>  m_readCursor;           //!< 缓冲区内部记录的读游标（读迭代器初始值）

    QMutex          m_locker;               //!< 用于多线程安全操作的互斥锁
};

/**
 *  @class LoopedFrameBuffer
 *  @brief 环形帧缓冲区包含的的显示共享数据
 */
template<class T>
class LoopedFrameBuffer
{
protected:
    QExplicitlySharedDataPointer< LoopedFrameBufferData<T> > d;
                                            //!< 显示共享的缓冲区内部数据

public:
    // TODO: 隐藏const_iterator的写相关操作，使其变成真正意义上的“只读”迭代器
    typedef FrameCursor<T> const_iterator;  //!< 只读迭代器
    typedef FrameCursor<T> iterator;        //!< 读写迭代器
    typedef const T&       const_reference; //!< 只读引用

public:
    LoopedFrameBuffer(void) : d(NULL) 
    {
        int breakpoint = 0;
    }

    LoopedFrameBuffer(const LoopedFrameBuffer& other) : d(other.d) {}

    ~LoopedFrameBuffer(void) {}

    /*! 
     * 带参构造函数
     * @param[in]  frameSize  缓冲区内部每个数据帧的元素个数
     * @param[in]  frameCount 缓冲区内部数据帧的个数
     */
    LoopedFrameBuffer(int frameSize, int frameCount = 3)
    {
        d = new LoopedFrameBufferData<T>(frameSize, frameCount);
    }

    /*! 
     * 判断当前缓冲区是否有效（是否已经初始化并分配内存）
     */
    inline bool isValid() const
    {
        return d ? (d->m_frameSize > 0 && d->m_frameCount > 0) : false;
    }


    /*! 
     * 获取每个帧的元素总个数
     * @return     每个帧的元素总个数
     */
    inline int frameSize() const
    {
        return d ? d->m_frameSize : 0;
    }

    /*! 
     * 获取当前缓冲区的总帧数
     * @return     当前缓冲区的总帧数
     */
    inline int frameCount() const
    {
        return d ? d->m_frameCount : 0;
    }

    /*! 
     * 获取当前有效元素个数
     * @return     当前有效元素个数
     */
    inline int curElementCount() const
    {
        int count = 0;
        if (d)
        {
            d->m_locker.lock();
            count = d->m_curElementCount;
            d->m_locker.unlock();
        }
        return count;
    }

    /*! 
     * 判断当前的环形缓冲区队列是否为空
     * @return     当前的环形缓冲区队列是否为空
     */
    inline bool isEmpty() const
    {
        bool empty = true;
        if (d)
        {
            d->m_locker.lock();
            empty = d->m_curElementCount == 0;
            d->m_locker.unlock();
        }
        return empty;
    }

    /*! 
     * 判断当前的环形缓冲区队列是否为满
     * @return     当前的环形缓冲区队列是否为满
     */
    inline bool isFull() const
    {
        bool full = false;
        if (d)
        {
            d->m_locker.lock();
            full = d->m_curElementCount == d->m_totalElementCount;
            d->m_locker.unlock();
        }
        return full;
    }

    /*! 
     * 判断当前的环形缓冲区队列是否已经溢出（溢出是指已经丢失数据）
     * @return     当前的环形缓冲区队列是否已经溢出
     */
    inline bool isOverflow() const
    {
        bool overflow = false;
        if (d)
        {
            d->m_locker.lock();
            overflow = d->m_overflow;
            d->m_locker.unlock();
        }
        return overflow;
    }

    /*! 
     * 重置缓冲区中的全部内容，但是不会重新分配缓冲区，通常用于overflow之后的复位
     */
    void reset()
    {
        d->m_locker.lock();
        d->m_curElementCount = 0;
        d->m_overflow = false;
        d->m_writeCursor.reset(d->m_frameList);
        d->m_readCursor.reset(d->m_frameList);
        d->m_locker.unlock();
    }

    /*! 
     * 按照新的参数，重新分配当前的帧缓冲区链表
     * @param[in]  frameSize  缓冲区内部每个数据帧的元素个数
     * @param[in]  frameCount 缓冲区内部数据帧的个数
     */
    void resize(int frameSize, int frameCount = 3)
    {
        if (frameSize <= 0)
        {
            d = NULL;
        }
        // QSharedData will be destroyed automatically
        //delete d;
        else if (!d || d->m_frameCount != frameCount || d->m_frameSize != frameSize)
        {
            d = new LoopedFrameBufferData<T>(frameSize, frameCount);
        }
    }

    /*! 
     * 清空缓冲区，释放内存，但仍可重新通过resize分配
     */
    void clear()
    {
        resize(0, 0);
    }

    /*! 
     * 获取可读区域的起始位置的读写迭代器
     * @return     对应本函数功能的迭代器
     */
    inline iterator begin(void)
    {
        iterator iter;
        d->m_locker.lock();
        iter = d->m_readCursor;
        d->m_locker.unlock();
        return iter;
    }

    /*! 
     * 获取可读区域的起始位置的只读迭代器，
     * 用于从头开始读取“可读数据”（生产者已生产但尚未被消费的数据）
     * @return     对应本函数功能的迭代器
     */
    inline const_iterator cbegin(void) const
    {
        const_iterator iter;
        d->m_locker.lock();
        iter = d->m_readCursor;
        d->m_locker.unlock();
        return iter;
    }

    /*! 
     * 获取可写区域的末尾位置的读写迭代器，
     * 用于从末尾开始追加新的“可读数据”
     * @return     对应本函数功能的迭代器
     */
    inline iterator end(void)
    {
        iterator iter;
        d->m_locker.lock();
        iter = d->m_writeCursor;
        d->m_locker.unlock();
        return iter;
    }

    /*! 
     * 获取可读区域的末尾位置的只读迭代器，
     * 用于从末尾开始反向遍历读取“可读数据”（生产者已生产但尚未被消费的数据）
     * @return     对应本函数功能的迭代器
     */
    inline const_iterator cend(void) const
    {
        const_iterator iter;
        d->m_locker.lock();
        iter = d->m_writeCursor;
        d->m_locker.unlock();
        return iter;
    }

    /*! 
     * 获取首个可读元素的常量引用
     * @return     首个可读元素的常量引用
     * @note       注意这种方法虽然访问比较快，但可能是非线程安全的
     */
    inline const_reference front() const
    {
        return const_cast<const_reference>(*d->m_readCursor);
    }

    /*! 
     * 获取首个可读元素的读写引用
     * @return     首个可读元素的读写引用
     * @note       注意这种方法虽然访问比较快，但可能是非线程安全的
     */
    inline T& front()
    {
        return *d->m_readCursor;
    }

    /*! 
     * 获取首个可读帧的读写引用
     * @return     首个可读帧的读写引用
     */
    inline T* frontFrame()
    {
        T* frame = NULL;
        d->m_locker.lock();
        frame = d->m_readCursor.m_frame->m_buffer;
        d->m_locker.unlock();
        return frame;
    }

    /*! 
     *  设置当前有效点数，只改变游标不改变数据内容(主要用于内存数据拷贝方式改变数据后的游标复位)
     * @param [in] validNum  当前有效数据的个数(小于一帧的个数)  
     * @return     
     */
    void setValidElementCount(int validNum)
    {
        d->m_locker.lock();
        d->m_curElementCount = validNum;
        if (d->m_curElementCount > d->m_totalElementCount)
        {
            if (!d->m_overflow)
                qWarning("LoopedFrameBuffer overflow @pushElement, depth=%d.\n", d->m_totalElementCount);
            d->m_curElementCount = d->m_totalElementCount;
            d->m_overflow = true;
        }
        d->m_writeCursor.m_offset = d->m_curElementCount;
        
        d->m_locker.unlock();
    }

    /*! 
     * 在可读区的末尾追加一个新的元素
     * @param[in]  element 追加的元素
     */
    void pushElement(const T& element)
    {
        d->m_locker.lock();
        #if 1
        if (++d->m_curElementCount > d->m_totalElementCount)
        {
            if (!d->m_overflow)
            {
                qWarning("LoopedFrameBuffer overflow @pushElement, depth=%d.\n", d->m_totalElementCount);
            }
            d->m_curElementCount = d->m_totalElementCount;
            d->m_overflow = true;
        }
#if DISABLE_WRITE_ON_OVERFLOW
        else 
#endif
        {
            *d->m_writeCursor = element;
            ++d->m_writeCursor;
        }
        #endif
        d->m_locker.unlock();
    }

    /*! 
     * 在可读区的头部弹出一个元素
     * @return        弹出的元素
     */
    T popElement()
    {
        T data;

        d->m_locker.lock();
        Q_ASSERT(d->m_curElementCount > 0);
        data = *d->m_readCursor;
        --d->m_curElementCount;
        ++d->m_readCursor;
        d->m_locker.unlock();

        return data;
    }

    /*! 
     * 在可读区的末尾追加一个新的帧
     * @param[in]  element 追加的帧
     */
    void pushFrame(const T* data)
    {
        d->m_locker.lock();
        d->m_curElementCount += d->m_frameSize;
        if (d->m_curElementCount > d->m_totalElementCount)
        {
            if (!d->m_overflow)
                qWarning("LoopedFrameBuffer overflow @pushFrame, depth=%d.\n", d->m_totalElementCount);
            d->m_curElementCount = d->m_totalElementCount;
            d->m_overflow = true;
        }
#if DISABLE_WRITE_ON_OVERFLOW
        else 
#endif
        {
            d->m_writeCursor.write(data, d->m_frameSize);
        }
        d->m_locker.unlock();
    }

    /*! 
     * 在可读区的头部弹出一个帧
     * @return        弹出的帧
     */
    T* popFrame(void)
    {
        T* data;

        d->m_locker.lock();
        Q_ASSERT(d->m_curElementCount >= d->m_frameSize);
        data = d->m_readCursor.m_frame->m_buffer;
        d->m_curElementCount -= d->m_frameSize;
        d->m_readCursor += d->m_frameSize;
        d->m_locker.unlock();

        return data;
    }

    /*! 
     * 经过优化的迭代算法
     * @param[in]  function     迭代算法
     * @param[in]  iterateCount 迭代次数
     * @param[in]  arg          迭代时传入的自定义参数，例如一个上下文
     * @return     实际迭代次数，可能小于指定的迭代次数
     */
    template <typename iterateFunction>
    int iterate(iterateFunction function, int iterateCount, void* arg = 0)
    {
        int count = 0;
        int size  = 0;

        d->m_locker.lock();
        
        if (iterateCount > d->m_curElementCount)
            iterateCount = d->m_curElementCount;
        size  = d->m_frameSize;
        count = iterateCount;
        
        Frame<T>* frame  = d->m_readCursor.m_frame;
        int    offset    = d->m_readCursor.m_offset;
        T*     iter      = frame->m_buffer + offset;
        while (count--)
        {
            function(iter++, arg);
            if (++offset >= size)
            {
                frame  = frame->m_next;
                iter   = frame->m_buffer;
                offset = 0;
            }
        }

        d->m_locker.unlock();

        return iterateCount - count - 1;
    }

    /*! 
     * 经过优化的反向迭代算法
     * @param[in]  function     反向迭代算法
     * @param[in]  iterateCount 迭代次数
     * @param[in]  arg          迭代时传入的自定义参数，例如一个上下文
     * @return     实际迭代次数，可能小于指定的迭代次数
     */
    template <typename iterateFunction>
    int reverseIterate(iterateFunction function, int iterateCount, void* arg = 0)
    {
        int count = 0;
        int size  = 0;

        d->m_locker.lock();
        
        if (iterateCount > d->m_curElementCount)
        {
            iterateCount = d->m_curElementCount;
        }
        size  = d->m_frameSize;
        count = iterateCount;
        
        Frame<T>* frame  = d->m_writeCursor.m_frame;
        int    offset    = d->m_writeCursor.m_offset;
        T*     iter      = frame->m_buffer + offset;
        while (count--)
        {
            if (--offset < 0)
            {
                frame  = frame->m_prev;
                iter   = frame->m_buffer + size;
                offset = size - 1;
            }
            function(--iter, arg);
        }

        d->m_locker.unlock();

        return iterateCount - count - 1;
    }
};

class Stat
{
public:
    float max;
    float min;
    float avg;
    float reserved;
};

struct Range
{
public:
    float max;
    float min;
};

// 注册进元对象系统，方便使用QVariant进行转换

Q_DECLARE_METATYPE(LoopedFrameBuffer<Stat>)
Q_DECLARE_METATYPE(LoopedFrameBuffer<Range>)

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
