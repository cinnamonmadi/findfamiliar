#include "ui.hpp"

#include <iostream>

const size_t DIALOG_ROW_LENGTH = 18;
const int DIALOG_TIMER_DURATION = 3;

UI::UI() {
    dialog_rows[0] = new char[DIALOG_ROW_LENGTH];
    dialog_rows[1] = new char[DIALOG_ROW_LENGTH];
    dialog_is_open = false;
}

UI::~UI() {
    delete [] dialog_rows[0];
    delete [] dialog_rows[1];
}

void UI::update() {
    if(dialog_is_open && dialog_display_length < (DIALOG_ROW_LENGTH * 2)) {
        dialog_timer--;
        if(dialog_timer == 0) {
            dialog_display_length++;
            dialog_timer = DIALOG_TIMER_DURATION;
        }
    }
}

void UI::dialog_open(const char* message) {
    dialog_message = message;
    dialog_progress();
}

void UI::dialog_progress() {
    if(dialog_is_open && dialog_display_length < (DIALOG_ROW_LENGTH * 2)) {
        dialog_display_length = DIALOG_ROW_LENGTH * 2;
        return;
    }

    if(dialog_message.length() == 0) {
        dialog_is_open = false;
        return;
    }

    dialog_pop_next_lines();
    dialog_display_length = 0;
    dialog_timer = DIALOG_TIMER_DURATION;
    if(!dialog_is_open) {
        dialog_is_open = true;
    }
}

void UI::dialog_pop_next_lines() {
    // Clean the row buffers
    for(size_t i = 0; i < DIALOG_ROW_LENGTH; i++) {
        dialog_rows[0][i] = ' ';
        dialog_rows[1][i] = ' ';
    }

    size_t row = 0;
    size_t col = 0;
    while(row != 2 && dialog_message.length() != 0) {
        // Find the next word's size
        const size_t space_index = dialog_message.find(' ');
        size_t next_word_size;
        if(space_index == std::string::npos) {
            next_word_size = dialog_message.length();
        } else {
            next_word_size = space_index;
        }

        // Check if we have space on this line for that word
        if(col + next_word_size > DIALOG_ROW_LENGTH) {
            row++;
            col = 0;
            if(row == 2) {
                break;
            }
        }

        // Pop the next word off the message
        std::string next_word = dialog_message.substr(0, next_word_size);
        dialog_message = dialog_message.substr(next_word_size, dialog_message.length() - next_word_size);

        // Fill the row buffer with the word
        for(char& c : next_word) {
            dialog_rows[row][col] = c;
            col++;
        }

        if(space_index != std::string::npos) {
            dialog_message = dialog_message.substr(1, dialog_message.length() - 1);
            if(col < DIALOG_ROW_LENGTH) {
                dialog_rows[row][col] = ' ';
                col++;
            }
        }
    }
}
