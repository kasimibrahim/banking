/*

! should be compiled using -lpthread -lrt flags

 */

#include <pthread.h>  // POSIX thread
#include <stdio.h>    // C I/O library
#include <stdlib.h>   // C standard library
#include <sys/wait.h> // declarations for waiting
#include <unistd.h>   // UNIX standard symbolic constants and types

#include <string.h>




/**
 * An account is a structure that represents a bank account.
 * @property {int} balance - The amount of money in the account.
 */
typedef struct __account {
    int balance;
} account;

/**
 * The type of transaction.
 */
typedef enum __transaction_t { Withdraw, Deposit, Unknown } transaction_t;

/**
 * A transaction is a structure that contains a transaction type and an amount.
 * @property {transaction_t} transactionType - The type of transaction.
 * @property {int} amount - The amount of the transaction.
 */
typedef struct __transaction {
    transaction_t transactionType;
    int amount;
} transaction;

/**
 * A transactionQueue is a struct that contains a long and a pointer to a transaction.
 * @property {long} size - The number of transactions in the queue.
 * @property {transaction} transactions - an array of transactions
 */
typedef struct __transactionQueue {
    long size;
    transaction *transactions;

} transactionQueue;

/**
 * ExecuteTransactionsStruct is a struct that contains a pointer to an account and a pointer to a
 * transactionQueue.
 * @property {account} transactingAccount - The account that is currently being transacted.
 * @property {transactionQueue} currentTransactionQueue - A queue of transactions that are to be
 * executed.
 */
typedef struct __executeTransactionsStruct {
    account *transactingAccount;
    transactionQueue *currentTransactionQueue;

} executeTransactionsStruct;

void *parseTransactions( void *voidFileName );
transaction createTransaction( char *transactionRecord );
void *executeTransactions( void *voidExecuteTransactionsStruct );
void processTransaction(transaction currentTransaction, char* who );
int deposit( void *account, int amount );
int withdraw(void *account,  int amount );



/*
    The definitions below are just used as example code.
    Your code will have to actually read data from files to get the data
    ! Remove them!
*/
// #define IS_WITHDRAWAL 1
// #define AMOUNT_READ 1
// #define THERE_IS_DATA_IN_FILE_1 1
// #define THERE_IS_DATA_IN_FILE_2 1

int init_amt;
account acc;
pthread_t h;
pthread_t w;
pthread_mutex_t lock;

int main( int argc, char *argv [] ) {

    if (argc <= 3)
    {
        printf("bank: amount file1 file2\n");
        return 1;
    }

    if (argc > 4)
    {
        printf("bank: amount file1 file2\n");
        return 1; 
    }

    //account *theirAccount;

    
    init_amt = atoi(argv[1]);
    
    acc.balance = init_amt;
    
        
    char* husband = argv[2];
    char*wife = argv[3];


    char *f = argv[2];
    FILE *fptr = fopen(f, "r");
    if(!fptr)
    {
        printf("bank: cannot open file\n");  //message passed if successful
        exit(1);
    }

    f = argv[3];
    fptr = fopen(f, "r");
    if(!fptr)
    {
        printf("bank: cannot open file\n");  //message passed if successful
        exit(1);
    }
    fclose(fptr);

    // Major step 1
    // Print the opening account balance
    //   in the format specified
    printf("Opening balance: %d\n", init_amt);

    // Major step 2
    // Read and process transactions in separate threads
    // ...
    pthread_create(&h, NULL, parseTransactions, (void*)husband);
    pthread_create(&w, NULL, parseTransactions, (void*) wife);

    pthread_join(w, NULL);
    pthread_join(h, NULL);


    // Major step 4
    // Print the closing account balance 
    //   in the format specified
    printf("Closing balance: %d\n", acc.balance);
    return 0;
}


/**
 * It reads a file, and returns a struct containing the number of transactions in the file, and an
 * array of transactions
 *
 * Args:
 *   voidFileName (void): THe name of the file.
 *
 * Returns:
 *   A void pointer to a transactionQueue struct.
 */
void *parseTransactions( void *voidFileName ) 
    {
        char *fileName = voidFileName;
        transaction transc;
        FILE *fptr = fopen(fileName, "r");

        // using the function fgets to read strings from the file
        char activity[100];
        while(fgets(activity,sizeof(activity),fptr))
        {
            if (activity == NULL){
                exit(1);
            }
            //We use the request to create a transaction
            transc = createTransaction(activity);
            processTransaction(transc, fileName);
            // closing the file
            
        }
        fclose(fptr);
        
        //pthread_join(h, NULL);
        return ( void * ) 0;
    }

/**
 * Create a transaction struct/object from a 'string'
 *
 * Args:
 *   transactionRecord (char): A string containing the transaction type and amount.
 *
 * Returns:
 *   A transaction struct.
 */
transaction createTransaction( char *transactionString ) {

    transaction t;

    char* items;
    int which_item;
    which_item = 0;
    while ((items = strtok_r(transactionString, " ", &transactionString)))
    {
        if (which_item == 0)
        {
            // this part is for deposit or withdraw
            if (strcmp(items, "deposit") == 0)
            {
                t.transactionType = Deposit;
            }
            else if (strcmp(items, "withdraw") == 0)
            {
                t.transactionType = Withdraw;
            }
            else
            {
                t.transactionType = Unknown;
            } 

            which_item++;
        }
        // This part is for amount
        else
        {
            t.amount = atoi(items);
        }
    }
    //printf("%s",t);
    return t;
    //return *( ( transaction * ) 0 );
}

/**
 * Process a queue of transactions
 *
 * Args:
 *   voidExecuteTransactionsStruct (void): a void pointer to the executeTransactionsStruct struct
 */
void *executeTransactions( void *voidExecuteTransactionsStruct ) {
    // ! Warning: This value is invalid. Do not use!
    return ( void * ) 0;
}

/**
 * "This function takes a pointer to an account and a pointer to a transaction,
 * and then processes the transaction on the account."
 *
 * Args:
 *   transactingAccount (account): This is a pointer to account that is being transacted on.
 *   currentTransaction (transaction): a pointer to a transaction struct
 */
void processTransaction(transaction currentTransaction, char* who ) { 
    char* without_txt = who;
    char* name = strtok_r(without_txt, ".", &without_txt );
    if (currentTransaction.amount > 0){
        if (currentTransaction.transactionType == Deposit)
        {
            pthread_mutex_lock(&lock);
            acc.balance += (int) currentTransaction.amount;
            pthread_mutex_unlock(&lock);
            printf("Deposit: %d, User: %s, Account balance after: %d\n",currentTransaction.amount, name, acc.balance);
        }
        else if (currentTransaction.transactionType == Withdraw)
        {
            //if i do not have enough to withdraw, decline
            if ((acc.balance - (int) currentTransaction.amount) < 0)
            {
                printf("Withdraw: %d, User: %s, Transaction declined\n",currentTransaction.amount, name);
            }else{
                pthread_mutex_lock(&lock);
                acc.balance -= (int) currentTransaction.amount;
                pthread_mutex_unlock(&lock);
                printf("Withdraw: %d, User: %s, Account balance after: %d\n",currentTransaction.amount, name, acc.balance);
            }
        }
            
    }else
    {
        //printf("Withdraw: %d, User: %s, Transaction declined\n",currentTransaction.amount, name);
    }
    
}

/**
 * It withdraws money from the provided account
 *
 * Args:
 *   account (void *): The account to be used
 *   amount  (int): the amount to be withdrawn
 */
int withdraw( void *account, int amount ) {
    return 0;
}

/**
 * Deposit an amount into provided account
 *
 * Args:
 *   account (void *): The account to be used
 *   amount  (int): The amount to be deposited
 */
int deposit( void *account, int amount ) {
    return 0;
}