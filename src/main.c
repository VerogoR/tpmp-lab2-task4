/* Author: Egor Gerdiy */

/*
 * main.c — клиент (КИС): просмотр содержимого файла, чтение данных абитуриентов,
 * фильтрация по проходному баллу >= 225, сохранение результата в новый файл.
 */

#include "applicant.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT   "applicants.txt"
#define DEFAULT_OUTPUT "result.txt"
#define THRESHOLD      225

static void print_usage(const char *prog);
static void print_applicant(const Applicant *a);

int main(int argc, char **argv)
{
    const char *input_path;
    const char *output_path;
    Applicant applicants[APPLICANT_MAX_COUNT];
    Applicant filtered[APPLICANT_MAX_COUNT];
    size_t n, n_filtered;
    int i;

    input_path  = argc > 1 ? argv[1] : DEFAULT_INPUT;
    output_path = argc > 2 ? argv[2] : DEFAULT_OUTPUT;

    if (argc >= 2 && strcmp(argv[1], "--view") == 0) {
        input_path = argc > 2 ? argv[2] : DEFAULT_INPUT;
        if (applicant_view_file(input_path) != 0) {
            fprintf(stderr, "Ошибка: не удалось открыть файл '%s' для просмотра.\n",
                    input_path);
            return 1;
        }
        return 0;
    }

    if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_usage(argv[0]);
        return 0;
    }

    /* Просмотр содержимого входного файла */
    printf("--- Содержимое файла '%s' ---\n", input_path);
    if (applicant_view_file(input_path) != 0) {
        fprintf(stderr, "Ошибка: не удалось открыть файл '%s'.\n", input_path);
        return 1;
    }
    printf("--- Конец файла ---\n\n");

    /* Чтение и обработка данных */
    if (applicant_read_from_file(input_path, applicants, &n) != 0) {
        fprintf(stderr, "Ошибка чтения данных из '%s'.\n", input_path);
        return 1;
    }
    printf("Прочитано абитуриентов: %zu\n", n);

    applicant_filter_by_passing_score(applicants, n, THRESHOLD, filtered, &n_filtered);
    printf("Абитуриентов с проходным баллом >= %d: %zu\n\n", THRESHOLD, n_filtered);

    /* Вывод данных об отобранных абитуриентах на экран */
    printf("--- Абитуриенты с проходным баллом >= %d ---\n", THRESHOLD);
    for (i = 0; i < (int)n_filtered; i++) {
        print_applicant(&filtered[i]);
        printf("\n");
    }

    /* Сохранение результата в новый файл */
    if (applicant_write_filtered(output_path, applicants, n, THRESHOLD) != 0) {
        fprintf(stderr, "Ошибка записи в файл '%s'.\n", output_path);
        return 1;
    }
    printf("Результат сохранён в файл '%s'.\n", output_path);

    return 0;
}

static void print_usage(const char *prog)
{
    printf("Использование: %s [входной_файл [выходной_файл]]\n", prog);
    printf("              %s --view [файл]\n", prog);
    printf("По умолчанию: входной — %s, выходной — %s.\n",
           DEFAULT_INPUT, DEFAULT_OUTPUT);
    printf("Выводит абитуриентов с проходным баллом >= %d.\n", THRESHOLD);
}

static void print_applicant(const Applicant *a)
{
    int i;

    printf("%s %s %s\n", a->surname, a->name, a->patronymic);
    printf("  Пол: %s, Национальность: %s\n", a->gender, a->nationality);
    printf("  Дата рождения: %04d-%02d-%02d\n",
           a->birth_date.year, a->birth_date.month, a->birth_date.day);
    printf("  Адрес: %s, %s, %s, %s, %s, %s, д.%s, кв.%s\n",
           a->address.postal_code, a->address.country, a->address.region,
           a->address.district, a->address.city, a->address.street,
           a->address.house, a->address.apartment);
    printf("  Баллы ЦТ:");
    for (i = 0; i < a->ct_scores_count; i++)
        printf(" %d", a->ct_scores[i]);
    printf("\n  Проходной балл: %d\n", a->passing_score);
}
