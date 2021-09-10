#pragma once

#include <string>

class UI {
    public:
        std::string dialog_message;
        char* dialog_rows[2];
        bool dialog_is_open;
        size_t dialog_display_length;

        UI();
        ~UI();
        void update();
        void dialog_open(const char* message);
        void dialog_progress();
    private:
        int dialog_timer;

        void dialog_pop_next_lines();
};
