/*** includes ***/

#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<termios.h>
#include<ctype.h>
#include<errno.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tctsetattr");
}

void enableRawMode() {
    struct termios raw;
    atexit(disableRawMode);

    if (tcgetattr(STDIN_FILENO, &raw) == -1) die("tctgetattr");
    orig_termios = raw;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | ISTRIP | INPCK | BRKINT);
    raw.c_oflag&= ~(OPOST);
    raw.c_cflag&= ~(CS8);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;


    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tctsetattr");
}

/*** init ***/

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') break;
    }
    return 0;
}