/* Author: Egor Gerdiy */

/*
 * applicant_server.c — сервер (КИС): чтение/запись файлов абитуриентов,
 * фильтрация по проходному баллу, просмотр содержимого файла.
 */

#include "applicant.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 256

static int read_line(FILE *fp, char *buf, size_t size);
static int parse_applicant(FILE *fp, Applicant *a);

/*
 * Читает одну строку (без \n) в buf, не более size-1 символов.
 * Возвращает 0 при успехе, -1 при EOF/ошибке.
 */
static int read_line(FILE *fp, char *buf, size_t size)
{
    if (fgets(buf, (int)size, fp) == NULL)
        return -1;
    buf[strcspn(buf, "\n")] = '\0';
    return 0;
}

/*
 * Парсит одного абитуриента из fp (16 строк).
 */
static int parse_applicant(FILE *fp, Applicant *a)
{
    char line[LINE_BUF_SIZE];

    if (read_line(fp, a->surname, sizeof(a->surname)) != 0)
        return -1;
    if (read_line(fp, a->name, sizeof(a->name)) != 0)
        return -1;
    if (read_line(fp, a->patronymic, sizeof(a->patronymic)) != 0)
        return -1;
    if (read_line(fp, a->gender, sizeof(a->gender)) != 0)
        return -1;
    if (read_line(fp, a->nationality, sizeof(a->nationality)) != 0)
        return -1;
    if (read_line(fp, line, sizeof(line)) != 0)
        return -1;
    if (sscanf(line, "%d %d %d",
               &a->birth_date.year, &a->birth_date.month, &a->birth_date.day) != 3)
        return -1;
    if (read_line(fp, a->address.postal_code, sizeof(a->address.postal_code)) != 0)
        return -1;
    if (read_line(fp, a->address.country, sizeof(a->address.country)) != 0)
        return -1;
    if (read_line(fp, a->address.region, sizeof(a->address.region)) != 0)
        return -1;
    if (read_line(fp, a->address.district, sizeof(a->address.district)) != 0)
        return -1;
    if (read_line(fp, a->address.city, sizeof(a->address.city)) != 0)
        return -1;
    if (read_line(fp, a->address.street, sizeof(a->address.street)) != 0)
        return -1;
    if (read_line(fp, a->address.house, sizeof(a->address.house)) != 0)
        return -1;
    if (read_line(fp, a->address.apartment, sizeof(a->address.apartment)) != 0)
        return -1;
    if (read_line(fp, line, sizeof(line)) != 0)
        return -1;
    /* Баллы ЦТ: числа через пробел в одной строке */
    {
        const char *p = line;
        int sc, consumed;
        a->ct_scores_count = 0;
        while (a->ct_scores_count < APPLICANT_CT_SCORES_MAX &&
               sscanf(p, "%d%n", &sc, &consumed) == 1) {
            a->ct_scores[a->ct_scores_count++] = sc;
            p += consumed;
            while (*p == ' ' || *p == '\t')
                p++;
        }
    }
    if (read_line(fp, line, sizeof(line)) != 0)
        return -1;
    if (sscanf(line, "%d", &a->passing_score) != 1)
        return -1;
    return 0;
}

int applicant_view_file(const char *path)
{
    FILE *fp;
    int c;

    fp = fopen(path, "r");
    if (fp == NULL)
        return -1;
    while ((c = fgetc(fp)) != EOF)
        putchar(c);
    fclose(fp);
    return 0;
}

int applicant_read_from_file(const char *path, Applicant *applicants, size_t *n)
{
    FILE *fp;
    size_t count = 0;

    if (path == NULL || applicants == NULL || n == NULL)
        return -1;
    fp = fopen(path, "r");
    if (fp == NULL)
        return -1;
    *n = 0;
    while (count < APPLICANT_MAX_COUNT) {
        if (parse_applicant(fp, &applicants[count]) != 0)
            break;
        count++;
    }
    *n = count;
    fclose(fp);
    return 0;
}

void applicant_filter_by_passing_score(const Applicant *applicants,
                                        size_t n,
                                        int threshold,
                                        Applicant *out,
                                        size_t *out_count)
{
    size_t i, j = 0;

    if (applicants == NULL || out == NULL || out_count == NULL)
        return;
    for (i = 0; i < n; i++) {
        if (applicants[i].passing_score >= threshold)
            out[j++] = applicants[i];
    }
    *out_count = j;
}

static void write_applicant(FILE *fp, const Applicant *a)
{
    int i;

    fprintf(fp, "%s\n", a->surname);
    fprintf(fp, "%s\n", a->name);
    fprintf(fp, "%s\n", a->patronymic);
    fprintf(fp, "%s\n", a->gender);
    fprintf(fp, "%s\n", a->nationality);
    fprintf(fp, "%d %d %d\n",
            a->birth_date.year, a->birth_date.month, a->birth_date.day);
    fprintf(fp, "%s\n", a->address.postal_code);
    fprintf(fp, "%s\n", a->address.country);
    fprintf(fp, "%s\n", a->address.region);
    fprintf(fp, "%s\n", a->address.district);
    fprintf(fp, "%s\n", a->address.city);
    fprintf(fp, "%s\n", a->address.street);
    fprintf(fp, "%s\n", a->address.house);
    fprintf(fp, "%s\n", a->address.apartment);
    for (i = 0; i < a->ct_scores_count; i++) {
        if (i > 0)
            fputc(' ', fp);
        fprintf(fp, "%d", a->ct_scores[i]);
    }
    fputc('\n', fp);
    fprintf(fp, "%d\n", a->passing_score);
}

int applicant_write_filtered(const char *path,
                              const Applicant *applicants,
                              size_t n,
                              int threshold)
{
    FILE *fp;
    size_t i;

    if (path == NULL || applicants == NULL)
        return -1;
    fp = fopen(path, "w");
    if (fp == NULL)
        return -1;
    for (i = 0; i < n; i++) {
        if (applicants[i].passing_score >= threshold)
            write_applicant(fp, &applicants[i]);
    }
    fclose(fp);
    return 0;
}
