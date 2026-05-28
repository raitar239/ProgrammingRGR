#pragma once
#include <string>
#include "types.h"

void clear_screen();
void wait_for_user();
int  get_safe_int_input(int min_val, int max_val, const std::string& prompt);
void print_card(const Card& card);
