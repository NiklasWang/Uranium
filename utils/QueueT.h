#ifndef _QUEUET_H__
#define _QUEUET_H__

#include <pthread.h>
#include "clist.h"

namespace sirius {

template <typename T>
using match_fn_data = bool (*)(T *data, void *user_data, T *match_data);

template <typename T>
using release_data_fn = void (*)(T* data, void *user_data);

template <typename T>
using match_fn = bool (*)(T *data, void *user_data);

template <typename T>
class QueueT {
public:
    QueueT();
    QueueT(release_data_fn<T> data_rel_fn, void *user_data);
    virtual ~QueueT();
    void init();
    bool enqueue(T *data);
    bool enqueueWithPriority(T *data);
    /* This call will put queue into uninitialized state.
     * Need to call init() in order to use the queue again */
    void flush();
    void flushNodes(match_fn<T> match);
    void flushNodes(match_fn_data<T> match, T *spec_data);
    T* dequeue(bool bFromHead = true);
    T* dequeue(match_fn_data<T> match, T *spec_data);
    T* peek();
    bool isEmpty();
    int size();
    int getCurrentSize();

private:
    typedef struct {
        struct clist list;
        T* data;
    } q_node;

    q_node m_head; // dummy head
    int    m_size;
    bool   m_active;
    pthread_mutex_t    m_lock;
    release_data_fn<T> m_dataFn;
    void  *m_userData;
};

};

#include "QueueT.hpp"

#endif

