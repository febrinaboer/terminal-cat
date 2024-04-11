#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

// Frames for the cat animation
const char *frames[] = {
    "     /\\_/\\\n    ( o.o )\n     > ^ <\n",
    "     /\\_/\\\n    ( -.- )\n     > ^ <\n"
};
int num_frames = sizeof(frames) / sizeof(frames[0]);

// Function to clear the screen
void clear_screen() {
    printf("\033[H\033[J");
}

// Function to get terminal width
int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col - 3;
}

// Function to move the cat horizontally and display the frame
int move_cat(const char *cat, int direction, int left_padding) {
    clear_screen();
    int terminal_width = get_terminal_width();
    int cat_width = strlen(cat) - 2 * (strchr(cat, '\n') - cat); // Calculate width from the first line
    
    if (direction == -1) {  // Moving left
        left_padding -= 1;
    } else if (direction == 1) {  // Moving right
        left_padding += 1;
    }
    
    // Ensure padding does not become negative or excessively large
    left_padding = left_padding < 0 ? 0 : left_padding;
    left_padding = left_padding > terminal_width - cat_width ? terminal_width - cat_width : left_padding;
    
    char *line = strdup(cat);
    char *next_line = line;
    while ((next_line = strchr(line, '\n')) != NULL) {
        *next_line = '\0';  // Terminate the current line for printing
        printf("%*s%s\n", left_padding, "", line);
        line = next_line + 1;
    }
    free(line - strlen(cat));  // Reset pointer to start of the malloc'd space before freeing
    usleep(100000);  // Adjust timing for movement speed
    return left_padding;
}

int main() {
    int cat_direction = 1;  // 1 for right, -1 for left
    int left_padding = 0;
    int current_frame_index = 0;
    
    // Setup to handle KeyboardInterrupt (CTRL+C)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    while (1) {
        const char *cat = frames[current_frame_index];
        int cat_width = strlen(cat) - 2 * (strchr(cat, '\n') - cat);
        
        while (1) {
            left_padding = move_cat(cat, cat_direction, left_padding);
            if (left_padding <= 0 && cat_direction == -1) {
                cat_direction = 1;
                break;
            } else if (left_padding >= get_terminal_width() - cat_width && cat_direction == 1) {
                cat_direction = -1;
                break;
            }
            // Alternate frames for blinking every iteration
            current_frame_index = (current_frame_index + 1) % num_frames;
            cat = frames[current_frame_index];
        }
    }
    
    // Reset terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}
