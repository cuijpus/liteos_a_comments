/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup los_list Doubly linked list
 * @ingroup kernel
 */

#ifndef _LOS_LIST_H
#define _LOS_LIST_H

#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * 这里的双向链表，参考了Linux Kernel的双向链表。
 但是 Linux 内核中的链表实现并没有这样做。所以问题来了：链表在哪里保存数据呢？
 实际上，内核里实现的链表是侵入式链表（Intrusive list）。
 侵入式链表并不在节点内保存数据-它的节点仅仅包含指向前后节点的指针，
 以及指向链表节点数据部分的指针——数据就是这样附加在链表上的。
 这就使得这个数据结构是通用的，使用起来就不需要考虑节点数据的类型了。


 在linux内核中，有大量的数据结构需要用到双循环链表，例如进程、文件、模块、页面等。若采用双循环链表的传统实现方式，
 需要为这些数据结构维护各自的链表，并且为每个链表都要设计插入、删除等操作函数。
 因为用来维持链表的next和prev指针指向对应类型的对象，因此一种数据结构的链表操作函数不能用于操作其它数据结构的链表。


 在Linux源代码树的include/linux/list.h文件中，采用了一种类型无关的双循环链表实现方式。
 
 其思想是将指针prev和next从具体的数据结构中提取出来构成一种通用的"双链表"数据结构list_head。
 
 如果需要构造某类对象的特定链表，则在其结构（被称为宿主数据结构）中定义一个类型为list_head类型的成员，
 通过这个成员将这类对象连接起来，形成所需链表，并通过通用链表函数对其进行操作。
 
 其优点是只需编写通用链表函数，即可构造和操作不同对象的链表，而无需为每类对象的每种列表编写专用函数，实现了代码的重用。
 * https://y3v299dwqw.feishu.cn/docx/U03UdISSfoZH72xRqkdcZeTsnoe 
 */


/**
 * @ingroup los_list
 * Structure of a node in a doubly linked list.
 */
typedef struct LOS_DL_LIST {
    struct LOS_DL_LIST *pstPrev; /**< Current node's pointer to the previous node */
    struct LOS_DL_LIST *pstNext; /**< Current node's pointer to the next node */
} LOS_DL_LIST;

/**
 * @ingroup los_list
 *
 * @par Description:
 * This API is used to initialize a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameter passed in should be ensured to be a legal pointer.</li>
 * </ul>
 *
 * @param list    [IN] Node in a doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListInit(LOS_DL_LIST *list)
{
    list->pstNext = list;
    list->pstPrev = list;
}

/**
 * @ingroup los_list
 * @brief Point to the next node pointed to by the current node.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to point to the next node pointed to by the current node.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param object  [IN] Node in the doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_FIRST(object) ((object)->pstNext)

/**
 * @ingroup los_list
 * @brief Node is the end of the list.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to test node is the end of the list.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param list  [IN] Doubly linked list.
 * @param node  [IN] The node to be tested if the end of the list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_IS_END(list, node) ((list) == (node) ? TRUE : FALSE)

/**
 * @ingroup los_list
 * @brief Node is on the list.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to test node is on the list.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param object  [IN] Node in the doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_IS_ON_QUEUE(node) ((node)->pstPrev != NULL && (node)->pstNext != NULL)

/**
 * @ingroup los_list
 * @brief Point to the previous node pointed to by the current node.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to point to the previous node pointed to by the current node.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param object  [IN] Node in the doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_LAST(object) ((object)->pstPrev)

/**
 * @ingroup los_list
 * @brief Insert a new node to a doubly linked list.
 *
 * @par Description:
 * This API is used to insert a new node to a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameters passed in should be ensured to be legal pointers.</li>
 * </ul>
 *
 * @param list    [IN] Doubly linked list where the new node is inserted.
 * @param node    [IN] New node to be inserted.
 *
 * @retval None
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListDelete
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListAdd(LOS_DL_LIST *list, LOS_DL_LIST *node)
{
    node->pstNext = list->pstNext;
    node->pstPrev = list;
    list->pstNext->pstPrev = node;
    list->pstNext = node;
}

/**
 * @ingroup los_list
 * @brief Insert a node to the tail of a doubly linked list.
 *
 * @par Description:
 * This API is used to insert a new node to the tail of a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameters passed in should be ensured to be legal pointers.</li>
 * </ul>
 *
 * @param list     [IN] Doubly linked list where the new node is inserted.
 * @param node     [IN] New node to be inserted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListAdd | LOS_ListHeadInsert
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListTailInsert(LOS_DL_LIST *list, LOS_DL_LIST *node)
{
    LOS_ListAdd(list->pstPrev, node);
}

/**
 * @ingroup los_list
 * @brief Insert a node to the head of a doubly linked list.
 *
 * @par Description:
 * This API is used to insert a new node to the head of a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameters passed in should be ensured to be legal pointers.</li>
 * </ul>
 *
 * @param list     [IN] Doubly linked list where the new node is inserted.
 * @param node     [IN] New node to be inserted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListAdd | LOS_ListTailInsert
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListHeadInsert(LOS_DL_LIST *list, LOS_DL_LIST *node)
{
    LOS_ListAdd(list, node);
}

/**
 * @ingroup los_list
 *
 * @par Description:
 * <ul>
 * <li>This API is used to delete a specified node from a doubly linked list.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The parameter passed in should be ensured to be a legal pointer.</li>
 * </ul>
 *
 * @param node    [IN] Node to be deleted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListAdd
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListDelete(LOS_DL_LIST *node)
{
    node->pstNext->pstPrev = node->pstPrev;
    node->pstPrev->pstNext = node->pstNext;
    node->pstNext = NULL;
    node->pstPrev = NULL;
}

/**
 * @ingroup los_list
 * @brief Identify whether a specified doubly linked list is empty.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to return whether a doubly linked list is empty.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The parameter passed in should be ensured to be a legal pointer.</li>
 * </ul>
 *
 * @param list  [IN] Doubly linked list.
 *
 * @retval TRUE The doubly linked list is empty.
 * @retval FALSE The doubly linked list is not empty.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE BOOL LOS_ListEmpty(LOS_DL_LIST *list)
{
    return (BOOL)(list->pstNext == list);
}

/**
 * @ingroup los_list
 * @brief Insert a new list to a doubly linked list.
 *
 * @par Description:
 * This API is used to insert a new list to a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameters passed in should be ensured to be legal pointers.</li>
 * </ul>
 *
 * @param oldList    [IN] Doubly linked list where the new list is inserted.
 * @param newList    [IN] New list to be inserted.
 *
 * @retval None
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListDelete
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListAddList(LOS_DL_LIST *oldList, LOS_DL_LIST *newList)
{
    LOS_DL_LIST *oldListHead = oldList->pstNext;
    LOS_DL_LIST *oldListTail = oldList;
    LOS_DL_LIST *newListHead = newList;
    LOS_DL_LIST *newListTail = newList->pstPrev;

    oldListTail->pstNext = newListHead;
    newListHead->pstPrev = oldListTail;
    oldListHead->pstPrev = newListTail;
    newListTail->pstNext = oldListHead;
}

/**
 * @ingroup los_list
 * @brief Insert a doubly list to the tail of a doubly linked list.
 *
 * @par Description:
 * This API is used to insert a new doubly list to the tail of a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameters passed in should be ensured to be legal pointers.</li>
 * </ul>
 *
 * @param oldList     [IN] Doubly linked list where the new list is inserted.
 * @param newList     [IN] New list to be inserted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListAddList | LOS_ListHeadInsertList
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListTailInsertList(LOS_DL_LIST *oldList, LOS_DL_LIST *newList)
{
    LOS_ListAddList(oldList->pstPrev, newList);
}

/**
 * @ingroup los_list
 * @brief Insert a doubly list to the head of a doubly linked list.
 *
 * @par Description:
 * This API is used to insert a new doubly list to the head of a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameters passed in should be ensured to be legal pointers.</li>
 * </ul>
 *
 * @param oldList     [IN] Doubly linked list where the new list is inserted.
 * @param newList     [IN] New list to be inserted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ListAddList | LOS_ListTailInsertList
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListHeadInsertList(LOS_DL_LIST *oldList, LOS_DL_LIST *newList)
{
    LOS_ListAddList(oldList, newList);
}

/**
 * @ingroup los_list
 * @brief Obtain the offset of a field to a structure address.
 *
 * @par  Description:
 * This API is used to obtain the offset of a field to a structure address.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param type    [IN] Structure name.
 * @param member  [IN] Name of the member of which the offset is to be measured.
 *
 * @retval Offset of the field to the structure address.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_OFF_SET_OF(type, member) ((UINTPTR)&((type *)0)->member)

/**
 * @ingroup los_list
 * @brief Obtain the pointer to a structure that contains a doubly linked list.
 *
 * @par Description:
 * This API is used to obtain the pointer to a structure that contains a doubly linked list.
 * <ul>
 * <li>None.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param item    [IN] Current node's pointer to the next node.
 * @param type    [IN] Structure name.
 * @param member  [IN] Member name of the doubly linked list in the structure.
 *
 * @retval Pointer to the structure that contains the doubly linked list.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_ENTRY(item, type, member) \
    ((type *)(VOID *)((CHAR *)(item) - LOS_OFF_SET_OF(type, member)))

/**
 * @ingroup los_list
 * @brief Iterate over a doubly linked list of given type.
 *
 * @par Description:
 * This API is used to iterate over a doubly linked list of given type.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param item           [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param list           [IN] Pointer to the doubly linked list to be traversed.
 * @param type           [IN] Structure name.
 * @param member         [IN] Member name of the doubly linked list in the structure.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_FOR_EACH_ENTRY(item, list, type, member)             \
    for (item = LOS_DL_LIST_ENTRY((list)->pstNext, type, member);        \
         &(item)->member != (list);                                      \
         item = LOS_DL_LIST_ENTRY((item)->member.pstNext, type, member))

/**
 * @ingroup los_list
 * @brief iterate over a doubly linked list safe against removal of list entry.
 *
 * @par Description:
 * This API is used to iterate over a doubly linked list safe against removal of list entry.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param item           [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param next           [IN] Save the next node.
 * @param list           [IN] Pointer to the doubly linked list to be traversed.
 * @param type           [IN] Structure name.
 * @param member         [IN] Member name of the doubly linked list in the structure.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_FOR_EACH_ENTRY_SAFE(item, next, list, type, member)               \
    for (item = LOS_DL_LIST_ENTRY((list)->pstNext, type, member),                     \
         next = LOS_DL_LIST_ENTRY((item)->member.pstNext, type, member);              \
         &(item)->member != (list);                                                   \
         item = next, next = LOS_DL_LIST_ENTRY((item)->member.pstNext, type, member))

/**
 * @ingroup los_list
 * @brief Delete initialize a doubly linked list.
 *
 * @par Description:
 * This API is used to delete initialize a doubly linked list.
 * @attention
 * <ul>
 * <li>The parameter passed in should be ensured to be s legal pointer.</li>
 * </ul>
 *
 * @param list    [IN] Doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_ListDelInit(LOS_DL_LIST *list)
{
    list->pstNext->pstPrev = list->pstPrev;
    list->pstPrev->pstNext = list->pstNext;
    LOS_ListInit(list);
}

/**
 * @ingroup los_list
 * @brief iterate over a doubly linked list.
 *
 * @par Description:
 * This API is used to iterate over a doubly linked list.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param item           [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param list           [IN] Pointer to the doubly linked list to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_FOR_EACH(item, list) \
    for (item = (list)->pstNext;         \
         (item) != (list);               \
         item = (item)->pstNext)

/**
 * @ingroup los_list
 * @brief Iterate over a doubly linked list safe against removal of list entry.
 *
 * @par Description:
 * This API is used to iterate over a doubly linked list safe against removal of list entry.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param item           [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param next           [IN] Save the next node.
 * @param list           [IN] Pointer to the doubly linked list to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_FOR_EACH_SAFE(item, next, list)      \
    for (item = (list)->pstNext, next = (item)->pstNext; \
         (item) != (list);                               \
         item = next, next = (item)->pstNext)

/**
 * @ingroup los_list
 * @brief Initialize a double linked list.
 *
 * @par Description:
 * This API is used to initialize a double linked list.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param list           [IN] Pointer to the doubly linked list to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_list.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_DL_LIST_HEAD(list) LOS_DL_LIST list = { &(list), &(list) }

#define LOS_ListPeekHeadType(list, type, element) ({             \
    type *__t;                                                   \
    if ((list)->pstNext == list) {                               \
        __t = NULL;                                              \
    } else {                                                     \
        __t = LOS_DL_LIST_ENTRY((list)->pstNext, type, element); \
    }                                                            \
    __t;                                                         \
})

#define LOS_ListRemoveHeadType(list, type, element) ({           \
    type *__t;                                                   \
    if ((list)->pstNext == list) {                               \
        __t = NULL;                                              \
    } else {                                                     \
        __t = LOS_DL_LIST_ENTRY((list)->pstNext, type, element); \
        LOS_ListDelete((list)->pstNext);                         \
    }                                                            \
    __t;                                                         \
})

#define LOS_ListNextType(list, item, type, element) ({           \
    type *__t;                                                   \
    if ((item)->pstNext == list) {                               \
        __t = NULL;                                              \
    } else {                                                     \
        __t = LOS_DL_LIST_ENTRY((item)->pstNext, type, element); \
    }                                                            \
    __t;                                                         \
})

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_LIST_H */
