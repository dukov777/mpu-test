/*
 * logger.h
 *
 *  Created on: Sep 24, 2022
 *      Author: petarlalov
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#ifdef __cplusplus
 extern "C" {
#endif

typedef int (*writter_t)(char*);

int logger_open();
int logger_write(char* line);
int logger_close();

#define GLOBAL_DATA    __attribute__( ( section( "global_data" ) ) )

 #ifdef __cplusplus
}
#endif

#endif /* INC_LOGGER_H_ */
