/*
* FILE : project.cpp
* PROJECT : SENG1050 - project
* PROGRAMMER : Aumkumar Parmar
* FIRST VERSION : 2024-08-12
* DESCRIPTION :
* The code manages a parcel tracking system that reads data from a file into a hash table and a binary search tree, allowing
* users to query and display parcel details based on various criteria like country, weight, valuation, and size.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma warning(disable:4996)

#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_NAME 21

// Definition of a parcel
typedef struct Parcel
{
    char country[MAX_COUNTRY_NAME];
    int weight;
    float valuation;
    struct Parcel* left, * right;
} Parcel;

// Definition of a hash table bucket
typedef struct HashTable
{
    Parcel* root;
} HashTable;

/*
 * FUNCTION : unsigned int hash(const char* str)
 * DESCRIPTION :
 * This function is used to generate unique hash value.
 * PARAMETERS :
 * const char* str
 */

unsigned int hash(const char* str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % HASH_TABLE_SIZE;
}

void toLowerCase(char* str);
Parcel* createParcel(const char* country, int weight, float valuation);
Parcel* insertParcel(Parcel* root, const char* country, int weight, float valuation);
void loadParcels(HashTable* table, const char* filename);
void inorderTraversal(Parcel* root, const char* country, void (*func)(Parcel*));
void printParcel(Parcel* parcel);
void displayParcels(HashTable* table, const char* country);
void displayParcelsByWeight(Parcel* root, const char* country, int weight, int higher, int* found);
void displayParcelsByWeightWrapper(Parcel* root, const char* country, int weight, int higher);
void calculateLoadAndValuation(Parcel* root, const char* country, int* totalLoad, float* totalValuation);
void displayTotalLoadAndValuation(HashTable* table, const char* country);
void findCheapestAndMostExpensive(Parcel* root, const char* country, Parcel** cheapest, Parcel** mostExpensive);
void displayCheapestAndMostExpensive(HashTable* table, const char* country);
void findLightestAndHeaviest(Parcel* root, const char* country, Parcel** lightest, Parcel** heaviest);
void displayLightestAndHeaviest(HashTable* table, const char* country);
void freeBST(Parcel* root);
void freeHashTable(HashTable* table);

/*
 * FUNCTION : displayMenu()
 * DESCRIPTION :
 * This function prints the menu items.
 */

void displayMenu()
{
    printf("Menu:\n");
    printf("1. Enter country name and display all the parcels details\n");
    printf("2. Enter country and weight pair\n");
    printf("3. Display the total parcel load and valuation for the country\n");
    printf("4. Enter the country name and display cheapest and most expensive parcel's details\n");
    printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
    printf("6. Exit the application\n");
}

int main()
{
    // Initialize the hash table with NULL values
    HashTable table[HASH_TABLE_SIZE] = { { NULL } };

    // Load parcels from the file into the hash table
    loadParcels(table, "couriers.txt");

    int choice;
    char country[MAX_COUNTRY_NAME];
    int weight;
    char buffer[100];
    while (1)
    {
        displayMenu(); // Display the menu options
        printf("Enter your choice: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        if (sscanf(buffer, "%d", &choice) != 1)
        {
            printf("Invalid input. Please try again.\n");
            continue; // If the input is not a number, ask again
        }

        switch (choice)
        {
        case 1:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';

            // Copy the trimmed input to the country variable
            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            // Display parcels for the given country
            displayParcels(table, country);
            break;


        case 2:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;
            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';
            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';
            toLowerCase(country);  // Normalize to lowercase

            // Check if the country exists in the hash table
            if (table[hash(country)].root == NULL)
            {
                printf("Error: Country '%s' not found in the system.\n", country);
                break;  // Exit the case without asking for weight
            }

            printf("Enter weight: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;
            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';
            if (sscanf(buffer, "%d", &weight) != 1)
            {
                printf("Invalid input. Please try again.\n");
                continue; // If the weight is invalid, ask again
            }

            printf("Display parcels with weight higher (1) or lower (0) than %d? ", weight);
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;
            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';

            // Check if the input is a valid integer
            int higher;
            if (sscanf(buffer, "%d", &higher) != 1 || (higher != 0 && higher != 1))
            {
                printf("Invalid input. Please enter 0 or 1.\n");
                continue; // If the input is not 0 or 1, ask again
            }

            // Display parcels based on the weight criteria
            displayParcelsByWeightWrapper(table[hash(country)].root, country, weight, higher);
            break;



        case 3:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';

            // Copy the trimmed input to the country variable
            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            // Display total load and valuation for the given country
            displayTotalLoadAndValuation(table, country);
            break;

        case 4:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';

            // Copy the trimmed input to the country variable
            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            // Display cheapest and most expensive parcels for the given country
            displayCheapestAndMostExpensive(table, country);
            break;

        case 5:
            printf("Enter country name: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) continue;

            // Remove newline character if present
            buffer[strcspn(buffer, "\n")] = '\0';

            // Copy the trimmed input to the country variable
            strncpy(country, buffer, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';

            // Display lightest and heaviest parcels for the given country
            displayLightestAndHeaviest(table, country);
            break;

        case 6:
            freeHashTable(table); // Free the memory used by the hash table
            printf("Exiting the application.\n");
            return 0; // Exit the application
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}

/*
 * FUNCTION : void toLowerCase(char* str)
 * DESCRIPTION :
 * This function is used to convert all the characters into lower case.
 * PARAMETERS :
 * char* str
 */

void toLowerCase(char* str)
{
    // Iterate through each character in the string
    for (int i = 0; str[i]; i++)
    {
        // Convert the character to lowercase
        str[i] = tolower(str[i]);
    }
}

/*
 * FUNCTION : Parcel* createParcel(const char* country, int weight, float valuation)
 * DESCRIPTION :
 * The code defines a function that creates and initializes a new Parcel object with specified country, weight, and valuation
 * values, while allocating memory for it and normalizing the country name to lowercase.
 * PARAMETERS :
 * const char* country
 * int weight
 * float valuation
 */

Parcel* createParcel(const char* country, int weight, float valuation)
{
    // Allocate memory for a new Parcel object
    Parcel* newParcel = (Parcel*)malloc(sizeof(Parcel));
    // Check if memory allocation was successful
    if (newParcel == NULL)
    {
        printf("Failed to allocate memory for new parcel");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }
    // Copy the country name to the new parcel's country field
    strncpy(newParcel->country, country, MAX_COUNTRY_NAME - 1);
    newParcel->country[MAX_COUNTRY_NAME - 1] = '\0';  // Ensure null-termination

    // Convert the country name to lowercase
    toLowerCase(newParcel->country);

    // Set the weight and valuation for the new parcel
    newParcel->weight = weight;
    newParcel->valuation = valuation;

    // Initialize the left and right pointers to NULL (no child nodes)
    newParcel->left = newParcel->right = NULL;

    // Return the pointer to the newly created parcel
    return newParcel;
}

/*
 * FUNCTION : Parcel* insertParcel(Parcel* root, const char* country, int weight, float valuation)
 * DESCRIPTION :
 * The code defines a function that inserts a new Parcel into a binary search tree based on its weight, creating
 * the parcel if the tree is empty or placing it in the correct position relative to existing parcels based on weight comparisons.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * int weight
 * float valuation
 */

Parcel* insertParcel(Parcel* root, const char* country, int weight, float valuation)
{
    // If the tree is empty, create a new parcel and return it as the root
    if (root == NULL)
    {
        return createParcel(country, weight, valuation);
    }
    // If the tree is not empty
    if (root != NULL)
    {

        // If the weight of the new parcel is less than the current node's weight,
       // insert the new parcel in the left subtree
        if (weight < root->weight)
        {
            root->left = insertParcel(root->left, country, weight, valuation);
        }

        // If the weight of the new parcel is greater than the current node's weight,
        // insert the new parcel in the right subtree
        else if (weight > root->weight)
        {
            root->right = insertParcel(root->right, country, weight, valuation);
        }
    }

    // Return the unchanged root pointer after insertion
    return root;
}

/*
 * FUNCTION : void loadParcels(HashTable* table, const char* filename)
 * DESCRIPTION :
 * The code defines a function that reads parcel data from a file, parses each line to extract country, weight,
 * and valuation information, and inserts the parsed parcels into a hash table, normalizing country names to lowercase..
 * PARAMETERS :
 * HashTable* table
 * const char* filename
 */

void loadParcels(HashTable* table, const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];  // Buffer to hold each line of the file
    char country[MAX_COUNTRY_NAME];
    int weight = 0;  // Initialize weight
    float valuation = 0.0f;  // Initialize valuation

    while (fgets(line, sizeof(line), file))
    {
        // Remove trailing newline character if present
        line[strcspn(line, "\n")] = '\0';

        // Parse the country name
        char* token = strtok(line, ",");
        if (token != NULL)
        {
            strncpy(country, token, MAX_COUNTRY_NAME - 1);
            country[MAX_COUNTRY_NAME - 1] = '\0';
        }

        // Parse the weight
        token = strtok(NULL, ",");
        if (token != NULL)
        {
            weight = atoi(token);
        }

        // Parse the valuation
        token = strtok(NULL, ",");
        if (token != NULL)
        {
            valuation = (float)atof(token);  // Explicitly cast double to float
        }

        // Check if the weight and valuation are within the required ranges
        if (weight >= 100 && weight <= 50000 && valuation >= 10.0f && valuation <= 2000.0f)
        {
            toLowerCase(country);  // Normalize the country name to lowercase
            unsigned int index = hash(country); // Calculate hash index for the country
            table[index].root = insertParcel(table[index].root, country, weight, valuation); // Insert the parcel into the hash table
        }
    }

    if (fclose(file) != 0)
    {
        printf("Failed to close the file");
        exit(EXIT_FAILURE);
    }
}

/*
 * FUNCTION : void inorderTraversal(Parcel* root, const char* country, void (*func)(Parcel*))
 * DESCRIPTION :
 * The code defines a function that performs an in-order traversal of a binary search tree, applying a specified
 * function to each Parcel node whose country matches the given country.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * void (*func)(Parcel*)
 */

void inorderTraversal(Parcel* root, const char* country, void (*func)(Parcel*))
{
    if (root == NULL) return; // Base case: if the current node is NULL, return
    inorderTraversal(root->left, country, func); // Recursively traverse the left subtree
    if (strcmp(root->country, country) == 0)  // Check if the current node's country matches the target country
        func(root); // If the country matches, apply the function to the current node
    inorderTraversal(root->right, country, func); // Recursively traverse the right subtree
}

/*
 * FUNCTION : printParcel(Parcel* parcel)
 * DESCRIPTION :
 * The code prints the details of a given parcel, including its country, weight, and valuation.
 * PARAMETERS :
 * Parcel* parcel
 */

void printParcel(Parcel* parcel)
{
    // Print the details of the parcel: country, weight, and valuation
    printf("Country: %s, Weight: %d, Valuation: %.2f\n", parcel->country, parcel->weight, parcel->valuation);
}

/*
 * FUNCTION : displayParcels(HashTable* table, const char* country)
 * DESCRIPTION :
 * The code retrieves and displays all parcels for a specified country from a hash table by performing an in-order
 * traversal of the corresponding binary search tree.
 * PARAMETERS :
 * HashTable* table
 * const char* country
 */

void displayParcels(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];

    // Copy the country name to lowerCountry buffer
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';

    // Normalize the country name to lowercase for consistent comparison
    toLowerCase(lowerCountry);  // Normalize to lowercase

    // Compute the hash index for the normalized country name
    unsigned int index = hash(lowerCountry);

    // Check if the table index has a root node
    if (table[index].root)
    {
        // Perform an inorder traversal to print parcels for the given country
        inorderTraversal(table[index].root, lowerCountry, printParcel);
    }
    else
    {
        // Inform the user that no parcels were found for the specified country
        printf("No parcels found for %s\n", country);
    }
}

/*
 * FUNCTION : displayParcelsByWeight(Parcel* root, const char* country, int weight, int higher, int* found)
 * DESCRIPTION :
 * The code recursively searches a binary search tree to find and display parcels for a specified country that either have
 * weights higher or lower than a given value, and updates a flag if such parcels are found.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * int weight
 * int higher
 * int* found
 */

void displayParcelsByWeight(Parcel* root, const char* country, int weight, int higher, int* found)
{
    if (root == NULL) return; // Base case: return if the node is NULL

    // Recursively traverse the left subtree
    displayParcelsByWeight(root->left, country, weight, higher, found);

    // Check if the current node's country matches and if the weight condition is met
    if (strcmp(root->country, country) == 0 &&
        ((higher && root->weight > weight) || (!higher && root->weight < weight)))
    {
        // Print the parcel details if conditions are satisfied
        printParcel(root);
        *found = 1;  // Set the flag to indicate a matching parcel has been found
    }

    // Recursively traverse the right subtree
    displayParcelsByWeight(root->right, country, weight, higher, found);
}

/*
 * FUNCTION : displayParcelsByWeightWrapper(Parcel* root, const char* country, int weight, int higher)
 * DESCRIPTION :
 * This function inserts a name into a sorted linked list in the correct order.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * int weight
 * int higher
 */

void displayParcelsByWeightWrapper(Parcel* root, const char* country, int weight, int higher)
{
    int found = 0; // Initialize flag to track if any matching parcels are found
    displayParcelsByWeight(root, country, weight, higher, &found); // Perform the traversal and check conditions

    // If no parcels were found after the entire traversal
    if (!found)
    {
        printf("There are no parcels with a %s weight than %d grams.\n", higher ? "higher" : "lower", weight);
    }
}

/*
 * FUNCTION : calculateLoadAndValuation(Parcel* root, const char* country, int* totalLoad, float* totalValuation)
 * DESCRIPTION :
 * The code recursively traverses a binary search tree to accumulate the total load and valuation of parcels
 * for a specified country.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * int* totalLoad
 * float* totalValuation
 */

void calculateLoadAndValuation(Parcel* root, const char* country, int* totalLoad, float* totalValuation)
{
    if (root == NULL) return;
    if (strcmp(root->country, country) == 0)
    {
        *totalLoad += root->weight;
        *totalValuation += root->valuation;
    }
    calculateLoadAndValuation(root->left, country, totalLoad, totalValuation);
    calculateLoadAndValuation(root->right, country, totalLoad, totalValuation);
}

/*
 * FUNCTION : displayTotalLoadAndValuation(HashTable* table, const char* country)
 * DESCRIPTION :
 * The code calculates and displays the total load and valuation of parcels for a given country by retrieving and
 * processing data from the corresponding bucket in the hash table.
 * PARAMETERS :
 * HashTable* table
 * const char* country
 */

void displayTotalLoadAndValuation(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(lowerCountry);  // Normalize to lowercase

    unsigned int index = hash(lowerCountry);
    if (table[index].root)
    {
        int totalLoad = 0;
        float totalValuation = 0;
        calculateLoadAndValuation(table[index].root, lowerCountry, &totalLoad, &totalValuation);
        printf("Total load: %d grams, Total valuation: %.2f\n", totalLoad, totalValuation);
    }
    else
    {
        printf("No parcels found for %s\n", country);
    }
}

/*
 * FUNCTION : findCheapestAndMostExpensive(Parcel* root, const char* country, Parcel** cheapest, Parcel** mostExpensive)
 * DESCRIPTION :
 * The code recursively traverses a binary search tree to identify and update pointers to the cheapest and most
 * expensive parcels for a given country.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * Parcel** cheapest
 * Parcel** mostExpensive
 */

void findCheapestAndMostExpensive(Parcel* root, const char* country, Parcel** cheapest, Parcel** mostExpensive)
{
    if (root == NULL) return;
    if (strcmp(root->country, country) == 0)
    {
        if (!(*cheapest) || root->valuation < (*cheapest)->valuation)
            *cheapest = root;
        if (!(*mostExpensive) || root->valuation > (*mostExpensive)->valuation)
            *mostExpensive = root;
    }
    findCheapestAndMostExpensive(root->left, country, cheapest, mostExpensive);
    findCheapestAndMostExpensive(root->right, country, cheapest, mostExpensive);
}

/*
 * FUNCTION : displayCheapestAndMostExpensive(HashTable* table, const char* country)
 * DESCRIPTION :
 * The code finds and displays the cheapest and most expensive parcels for a given country by searching
 * the relevant bucket in the hash table.
 * PARAMETERS :
 * HashTable* table
 * const char* country
 */

void displayCheapestAndMostExpensive(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1);
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0';
    toLowerCase(lowerCountry);  // Normalize to lowercase

    unsigned int index = hash(lowerCountry);
    if (table[index].root)
    {
        Parcel* cheapest = NULL;
        Parcel* mostExpensive = NULL;
        findCheapestAndMostExpensive(table[index].root, lowerCountry, &cheapest, &mostExpensive);
        if (cheapest)
        {
            printf("Cheapest parcel: \n");
            printParcel(cheapest);
        }
        if (mostExpensive)
        {
            printf("Most expensive parcel: \n");
            printParcel(mostExpensive);
        }
    }
    else
    {
        printf("No parcels found for %s\n", country);
    }
}

/*
 * FUNCTION : findLightestAndHeaviest(Parcel* root, const char* country, Parcel** lightest, Parcel** heaviest)
 * DESCRIPTION :
 * The code recursively traverses a binary search tree to find and update pointers to the lightest and heaviest
 * parcels for a given country.
 * PARAMETERS :
 * Parcel* root
 * const char* country
 * Parcel** lightest
 * Parcel** heaviest
 */

void findLightestAndHeaviest(Parcel* root, const char* country, Parcel** lightest, Parcel** heaviest)
{
    if (root == NULL) return;
    if (strcmp(root->country, country) == 0)
    {
        if (!(*lightest) || root->weight < (*lightest)->weight)
            *lightest = root;
        if (!(*heaviest) || root->weight > (*heaviest)->weight)
            *heaviest = root;
    }
    findLightestAndHeaviest(root->left, country, lightest, heaviest);
    findLightestAndHeaviest(root->right, country, lightest, heaviest);
}

/*
 * FUNCTION : displayLightestAndHeaviest(HashTable* table, const char* country)
 * DESCRIPTION :
 * This function is used to The code displays the lightest and heaviest parcels for a given country by searching
 * the corresponding bucket in the hash table.
 * PARAMETERS :
 * HashTable* table
 * const char* country
 */

void displayLightestAndHeaviest(HashTable* table, const char* country)
{
    char lowerCountry[MAX_COUNTRY_NAME];
    strncpy(lowerCountry, country, MAX_COUNTRY_NAME - 1); // Copy the country name to avoid modifying the original
    lowerCountry[MAX_COUNTRY_NAME - 1] = '\0'; // Ensure null-termination
    toLowerCase(lowerCountry); // Convert the country name to lowercase

    unsigned int index = hash(lowerCountry);  // Compute the hash index for the country
    if (table[index].root) // Check if there are any parcels for the given country
    {
        Parcel* lightest = NULL; // Initialize pointers to keep track of the lightest and heaviest parcels
        Parcel* heaviest = NULL;
        findLightestAndHeaviest(table[index].root, lowerCountry, &lightest, &heaviest); // Find lightest and heaviest parcels
        if (lightest) // If the lightest parcel is found, print its details
        {
            printf("Lightest parcel: ");
            printParcel(lightest);
        }
        if (heaviest) // If the heaviest parcel is found, print its details
        {
            printf("Heaviest parcel: ");
            printParcel(heaviest);
        }
    }
    else
    {
        printf("No parcels found for %s\n", country); // Print a message if no parcels are found
    }
}

/*
 * FUNCTION : freeBST(Parcel* root)
 * DESCRIPTION :
 * This function is used to code recursively frees all nodes in a binary search tree starting from the given root.
 * PARAMETERS :
 * Parcel* root
 */

void freeBST(Parcel* root)
{
    if (!root) return; // Base case: if the current node is NULL, return
    freeBST(root->left); // Recursively free the left subtree
    freeBST(root->right); // Recursively free the right subtree
    free(root); // Free the current node
}

/*
 * FUNCTION : freeHashTable(HashTable* table)
 * DESCRIPTION :
 * This function used to all the memory allocated for the binary search trees stored in each bucket of the hash table.
 * PARAMETERS :
 * HashTable* table
 */

void freeHashTable(HashTable* table)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        freeBST(table[i].root); // Free all nodes in the binary search tree at index i
    }
}