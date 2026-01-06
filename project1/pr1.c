#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

/**********************************************
 * Nicholas DiNonno
 * CS380
 * Project 1
 **********************************************/

#define USERNAME_LEN 80

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
 * @brief Represents an account balance.
 * 
 * @param username The account holder's username.
 * @param amount The account balance.
 */
typedef struct balance_t {
    char username[USERNAME_LEN];
    uint64_t amount;
} balance_t;


// Returns number of lines in file
int lineCounter(char *fileName) {
    
    // Open the file, test for errors
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error for lineCount opening file!");
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


// Compare function used by qsort for comparing transactions
int compare_trans(const void *a, const void *b) {
    return ((transaction_t*) a)->created_at - ((transaction_t*) b)->created_at;
}

// Create balance dictionary, then populate it with valid transactions
int calcBalance(transaction_t* transactions, int count, int numOfLines) {

    // Instantiate balance dictionary
    struct balance_t *balance;
    //struct balance_t balance[2 * numOfLines];
    balance = (balance_t*) malloc((numOfLines * 2) * sizeof(balance_t));
    int balanceCount = 0;
    
    // Append new names to dictionary
    for (int j = 0; j < count; j++) { // Iterate through transactions
        bool senderMatch = false;
        bool recipientMatch = false;
        for (int k = 0; k < balanceCount; k++) { // Iterate through balance

            // Identify non unique sender names, do not add "system" to dictionary
            if (strcmp(transactions[j].sender, balance[k].username) == 0 || strcmp(transactions[j].sender, "system") == 0) {
                senderMatch = true;
            }

            // Identify non unique recipient names, do not add "system" to dictionary
            if (strcmp(transactions[j].recipient, balance[k].username) == 0 || strcmp(transactions[j].sender, "system") == 0) {
                recipientMatch = true;
            }
        }

        // Append the unique sender name to the dictionary
        if (senderMatch == false) {
            strncpy(balance[balanceCount].username, transactions[j].sender, sizeof(balance[balanceCount].username));
            balanceCount++;
        }

        // Append the unique recipient name to the dictionary
        if (recipientMatch == false) {
            strncpy(balance[balanceCount].username, transactions[j].recipient, sizeof(balance[balanceCount].username));
            balanceCount++;
        }
    }

    // Set all initial balances to 0
    for (int r =0; r < balanceCount; r++) {
        balance[r].amount = 0;
    }

    // Execute valid transactions
    for (int d = 0; d < count; d++) { // Iterate through transactions
        for (int e = 0; e < balanceCount; e++) { // Iterate through balance

            // If sender is system, add to recipients wallet and do not subtract
            if (strcmp(transactions[d].sender, "system") == 0) {
                
                if (strcmp(transactions[d].recipient, balance[e].username) == 0) {
                    balance[e].amount += transactions[d].amount;
                    break;
                }
            }

            // Subtract transaction amount from sender
            if (strcmp(transactions[d].sender, balance[e].username) == 0) {
                
                // If the sender has enough for a valid transaction. aka if user balance is greater than transaction amount
                if (balance[e].amount >= transactions[d].amount) {
                    balance[e].amount -= transactions[d].amount;

                    // Add transaction amount to recipient
                    for (int f = 0; f < balanceCount; f++) { // Separate for loop to identify balance of recipient
                        if (strcmp(transactions[d].recipient, balance[f].username) == 0) {
                            balance[f].amount += transactions[d].amount;
                        }
                    }
                } 
            }
        }
    }

    // Print out balances
    printf("username,balance\n");
    for (int i = 0; i < balanceCount; i++) {
        printf("%s,%ld\n", balance[i].username, balance[i].amount);
    }

    // Free memory
    free(balance);
    return 0;
}


// main function
int main(int argc, char* argv[]) {

    // Get the user input for filename
    char* fileName = argv[1];

    // Open the file and ensure it opens properly
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error for main opening file! Womp womp");
    }
 
    // Allocate space for array
    int numOfLines = lineCounter(fileName);
    struct transaction_t *transactions;
    char line[255]; 
    transactions = (transaction_t*) malloc((numOfLines - 1) * sizeof(transaction_t));
    int count = 0;

    // Read first line to remove column headers
    fgets(line, sizeof(line), file); 

    // Append csv data to array line by line
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

    // Sort listings by created_at
    qsort(transactions, count, sizeof(transactions[0]), compare_trans);

    // Print sorted array
    printf("created_at,sender,recipient,amount\n");
    for (int i = 0; i < count; i++) {
        printf("%ld,", transactions[i].created_at);
        printf("%s,", transactions[i].sender);
        printf("%s,", transactions[i].recipient);
        printf("%ld\n", transactions[i].amount);
    }

    calcBalance(transactions, count, numOfLines);

    // Free memory, close file
    fclose(file);
    free(transactions);
    return 0;
}