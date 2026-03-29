/**
  ******************************************************************************
  * @file           : API_debounce.h
  * @brief          : API debounce definitions
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

#ifndef __API_debounce_H /**< To prevent recursive inclusion*/
#define __API_debounce_H

#include "API_common.h"

/**
 * @brief API debounce initialization.
 *
 * @param{in} bt_config		Button gpio configuration for debounce.
 *
 * @return void.
 * */
void debounceFSM_init(void);

/**
 * @brief Updating states on FSM for API debounce.
 *
 * @param[in] debounce_time		Bool parameter to know if the debounce time (40ms) was reached before to validate it.
 *
 * @return void.
 * */
void debounceFSM_update(bool_t debounce_time);

/**
 * @brief Read state of button for know if the button was pressed or not.
 *
 * @return bool_t
 * @retval true if the button was pressed, false in another case.
 * */
bool_t readKey(void);


#endif /* __API_debounce_H */
