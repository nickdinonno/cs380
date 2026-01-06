#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "uthash.h"

/**********************************************
 * Nicholas DiNonno
 * CS380
 * Project 4 (part 1)
 **********************************************/


#define USERNAME_LEN 64

/**
 * @brief Represents a transaction.
 * 
 * @param created_at The datetime at which the user created this transaction.
 * @param sender The sender's username.
 * @param recipient The recipient's username.
 * @param amount The amount transferred from sender to recipient.
 */
typedef struct transaction_t {
    time_t created_at;
    char sender[USERNAME_LEN];
    char recipient[USERNAME_LEN];
    uint64_t amount;
} transaction_t;


/**
 * @brief Represents a block in the blockchain.
 * 
 * @param transaction The transaction this block contains.
 * @param proof_of_work A number such that the hash of the block_t contains
 * some number of leading zeros. It has no meaning other than as part of the
 * hash.
 */
typedef struct block_t {
    transaction_t transaction;
    uint64_t proof_of_work;
} block_t;


// Returns number of lines in file
int lineCounter(char *fileName) {
    
    // Open the file, test for errors
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error for lineCounter opening file!");
        return 1;
    }    

    // Count the number of lines in the file
    int lineCount = 0;
    int fgetcFile;    
    while ((fgetcFile = fgetc(file)) != EOF) {
        if (fgetcFile == '\n') {
            lineCount++;
        }
    }

    fclose(file);
    return lineCount;
}

// Populate transactions array
int populateTransactions(char *fileName, FILE *file, transaction_t *transactions) {

    // Error Handling
    if (fileName == NULL) {
        printf("Error processing filename\n");
        return 1;
    }
    if (file == NULL) {
        printf("Error processing file\n");
        return 1;
    }
    if (transactions == NULL) {
        printf("Error processing transactions array\n");
        return 1;
    }

    // Buffer for transactions array
    char line[255];

    // Read first line to remove column headers
    fgets(line, sizeof(line), file); 

    // Append csv data to array line by line
    int count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {

        // Separate by each column, then append to array of strcuts. 
        char *token = strtok(line, ",");
        for (int i = 0; i < 4; i++) { // i < 4 because there are 4 columns

            // Append column created_at
            if (i == 0) {
                transactions[count].created_at = (time_t) atoi(token);
            }
            // Apend column sender
            else if (i == 1) {
                strncpy(transactions[count].sender, token, sizeof(transactions[count].sender));
            }
            // Append column recipient
            else if (i == 2) {
                strncpy(transactions[count].recipient, token, sizeof(transactions[count].recipient));
            }
            // Append column amount
            else if (i == 3) {
                transactions[count].amount = (uint64_t) atoi(token);
            }
            token = strtok(NULL, ",");
        }
        count++;
    }

    return 0;
}


// Mine a block for a transaction
int mine(transaction_t* transactions, int numOfLines) {

    // Error handling
    if (transactions == NULL) {
        printf("Error in processing transactions array.\n");
        return 1;
    }
    if (numOfLines == 0 || numOfLines == 1) {
        printf("Incorrect number of lines found while Mining. Please validate your csv has:\n 1) one row of column headers\n2) at least one row of data\n");
        return 1;
    }
    
    // Header for output
    printf("\ncreated_at,sender,recipient,amount,proof,digest\n");

    // Iterate through transactions and make a block for each transaction
    for (int i = 0; i < numOfLines - 1; i ++) {

        // Create a block_t data structure and fill with zeroes
        block_t *block = malloc(sizeof(struct block_t));
        memset(block, 0, sizeof(block_t));

        // Assign to the block a transaction
        block->transaction = transactions[i];

        // Check if there is a valid hash by iterating through every proof of work possibility
        for (int j = 0; j <= UINT64_MAX; j++) {

            // Assign variable proof of work to block to be hashed
            block->proof_of_work = j;

            // Create hash of block 
            unsigned char digest[SHA256_DIGEST_LENGTH];
            SHA256((unsigned char *)block, sizeof(block_t), digest);

            // See if digest is valid
            if (digest[0] == 0 && digest[1] == 0 && digest[2] == 0) {

                printf("%ld,", block->transaction.created_at);
                printf("%s,", block->transaction.sender);
                printf("%s,", block->transaction.recipient);
                printf("%ld,", block->transaction.amount);
                printf("%ld,", block->proof_of_work);

                // Print the digest
                for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) {
                    printf("%02hhx", digest[k]);
                }
                putchar('\n');
                break;
            }

            // Cannot find valid proof of work
            if (j == UINT64_MAX) { // If it is uint64max and makes it this far in the code
                printf("Cannot find a valid proof of work");
                return 1;
            }
        }
        // Free memory allocated to block
        free(block);
    }
    return 0;
}

// Struct for hash table
struct my_struct {
    const char *name; // Key
    int id;
    UT_hash_handle hh; // Makes struct hashable
};


// Calculate pending credit and print it (please note much of the hash table code is based off of uthash documentation)
int pendingCredit(transaction_t *transactions, int numOfLines) {
    // Error handling
    if (transactions == NULL) {
        printf("Error in processing transactions array.\n");
        return 1;
    }
    if (numOfLines == 0 || numOfLines == 1) {
        printf("Incorrect number of lines found while calculating Pending Credit. Please validate your csv has:\n 1) one row of column headers\n2) at least one row of data\n");
        return 1;
    }

    // Define variables needed for hash table
    struct my_struct *s, *tmp, *users = NULL;

    // Iterate through transactions array
    for (int i = 0; i < numOfLines - 1; ++i) {

        // Identify if it is the first time a user is found
        HASH_FIND_STR(users, transactions[i].recipient, s);
        if (s == NULL) {
            s = (struct my_struct *)malloc(sizeof *s);
            s->name = transactions[i].recipient;
            s->id = transactions[i].amount;
            HASH_ADD_KEYPTR(hh, users, s->name, strlen(s->name), s);
        }

        // If name is already in hash table, add pending credit
        else {
            s->id += transactions[i].amount;
        }
    }

    // Print pending credit information, print user names once
    printf("\nusername,pending_credit\n");
    for (int j = 0; j < numOfLines - 1; j ++) {
        HASH_FIND_STR(users, transactions[j].recipient, s);
        if (s->name == transactions[j].recipient) {
            printf("%s,", s->name);
            printf("%d\n", s->id);
        }
    }

    // Free the hash table contents
    HASH_ITER(hh, users, s, tmp) {
        HASH_DEL(users, s);
        free(s);
    }

    return 0;
}


// Main Function
int main(int argc, char* argv[]) {

    // Get the user input for filename
    char* fileName = argv[1];

    // Open the file and ensure it opens properly
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error from main opening file!\nDid you spell the filename wrong? Try again!\n");
        return 1;
    }

    // Create a transactions array
    int numOfLines = lineCounter(fileName);
    struct transaction_t *transactions; 
    transactions = (transaction_t*) malloc((numOfLines) * sizeof(transaction_t));

    // Fill transactions with specified csv transactions file
    populateTransactions(fileName, file, transactions);

    // Mine a block for each transaction and print transaction information
    mine(transactions, numOfLines);

    // Calculate pending credit and print it
    pendingCredit(transactions, numOfLines);

    // Free memory and close file
    fclose(file);
    free(transactions);

}