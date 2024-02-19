#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int count_spaces(char *str) {
    int count = 0;
    while (*str) {
        if (*str == ' ') {
            count++;
        }
        str++;
    }
    return count;
}

int count_digits(char *str) {
    int count = 0;
    while (*str) {
        if ((*str) >= '0' && (*str) <= '9') {
            count++;
        }
        str++;
    }
    return count;
}

int count_starting_spaces(char *str) {
    char *str_cp = str;
    while (*str_cp == ' ') {
        str_cp++;
    }

    return str_cp - str;
}

int count_ending_spaces(char *str) {
    char *str_cp = str;
    char *str_s = str;
    while (*str_cp != '\0' && *(str_cp + 1) != '\0') {
        str_cp++;
    }
    int cnt = 0;
    while (str_cp != str_s && *str_cp == ' ') {
        cnt++;
        str_cp--;
    }
    return cnt;
}

int count_numbers(char *str) {
    int seen_digit = 0;
    int cnt = 0;
    while (*str != '\0') {
        if (*str == ' ' && seen_digit) {
            cnt++;
        }
        while (*str == ' ') {
            str++;
            seen_digit = 0;
        }

        while ((*str >= '0' && *str <= '9')) {
            str++;
            seen_digit = 1;
        }
    }

    if (seen_digit) {
        cnt++;
    }
    return cnt;
}

int main(int argc, char *argv[]) {
    int BUF_SIZE = 50;
    char buf[BUF_SIZE];
    gets(buf, BUF_SIZE);

    if (strchr(buf, '\n')) {
        BUF_SIZE = buf - strchr(buf, '\n');
        *strchr(buf, '\n') = '\0';
    }

    buf[BUF_SIZE - 1] = '\0';
    BUF_SIZE = strlen(buf) + 1;

    int spaces = count_spaces(buf);
    int digits = count_digits(buf);
    int extra_starting_spaces = count_starting_spaces(buf);
    int extra_ending_spaces = count_ending_spaces(buf + extra_starting_spaces);

    if (spaces + digits != BUF_SIZE - 1) {
        printf("Invalid args: unexpected symbol\n");
        exit(1);
    }

    if (!(digits > 1 && digits < 21)) {  //MAX_INT has 10 digits
        printf("Invalid args: unexpected number of digits\n");
        exit(1);
    }

    if (spaces - extra_starting_spaces - extra_ending_spaces == 0) {
        printf("Invalid number of args: two numbers expected, but less received\n");
        exit(1);
    }

    int numbers_counter = count_numbers(buf);

    if (numbers_counter != 2) {
        printf("Invalid number of args: two numbers expected, but more received\n");
        exit(1);
    }

    *strchr(buf + extra_starting_spaces, ' ') = '\0';

    char first_num_s[strlen(buf) + 1 - extra_starting_spaces];
    char second_num_s[BUF_SIZE - strlen(buf) - 1];

    strcpy(first_num_s, buf + extra_starting_spaces);
    strcpy(second_num_s, buf + strlen(first_num_s) + extra_starting_spaces + 1);

    int first_num = atoi(first_num_s);
    int second_num = atoi(second_num_s + count_starting_spaces(second_num_s));

    int sum = add(first_num, second_num);
    if (sum < 0) {
        printf("Undefined behaviour while adding two numbers happened\n");
        exit(1);
    }
    printf("%d", sum);
    exit(0);
}