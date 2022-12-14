#include <stdlib.h>
#include <stdio.h>

#include "mush.h"
#include "debug.h"

struct Node {
    STMT* statement;
    struct Node *next; //link to next node
    struct Node *prev; //links to previous node
 };

static int programCounter;
static struct Node *head = NULL; //global head node

/*
 * This is the "program store" module for Mush.
 * It maintains a set of numbered statements, along with a "program counter"
 * that indicates the current point of execution, which is either before all
 * statements, after all statements, or in between two statements.
 * There should be no fixed limit on the number of statements that the program
 * store can hold.
 */

/**
 * @brief  Output a listing of the current contents of the program store.
 * @details  This function outputs a listing of the current contents of the
 * program store.  Statements are listed in increasing order of their line
 * number.  The current position of the program counter is indicated by
 * a line containing only the string "-->" at the current program counter
 * position.
 *
 * @param out  The stream to which to output the listing.
 * @return  0 if successful, -1 if any error occurred.
 */
int prog_list(FILE *out) {

    struct Node *temp = head;
    temp = temp->next;

    while(temp != NULL){
        if(temp->statement->lineno == programCounter){
            fputs("-->\n", out); //extra thing to show where thee current position of the program is
            show_stmt(out, temp->statement);
            temp = temp->next;
            continue;
        }
        else{
            show_stmt(out, temp->statement);
        }

        temp = temp->next; 

    }

    return 0;
    
}

/**
 * @brief  Insert a new statement into the program store.
 * @details  This function inserts a new statement into the program store.
 * The statement must have a line number.  If the line number is the same as
 * that of an existing statement, that statement is replaced.
 * The program store assumes the responsibility for ultimately freeing any
 * statement that is inserted using this function.
 * Insertion of new statements preserves the value of the program counter:
 * if the position of the program counter was just before a particular statement
 * before insertion of a new statement, it will still be before that statement
 * after insertion, and if the position of the program counter was after all
 * statements before insertion of a new statement, then it will still be after
 * all statements after insertion.
 *
 * @param stmt  The statement to be inserted.
 * @return  0 if successful, -1 if any error occurred.
 */
int prog_insert(STMT *stmt) {
    // TO BE IMPLEMENTED
    if (stmt == NULL){
        return -1; //errors if it happens
    }

    struct Node *newElement = malloc(sizeof(struct Node)); //allocates space of the size of the node struct for a node
    struct Node *temp = head;

    if(temp == NULL){
        newElement->next = NULL; //set the next to NULL
        newElement->statement = stmt; //data that is stored
        head = malloc(sizeof(struct Node)); //dummy node
        head->next = newElement; //setting the head to the newElement
        newElement->prev = head; //sets the previous to head
        programCounter = newElement->statement->lineno; //initializes the programCounter
        return 0;
    }
    else{
        temp = head->next;

        newElement->statement = stmt; //setting it to stmt using a stmt pointer struct so basically transferring data
        int insertingNodeVal = newElement->statement->lineno;

        while(temp->next != NULL){

            int value = temp->statement->lineno; //current temp data line #

            if(insertingNodeVal == value){ //CASE 1
                 free_stmt(temp->statement); //frees the statement
                 free(newElement);
                 temp->statement = stmt; //transfer the data basically replace the statement
                 return 0;
            }

            int valueBefore;

            if(temp->prev->statement == NULL){
                valueBefore = 0;
            }else{
                valueBefore = temp->prev->statement->lineno; //previous Node's line #
            }

            if (insertingNodeVal > valueBefore){ //if line # greater than previous line # node   CASE 2
                if(insertingNodeVal < value){ //if line # less than previous line # node
                    temp->prev->next = newElement; //sets the next to newElement node
                    newElement->next = temp; //sets the next to the temp
                    
                    newElement->prev = temp->prev; //sets the other direction previous of newElement node
                    temp->prev = newElement; //sets the previous of the temp node

                    //insertion finished
                    return 0;
                }
            }
            
            temp = temp->next; //iterates through next node to check
        }

        //if it gets to here then it should get added at the end of the linkedlist //CASE 3
        if(temp->prev->statement == NULL){
             if(insertingNodeVal < temp->statement->lineno){ //if line # less than previous line # node
                 temp->prev->next = newElement; //sets the next to newElement node
                 newElement->next = temp; //sets the next to the temp
                    
                 newElement->prev = temp->prev; //sets the other direction previous of newElement node
                 temp->prev = newElement; //sets the previous of the temp n
                 return 0;

            }
        }

        if (insertingNodeVal < temp->statement->lineno){ 
            temp->prev->next = newElement; //sets the next to newElement node
            newElement->next = temp; //sets the next to the temp
                    
            newElement->prev = temp->prev; //sets the other direction previous of newElement node
            temp->prev = newElement; //sets the previous of the temp node
            return 0;
          }

        //case 4
        temp->next = newElement; //sets the next node to newElement node
        newElement->prev = temp; //sets the previous of the newElement to temp node
        newElement->next = NULL; //sets the next of newElement node to NULL

        return 0;

     }
    return -1; //if any error occurs someway //CASE 4
  }

/**
 * @brief  Delete statements from the program store.
 * @details  This function deletes from the program store statements whose
 * line numbers fall in a specified range.  Any deleted statements are freed.
 * Deletion of statements preserves the value of the program counter:
 * if before deletion the program counter pointed to a position just before
 * a statement that was not among those to be deleted, then after deletion the
 * program counter will still point the position just before that same statement.
 * If before deletion the program counter pointed to a position just before
 * a statement that was among those to be deleted, then after deletion the
 * program counter will point to the first statement beyond those deleted,
 * if such a statement exists, otherwise the program counter will point to
 * the end of the program.
 *
 * @param min  Lower end of the range of line numbers to be deleted.
 * @param max  Upper end of the range of line numbers to be deleted.
 */
int prog_delete(int min, int max) {
    // TO BE IMPLEMENTED
    if (min > max){
        return -1; //errors if this
    }
    if (min < 0 || max < 0){
        return -1; //errors if this
    }
    int flag = 0;

    //edge case
    struct Node *temp = head;
    struct Node *temptwo = head;

    while(temptwo->next != NULL){
        temptwo = temptwo->next; //iterates
    }

    if((temp->next->statement->lineno >= min) && (temptwo->statement->lineno <= max)){
        struct Node *tempthree = head;

        while(tempthree->next != NULL){
            tempthree = tempthree->next;
            free_stmt(tempthree->prev->statement);
            free(tempthree->prev);
        }
        head->next = NULL; //set
        programCounter = 0;
        return 0;
    }


    while(temp->next != NULL){
        temp = temp->next; //advances the temp

        if (temp->statement->lineno >= min){ //This is where you start deleting the nodes
  
            struct Node *tempholder = temp->prev; //holds the previous 

            while(temp->statement->lineno <= max){
                if(temp->next == NULL){
                    tempholder->next = temp; //connect
                    temp->prev = tempholder; //connect
                    return 0;
                }
                temp = temp->next; //iterate 
                free_stmt(temp->prev->statement);
                free(temp->prev);
                flag = 1; //changes if its in while loop
            }

       //     tempholder->next = temp; //sets the placeholder to the node after <= max
            if(flag == 1){ //reconnect due to deletion
                tempholder->next = temp;
                temp->prev = tempholder;
                programCounter = temp->statement->lineno; //sets prog counter here
                return 0;
            }

        }
    }

    return 0; //success
}

/**
 * @brief  Reset the program counter to the beginning of the program.
 * @details  This function resets the program counter to point just
 * before the first statement in the program.
 */
void prog_reset(void) {
    // TO BE IMPLEMENTED
    if(head == NULL){
        return;
    }
    else{
        programCounter = 0; //setting the ProgramCounter
    }
    
}

/**
 * @brief  Fetch the next program statement.
 * @details  This function fetches and returns the first program
 * statement after the current program counter position.  The program
 * counter position is not modified.  The returned pointer should not
 * be used after any subsequent call to prog_delete that deletes the
 * statement from the program store.
 *
 * @return  The first program statement after the current program
 * counter position, if any, otherwise NULL.
 */
STMT *prog_fetch(void) {
    // TO BE IMPLEMENTED
    struct Node *temp = head;
    temp = temp->next;

    //here its == to programCounter

    if(programCounter == 0 && temp != NULL){
        return temp->statement;
    }

    if(programCounter != 0 && temp != NULL){
        while(temp->statement->lineno != programCounter){
            temp = temp->next;
        }
        temp = temp->next;

        if(temp == NULL){
            return NULL;
        }
        return temp->statement;
       
    }

    return NULL;

}

/**
 * @brief  Advance the program counter to the next existing statement.
 * @details  This function advances the program counter by one statement
 * from its original position and returns the statement just after the
 * new position.  The returned pointer should not be used after any
 * subsequent call to prog_delete that deletes the statement from the
 * program store.
 *
 * @return The first program statement after the new program counter
 * position, if any, otherwise NULL.
 */
STMT *prog_next() {
    // TO BE IMPLEMENTED
    struct Node *temp = head;
    
    if (programCounter == 0){
        if(temp->next != NULL){
            programCounter = temp->next->statement->lineno;
            return temp->next->statement;
        }else{
            return NULL;
            }
    }
       
    temp = temp->next;
    //probably 1st node or higher
    while(temp != NULL){
        if(temp->statement->lineno == programCounter){
            if(temp->next != NULL){
                programCounter = temp->next->statement->lineno;
                return prog_fetch();
            }
            else{
                return NULL;
            }
        }
        temp = temp->next;
    }
    //else
    return NULL;

 }


/**
 * @brief  Perform a "go to" operation on the program store.
 * @details  This function performs a "go to" operation on the program
 * store, by resetting the program counter to point to the position just
 * before the statement with the specified line number.
 * The statement pointed at by the new program counter is returned.
 * If there is no statement with the specified line number, then no
 * change is made to the program counter and NULL is returned.
 * Any returned statement should only be regarded as valid as long
 * as no calls to prog_delete are made that delete that statement from
 * the program store.
 *
 * @return  The statement having the specified line number, if such a
 * statement exists, otherwise NULL.
 */
STMT *prog_goto(int lineno) { 
    // TO BE IMPLEMENTED
    struct Node *temp = head->next;
    int temp2 = programCounter;

    while(temp != NULL){
        if (temp->statement->lineno == lineno){
            programCounter = temp->prev->statement->lineno;
            return temp->statement;
        }
        temp = temp->next;
    }
    //if NULL no changes made
    programCounter = temp2; //no change is made
    return NULL;

}
