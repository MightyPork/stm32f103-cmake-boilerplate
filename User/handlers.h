//
// Created by MightyPork on 2016/9/4
//

#ifndef MPORK_HANDLERS_H
#define MPORK_HANDLERS_H

#include <common.h>

void ButtonHandler(uint32_t button, bool press);

void user_Error_Handler();

void user_assert_failed(uint8_t* file, uint32_t line);

void user_error_file_line(const char *message, const char *file, uint32_t line);

#endif //MPORK_HANDLERS_H
