#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * This is the "data store" module for Mush.
 * It maintains a mapping from variable names to values.
 * The values of variables are stored as strings.
 * However, the module provides functions for setting and retrieving
 * the value of a variable as an integer.  Setting a variable to
 * an integer value causes the value of the variable to be set to
 * a string representation of that integer.  Retrieving the value of
 * a variable as an integer is possible if the current value of the
 * variable is the string representation of an integer.
 */
 struct Datastore{
    char* name;
    char* value;
    struct Datastore *next; //links to next node
    struct Datastore *prev; //links to previous node
 };

char* LongtoStringConverter(long var){

   int count = 0;
   long temp = var;
    while(temp >= 1){
        temp = temp / 10;
        count = count + 1;
    } //number of characters
    
    char* str = malloc(count); //malloc for the string space
    sprintf(str, "%ld", var);
    return str;

}

 static struct Datastore *head = NULL; //global head node

/**
 * @brief  Get the current value of a variable as a string.
 * @details  This function retrieves the current value of a variable
 * as a string.  If the variable has no value, then NULL is returned.
 * Any string returned remains "owned" by the data store module;
 * the caller should not attempt to free the string or to use it
 * after any subsequent call that would modify the value of the variable
 * whose value was retrieved.  If the caller needs to use the string for
 * an indefinite period, a copy should be made immediately.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @return  A string that is the current value of the variable, if any,
 * otherwise NULL.
 */
char *store_get_string(char *var) {
    // TO BE IMPLEMENTED
    struct Datastore *temp = head;
    if (var == NULL){ //initial case
        return NULL;
    }

    while(temp->next != NULL){
        if (strcmp(temp->name,var) == 0){
            return temp->value; //returns the value at that point like a hashmap would
        }

        temp = temp->next; //iterates
    }

    return NULL; //not found
}

/**
 * @brief  Get the current value of a variable as an integer.
 * @details  This retrieves the current value of a variable and
 * attempts to interpret it as an integer.  If this is possible,
 * then the integer value is stored at the pointer provided by
 * the caller.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @param  valp  Pointer at which the returned value is to be stored.
 * @return  If the specified variable has no value or the value
 * cannot be interpreted as an integer, then -1 is returned,
 * otherwise 0 is returned.
 */
int store_get_int(char *var, long *valp) {
    
    // TO BE IMPLEMENTED
    struct Datastore *temp = head;

    if (var == NULL){ //initial case has no value
        return -1;
    }

    while(temp->next != NULL){
        temp = temp->next; //iterates through

        if (strcmp(temp->name,var) == 0){
            long result;
            char *temp2;
            result = strtol(temp->value, &temp2, 10);
    
            if (!*temp2){
                *valp = result;
                return 0;
            }
             else{
                 return -1;
             }

        }

    }

    return -1; //if it doesn't find anything
}

/**
 * @brief  Set the value of a variable as a string.
 * @details  This function sets the current value of a specified
 * variable to be a specified string.  If the variable already
 * has a value, then that value is replaced.  If the specified
 * value is NULL, then any existing value of the variable is removed
 * and the variable becomes un-set.  Ownership of the variable and
 * the value strings is not transferred to the data store module as
 * a result of this call; the data store module makes such copies of
 * these strings as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set, or NULL if the variable is to become
 * un-set.
 */
int store_set_string(char *var, char *val) {
    // TO BE IMPLEMENTED
    struct Datastore *temp = head; //head is the dummy value

    if(val == NULL){
        temp = temp->next;
        while(temp != NULL){
            if(strcmp(temp->name, var) == 0){
                    if(temp->prev == head && temp->next == NULL){
                        head->next = NULL; //sets it to itself
                        free(temp);
                        return 0;
                    }
                    temp->prev->next = temp->next; //reconnection deleting node
                    temp->next->prev = temp->prev; //reconnection deleting node
                    temp->next = temp; //setting to self
                    temp->prev = temp; //setting to self
                    free(temp); //frees the node
                    return 0;
            }


            temp = temp->next; //iterates through
        }
        return 0;
    }

    if(temp == NULL){
        struct Datastore *newElement = malloc(sizeof(struct Datastore)); //allocates space of the size of the node struct for a node
        newElement->name = strdup(var);
        newElement->value = strdup(val); 

        newElement->next = NULL; //set the next to NULL
        head = malloc(sizeof(struct Datastore)); //dummy node
        head->next = newElement; //setting the head to the newElement
        newElement->prev = head; //sets the previous to head
        return 0;
    }
    else{
        if (temp->next == NULL){
            struct Datastore *newElement = malloc(sizeof(struct Datastore)); //allocates space of the size of the node struct for a node
            newElement->name = strdup(var);
            newElement->value = strdup(val); 
            head->next = newElement; //setting the head to the newElement
            newElement->prev = head; //sets the previous to head
            return 0; //success
        }
        temp = head->next;

        while(temp->next != NULL){
            if(strcmp(temp->name, var) == 0){
                if(temp->value != NULL){ //CASE 1 if found you replace
                    memcpy(temp->value, val, strlen(val)+1); //then you replace the value by using memcpy
                    return 0;
                }
                else{
                    //if its NULL its unset CASE 2
                    free(temp->value);
                    free(temp->name);
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                    free(temp);
                    return 0;

                }
            }

            temp = temp->next; //iterates through
      }

    if(strcmp(temp->name,var) == 0){ //CASE 3
        memcpy(temp->value, val, strlen(val)+1); //then you replace the value by using memcpy
        return 0;
    }

    //else you insert it at the end CASE 4
    struct Datastore *newElement = malloc(sizeof(struct Datastore)); //allocates space of the size of the node struct for a node
    newElement->name = strdup(var);
    newElement->value = strdup(val); 

    temp->next = newElement; //sets the next node to newElement node
    newElement->prev = temp; //sets the previous of the newElement to temp node
    newElement->next = NULL; //sets the next of newElement node to NULL
    return 0;

   }

}

/**
 * @brief  Set the value of a variable as an integer.
 * @details  This function sets the current value of a specified
 * variable to be a specified integer.  If the variable already
 * has a value, then that value is replaced.  Ownership of the variable
 * string is not transferred to the data store module as a result of
 * this call; the data store module makes such copies of this string
 * as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set.
 */
int store_set_int(char *var, long val) {
       // TO BE IMPLEMENTED
    //char *converted = malloc(sizeof(long));
    //sprintf(converted, "%ld", val);
    char *convertedString = LongtoStringConverter(val);
    return store_set_string(var, convertedString);


}

/**
 * @brief  Print the current contents of the data store.
 * @details  This function prints the current contents of the data store
 * to the specified output stream.  The format is not specified; this
 * function is intended to be used for debugging purposes.
 *
 * @param f  The stream to which the store contents are to be printed.
 */
void store_show(FILE *f) {
    // TO BE IMPLEMENTED
       // TO BE IMPLEMENTED
    struct Datastore *temp = head;
    temp = temp->next;
    fprintf(f, "{");

    if (temp == NULL){
        fprintf(f, "}"); // if its empty
        return;
    }

    while(temp != NULL){
        fprintf(f,"%s=", temp->name);
        fprintf(f, "%s", temp->value);
        if(temp->next != NULL){
            fprintf(f, "%s",",");
        }

        temp = temp->next;
    }

    fprintf(f, "}");
    fflush(f);
    
}
