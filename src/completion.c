#include <fmcu.h>

#define FM_COMPLETED    1
#define FM_UNCOMPLETED  0

/**
 * @brief This function will initialize a completion object.
 *
 * @param completion is a pointer to a completion object.
 */
void fm_completion_init(struct fm_completion *completion)
{
    FM_ASSERT(completion != FM_NULL);

    completion->flag = FM_COMPLETED;
}

/**
 * @brief This function will wait for a completion, if the completion is unavailable, the thread shall wait for
 *        the completion up to a specified time.
 *
 * @param completion is a pointer to a completion object.
 *
 * @param timeout is a timeout period (unit: OS ticks). If the completion is unavailable, the thread will wait for
 *                the completion done up to the amount of time specified by the argument.
 *                NOTE: Generally, we use the macro FM_WAITING_FOREVER to set this parameter, which means that when the
 *                completion is unavailable, the thread will be waitting forever.
 *
 * @warning This function can ONLY be called in the thread context. It MUST NOT be called in interrupt context.
 */
void fm_completion_wait(struct fm_completion *completion,
                            fm_int32_t            timeout)
{
    FM_ASSERT(completion != FM_NULL);

    completion->flag = FM_UNCOMPLETED;

    fm_uint32_t start_tick = fm_get_systick();

    if (timeout == FM_WAITING_FOREVER)
    {
        while(completion->flag == FM_UNCOMPLETED);
    }
    else
    {
        while((completion->flag == FM_UNCOMPLETED) && 
            (fm_get_systick() - start_tick < timeout));
    }

    completion->flag = FM_COMPLETED;
}

/**
 * @brief This function indicates a completion has done.
 *
 * @param completion is a pointer to a completion object.
 */
void fm_completion_done(struct fm_completion *completion)
{
    FM_ASSERT(completion != FM_NULL);

    if (completion->flag == FM_COMPLETED)
        return;
    
    completion->flag = FM_COMPLETED;
}
