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

typedef struct HashTable
{
    Parcel* root;
} HashTable;

unsigned int hash(const char* str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % HASH_TABLE_SIZE;
}

void toLowerCase(char* str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

Parcel* createParcel(const char* country, int weight, float valuation)
{
    Parcel* newParcel = (Parcel*)malloc(sizeof(Parcel));
    if (newParcel == NULL)
    {
        printf("Failed to allocate memory for new parcel");
        exit(EXIT_FAILURE);
    }
    strncpy(newParcel->country, country, MAX_COUNTRY_NAME - 1);
    newParcel->country[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(newParcel->country);  // Normalize to lowercase
    newParcel->weight = weight;
    newParcel->valuation = valuation;
    newParcel->left = newParcel->right = NULL;
    return newParcel;
}