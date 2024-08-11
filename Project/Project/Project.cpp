#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma warning(disable:4996)

#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_NAME 21

typedef struct Parcel
{
    char country[MAX_COUNTRY_NAME];
    int weight;
    float valuation;
    struct Parcel* left, * right;
} Parcel;

typedef struct
{
    Parcel* root;
} HashTable;