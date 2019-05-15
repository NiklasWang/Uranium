#include <stdlib.h>
#include <string.h>
#include "clist.h"
#include "CQueue.h"
#include "logs.h"

namespace sirius {

CQueue::CQueue()
{
    pthread_mutex_init(&m_lock, NULL);
    clist_init(&m_head.list);
    m_size = 0;
    m_dataFn = NULL;
    m_userData = NULL;
    m_active = true;
}

CQueue::CQueue(release_data_fn_c data_rel_fn, void *user_data)
{
    pthread_mutex_init(&m_lock, NULL);
    clist_init(&m_head.list);
    m_size = 0;
    m_dataFn = data_rel_fn;
    m_userData = user_data;
    m_active = true;
}

CQueue::~CQueue()
{
    flush();
    pthread_mutex_destroy(&m_lock);
}

int CQueue::getCurrentSize()
{
    return m_size;
}

int CQueue::size()
{
    return getCurrentSize();
}

void CQueue::init()
{
    pthread_mutex_lock(&m_lock);
    m_active = true;
    pthread_mutex_unlock(&m_lock);
}

bool CQueue::isEmpty()
{
    bool flag = true;
    pthread_mutex_lock(&m_lock);
    if (m_size > 0) {
        flag = false;
    }
    pthread_mutex_unlock(&m_lock);
    return flag;
}

bool CQueue::enqueue(void *data)
{
    bool rc;
    q_node *node =
        (q_node *)malloc(sizeof(q_node));
    if (NULL == node) {
        LOGE(MODULE_UTILS, "No memory for q_node");
        return false;
    }

    memset(node, 0, sizeof(q_node));
    node->data = data;

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        clist_add_tail_node(&node->list, &m_head.list);
        m_size++;
        rc = true;
    } else {
        free(node);
        rc = false;
    }
    pthread_mutex_unlock(&m_lock);
    return rc;
}

bool CQueue::enqueueWithPriority(void *data)
{
    bool rc;
    q_node *node =
        (q_node *)malloc(sizeof(q_node));
    if (NULL == node) {
        LOGE(MODULE_UTILS, "No memory for q_node");
        return false;
    }

    memset(node, 0, sizeof(q_node));
    node->data = data;

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        struct clist *p_next = m_head.list.next;

        m_head.list.next = &node->list;
        p_next->prev = &node->list;
        node->list.next = p_next;
        node->list.prev = &m_head.list;

        m_size++;
        rc = true;
    } else {
        free(node);
        rc = false;
    }
    pthread_mutex_unlock(&m_lock);
    return rc;
}

void* CQueue::peek()
{
    q_node* node = NULL;
    void* data = NULL;
    struct clist *head = NULL;
    struct clist *pos = NULL;

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        head = &m_head.list;
        pos = head->next;
        if (pos != head) {
            node = member_of(pos, q_node, list);
        }
    }
    pthread_mutex_unlock(&m_lock);

    if (NULL != node) {
        data = node->data;
    }

    return data;
}

void* CQueue::dequeue(bool bFromHead)
{
    q_node* node = NULL;
    void* data = NULL;
    struct clist *head = NULL;
    struct clist *pos = NULL;

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        head = &m_head.list;
        if (bFromHead) {
            pos = head->next;
        } else {
            pos = head->prev;
        }
        if (pos != head) {
            node = member_of(pos, q_node, list);
            clist_del_node(&node->list);
            m_size--;
        }
    }
    pthread_mutex_unlock(&m_lock);

    if (NULL != node) {
        data = node->data;
        free(node);
    }

    return data;
}

void* CQueue::dequeue(match_fn_data_c match, void *match_data)
{
    q_node* node = NULL;
    struct clist *head = NULL;
    struct clist *pos = NULL;
    void* data = NULL;

    if ( NULL == match || NULL == match_data ) {
        return NULL;
    }

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        head = &m_head.list;
        pos = head->next;

        while(pos != head) {
            node = member_of(pos, q_node, list);
            pos = pos->next;
            if (NULL != node) {
                if ( match(node->data, m_userData, match_data) ) {
                    clist_del_node(&node->list);
                    m_size--;
                    data = node->data;
                    free(node);
                    pthread_mutex_unlock(&m_lock);
                    return data;
                }
            }
        }
    }
    pthread_mutex_unlock(&m_lock);
    return NULL;
}

void CQueue::flush()
{
    q_node* node = NULL;
    struct clist *head = NULL;
    struct clist *pos = NULL;

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        head = &m_head.list;
        pos = head->next;

        while(pos != head) {
            node = member_of(pos, q_node, list);
            pos = pos->next;
            clist_del_node(&node->list);
            m_size--;

            if (NULL != node->data) {
                if (m_dataFn) {
                    m_dataFn(node->data, m_userData);
                }
                free(node->data);
            }
            free(node);

        }
        m_size = 0;
        m_active = false;
    }
    pthread_mutex_unlock(&m_lock);
}

void CQueue::flushNodes(match_fn_c match)
{
    q_node* node = NULL;
    struct clist *head = NULL;
    struct clist *pos = NULL;

    if ( NULL == match ) {
        return;
    }

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        head = &m_head.list;
        pos = head->next;

        while(pos != head) {
            node = member_of(pos, q_node, list);
            pos = pos->next;
            if ( match(node->data, m_userData) ) {
                clist_del_node(&node->list);
                m_size--;

                if (NULL != node->data) {
                    if (m_dataFn) {
                        m_dataFn(node->data, m_userData);
                    }
                    free(node->data);
                }
                free(node);
            }
        }
    }
    pthread_mutex_unlock(&m_lock);
}

void CQueue::flushNodes(match_fn_data_c match, void *match_data)
{
    q_node* node = NULL;
    struct clist *head = NULL;
    struct clist *pos = NULL;

    if ( NULL == match ) {
        return;
    }

    pthread_mutex_lock(&m_lock);
    if (m_active) {
        head = &m_head.list;
        pos = head->next;

        while(pos != head) {
            node = member_of(pos, q_node, list);
            pos = pos->next;
            if ( match(node->data, m_userData, match_data) ) {
                clist_del_node(&node->list);
                m_size--;

                if (NULL != node->data) {
                    if (m_dataFn) {
                        m_dataFn(node->data, m_userData);
                    }
                    free(node->data);
                }
                free(node);
            }
        }
    }
    pthread_mutex_unlock(&m_lock);
}

};

