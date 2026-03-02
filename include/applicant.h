/* Author: Egor Gerdiy */

#ifndef APPLICANT_H
#define APPLICANT_H

/*
 * applicant.h — интерфейс (КИС) модуля абитуриентов.
 * Типы: Date, Address, Applicant.
 * Функции: чтение из файла, фильтрация по проходному баллу >= 225,
 * запись результата в файл, просмотр содержимого файла.
 */

#include <stddef.h>
#include <stdio.h>

#define APPLICANT_SURNAME_LEN   64
#define APPLICANT_NAME_LEN     64
#define APPLICANT_PATRONYMIC_LEN 64
#define APPLICANT_GENDER_LEN   8
#define APPLICANT_NATIONALITY_LEN 32
#define APPLICANT_ADDR_FIELD_LEN 64
#define APPLICANT_CT_SCORES_MAX 5
#define APPLICANT_MAX_COUNT    256

typedef struct {
    int year;
    int month;
    int day;
} Date;

typedef struct {
    char postal_code[APPLICANT_ADDR_FIELD_LEN];
    char country[APPLICANT_ADDR_FIELD_LEN];
    char region[APPLICANT_ADDR_FIELD_LEN];
    char district[APPLICANT_ADDR_FIELD_LEN];
    char city[APPLICANT_ADDR_FIELD_LEN];
    char street[APPLICANT_ADDR_FIELD_LEN];
    char house[APPLICANT_ADDR_FIELD_LEN];
    char apartment[APPLICANT_ADDR_FIELD_LEN];
} Address;

typedef struct {
    char surname[APPLICANT_SURNAME_LEN];
    char name[APPLICANT_NAME_LEN];
    char patronymic[APPLICANT_PATRONYMIC_LEN];
    char gender[APPLICANT_GENDER_LEN];
    char nationality[APPLICANT_NATIONALITY_LEN];
    Date birth_date;
    Address address;
    int ct_scores[APPLICANT_CT_SCORES_MAX];
    int ct_scores_count;
    int passing_score;
} Applicant;

/*
 * Выводит содержимое текстового файла в stdout (просмотр файла).
 * Возвращает 0 при успехе, -1 при ошибке.
 */
int applicant_view_file(const char *path);

/*
 * Читает абитуриентов из файла path в массив applicants.
 * По адресу n записывает количество прочитанных записей.
 * Формат файла: блоки по 16 строк на одного абитуриента (см. docs).
 * Возвращает 0 при успехе, -1 при ошибке.
 */
int applicant_read_from_file(const char *path, Applicant *applicants, size_t *n);

/*
 * Записывает абитуриентов с проходным баллом >= threshold в файл path.
 * Возвращает 0 при успехе, -1 при ошибке.
 */
int applicant_write_filtered(const char *path,
                             const Applicant *applicants,
                             size_t n,
                             int threshold);

/*
 * Заполняет выходной массив out абитуриентами из applicants с проходным
 * баллом >= threshold. По адресу out_count записывает количество.
 */
void applicant_filter_by_passing_score(const Applicant *applicants,
                                       size_t n,
                                       int threshold,
                                       Applicant *out,
                                       size_t *out_count);

#endif /* APPLICANT_H */
