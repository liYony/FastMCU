/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2023-2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-14     Bernard      the first version
 * 2006-04-21     Bernard      change the scheduler lock to interrupt lock
 * 2006-05-18     Bernard      fix the object init bug
 * 2006-08-03     Bernard      add hook support
 * 2007-01-28     Bernard      rename RT_OBJECT_Class_Static to RT_Object_Class_Static
 * 2010-10-26     yi.qiu       add module support in rt_object_allocate and rt_object_free
 * 2017-12-10     Bernard      Add object_info enum.
 * 2018-01-25     Bernard      Fix the object find issue when enable MODULE.
 * 2022-01-07     Gabriel      Moving __on_rt_xxxxx_hook to object.c
 * 2024-01-03     liYony       Adaptive FastMCU
 */

#include <fmcu.h>

/*
 * define object_info for the number of _object_container items.
 */
enum fm_object_info_type
{
    FM_Object_Info_Device = 0,                          /**< The object is a thread. */
    FM_Object_Info_Fsm,                                 /**< The object is a fsm */
    FM_Object_Info_Timer,                               /**< The object is a timer. */

    FM_Object_Info_Unknown,                             /**< The object is unknown. */
};

#define _OBJ_CONTAINER_LIST_INIT(c)     \
    {&(_object_container[c].object_list), &(_object_container[c].object_list)}

static struct fm_object_information _object_container[FM_Object_Info_Unknown] =
{
    {FM_Object_Class_Device, _OBJ_CONTAINER_LIST_INIT(FM_Object_Info_Device), sizeof(struct fm_device)},
    {FM_Object_Class_Fsm, _OBJ_CONTAINER_LIST_INIT(FM_Object_Info_Fsm), sizeof(struct fsm_info)},
    {FM_Object_Class_Timer, _OBJ_CONTAINER_LIST_INIT(FM_Object_Info_Timer), sizeof(struct fm_timer)},
};

/**
 * @brief This function will return the specified type of object information.
 *
 * @param type is the type of object, which can be
 *             FM_Object_Class_Thread/Semaphore/Mutex... etc
 *
 * @return the object type information or FM_NULL
 */
struct fm_object_information *
fm_object_get_information(enum fm_object_class_type type)
{
    int index;

    for (index = 0; index < FM_Object_Info_Unknown; index ++)
        if (_object_container[index].type == type) return &_object_container[index];

    return FM_NULL;
}

/**
 * @brief This function will return the length of object list in object container.
 *
 * @param type is the type of object, which can be
 *             FM_Object_Class_Thread/Semaphore/Mutex... etc
 *
 * @return the length of object list
 */
int fm_object_get_length(enum fm_object_class_type type)
{
    int count = 0;
    struct fm_list_node *node = FM_NULL;
    struct fm_object_information *information = FM_NULL;

    information = fm_object_get_information((enum fm_object_class_type)type);
    if (information == FM_NULL) return 0;

    /* get the count of objects */
    fm_list_for_each(node, &(information->object_list))
    {
        count ++;
    }

    return count;
}

/**
 * @brief This function will copy the object pointer of the specified type,
 *        with the maximum size specified by maxlen.
 *
 * @param type is the type of object, which can be
 *             FM_Object_Class_Thread/Semaphore/Mutex... etc
 *
 * @param pointers is the pointer will be saved to.
 *
 * @param maxlen is the maximum number of pointers can be saved.
 *
 * @return the copied number of object pointers.
 */
int fm_object_get_pointers(enum fm_object_class_type type, fm_object_t *pointers, int maxlen)
{
    int index = 0;

    struct fm_object *object;
    struct fm_list_node *node = FM_NULL;
    struct fm_object_information *information = FM_NULL;

    if (maxlen <= 0) return 0;

    information = fm_object_get_information((enum fm_object_class_type)type);
    if (information == FM_NULL) return 0;

    /* retrieve pointer of object */
    fm_list_for_each(node, &(information->object_list))
    {
        object = fm_list_entry(node, struct fm_object, list);

        pointers[index] = object;
        index ++;

        if (index >= maxlen) break;
    }

    return index;
}

/**
 * @brief This function will initialize an object and add it to object system
 *        management.
 *
 * @param object is the specified object to be initialized.
 *
 * @param type is the object type.
 *
 * @param name is the object name. In system, the object's name must be unique.
 */
void fm_object_init(struct fm_object         *object,
                    enum fm_object_class_type type,
                    const char               *name)
{
#ifdef FM_USING_DEBUG
    struct fm_list_node *node = FM_NULL;
#endif
    struct fm_object_information *information;

    /* get object information */
    information = fm_object_get_information(type);
    FM_ASSERT(information != FM_NULL);

#ifdef FM_USING_DEBUG
    /* check object type to avoid re-initialization */
    /* try to find object */
    for (node  = information->object_list.next;
            node != &(information->object_list);
            node  = node->next)
    {
        struct fm_object *obj;
        obj = fm_list_entry(node, struct fm_object, list);
        FM_ASSERT(obj != object);
    }
#endif
    /* initialize object's parameters */
    /* set object type to static */
    object->type = type | FM_Object_Class_Static;
#if FM_NAME_MAX > 0
    fm_strncpy(object->name, name, FM_NAME_MAX);  /* copy name */
#else
    object->name = name;
#endif /* FM_NAME_MAX > 0 */

    /* insert object into information object list */
    fm_list_insert_after(&(information->object_list), &(object->list));
}

/**
 * @brief This function will detach a static object from object system,
 *        and the memory of static object is not freed.
 *
 * @param object the specified object to be detached.
 */
void fm_object_detach(fm_object_t object)
{
    /* object check */
    FM_ASSERT(object != FM_NULL);

    /* reset object type */
    object->type = 0;

    /* remove from old list */
    fm_list_remove(&(object->list));
}

#ifdef FM_USING_HEAP
/**
 * @brief This function will allocate an object from object system.
 *
 * @param type is the type of object.
 *
 * @param name is the object name. In system, the object's name must be unique.
 *
 * @return object
 */
fm_object_t fm_object_allocate(enum fm_object_class_type type, const char *name)
{
    struct fm_object *object;
    struct fm_object_information *information;

    /* get object information */
    information = fm_object_get_information(type);
    FM_ASSERT(information != FM_NULL);

    object = (struct fm_object *)FM_KERNEL_MALLOC(information->object_size);
    if (object == FM_NULL)
    {
        /* no memory can be allocated */
        return FM_NULL;
    }

    /* clean memory data of object */
    fm_memset(object, 0x0, information->object_size);

    /* initialize object's parameters */
    /* set object type */
    object->type = type;

#if FM_NAME_MAX > 0
    fm_strncpy(object->name, name, FM_NAME_MAX); /* copy name */
#else
    object->name = name;
#endif /* FM_NAME_MAX > 0 */

    /* insert object into information object list */
    fm_list_insert_after(&(information->object_list), &(object->list));

    /* return object */
    return object;
}

/**
 * @brief This function will delete an object and release object memory.
 *
 * @param object is the specified object to be deleted.
 */
void fm_object_delete(fm_object_t object)
{

    /* object check */
    FM_ASSERT(object != FM_NULL);
    FM_ASSERT(!(object->type & FM_Object_Class_Static));

    /* reset object type */
    object->type = FM_Object_Class_Null;

    /* remove from old list */
    fm_list_remove(&(object->list));

    /* free the memory of object */
    FM_KERNEL_FREE(object);
}
#endif /* FM_USING_HEAP */


/**
 * @brief This function will judge the object is system object or not.
 *
 * @note  Normally, the system object is a static object and the type
 *        of object set to FM_Object_Class_Static.
 *
 * @param object is the specified object to be judged.
 *
 * @return FM_TRUE if a system object, FM_FALSE for others.
 */
fm_bool_t fm_object_is_systemobject(fm_object_t object)
{
    /* object check */
    FM_ASSERT(object != FM_NULL);

    if (object->type & FM_Object_Class_Static)
        return FM_TRUE;

    return FM_FALSE;
}

/**
 * @brief This function will return the type of object without
 *        FM_Object_Class_Static flag.
 *
 * @param object is the specified object to be get type.
 *
 * @return the type of object.
 */
fm_uint8_t fm_object_get_type(fm_object_t object)
{
    /* object check */
    FM_ASSERT(object != FM_NULL);

    return object->type & ~FM_Object_Class_Static;
}

/**
 * @brief This function will find specified name object from object
 *        container.
 *
 * @param name is the specified name of object.
 *
 * @param type is the type of object
 *
 * @return the found object or FM_NULL if there is no this object
 * in object container.
 *
 * @note this function shall not be invoked in interrupt status.
 */
fm_object_t fm_object_find(const char *name, fm_uint8_t type)
{
    struct fm_object *object = FM_NULL;
    struct fm_list_node *node = FM_NULL;
    struct fm_object_information *information = FM_NULL;

    information = fm_object_get_information((enum fm_object_class_type)type);

    /* parameter check */
    if ((name == FM_NULL) || (information == FM_NULL)) return FM_NULL;

    /* try to find object */
    fm_list_for_each(node, &(information->object_list))
    {
        object = fm_list_entry(node, struct fm_object, list);
        if (fm_strncmp(object->name, name, FM_NAME_MAX) == 0)
        {
            return object;
        }
    }

    return FM_NULL;
}

/**
 * @brief This function will return the name of the specified object container
 *
 * @param object    the specified object to be get name
 * @param name      buffer to store the object name string
 * @param name_size  maximum size of the buffer to store object name
 *
 * @return -FM_EINVAL if any parameter is invalid or FM_EOK if the operation is successfully executed
 *
 * @note this function shall not be invoked in interrupt status
 */
fm_err_t fm_object_get_name(fm_object_t object, char *name, fm_uint8_t name_size)
{
    fm_err_t result = -FM_EINVAL;
    if ((object != FM_NULL) && (name != FM_NULL) && (name_size != 0U))
    {
        const char *obj_name = object->name;
        (void) fm_strncpy(name, obj_name, (fm_size_t)name_size);
        result = FM_EOK;
    }

    return result;
}
