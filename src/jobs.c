#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#include "mush.h"
#include "debug.h"
#define READ 0
#define WRITE 1

/*
 * This is the "jobs" module for Mush.
 * It maintains a table of jobs in various stages of execution, and it
 * provides functions for manipulating jobs.
 * Each job contains a pipeline, which is used to initialize the processes,
 * pipelines, and redirections that make up the job.
 * Each job has a job ID, which is an integer value that is used to identify
 * that job when calling the various job manipulation functions.
 *
 * At any given time, a job will have one of the following status values:
 * "new", "running", "completed", "aborted", "canceled".
 * A newly created job starts out in with status "new".
 * It changes to status "running" when the processes that make up the pipeline
 * for that job have been created.
 * A running job becomes "completed" at such time as all the processes in its
 * pipeline have terminated successfully.
 * A running job becomes "aborted" if the last process in its pipeline terminates
 * with a signal that is not the result of the pipeline having been canceled.
 * A running job becomes "canceled" if the jobs_cancel() function was called
 * to cancel it and in addition the last process in the pipeline subsequently
 * terminated with signal SIGKILL.
 *
 * In general, there will be other state information stored for each job,
 * as required by the implementation of the various functions in this module.
 */
typedef enum {
    NEW,
    RUNNING,
    COMPLETED,
    ABORTED,
    CANCELED,
} STATUS;

struct JobTable {
     int jobid; //an integer value that is used to identify that job when calling the various job manipulation functions
     int parentid; //parent id present
     PIPELINE* pipelineptr; //pipelineptr that is used to initialize the processes, pipelines, and redirections that make up the job.
     STATUS status;
     int filedescriptor; //read filedescriptor
 };

 static struct JobTable jobtable[MAX_JOBS]; //using this jobtable as an Array under .data section
 static int numrows = 0;
/**
 * @brief  Initialize the jobs module.
 * @details  This function is used to initialize the jobs module.
 * It must be called exactly once, before any other functions of this
 * module are called.
 *
 * @return 0 if initialization is successful, otherwise -1.
 */
int jobs_init(void) {
    // TO BE IMPLEMENTED
    //could initialize file descriptor to -1
    if (jobtable == NULL){
        return -1;
    }

    return 0;
}

/**
 * @brief  Finalize the jobs module.
 * @details  This function is used to finalize the jobs module.
 * It must be called exactly once when job processing is to be terminated,
 * before the program exits.  It should cancel all jobs that have not
 * yet terminated, wait for jobs that have been cancelled to terminate,
 * and then expunge all jobs before returning.
 *
 * @return 0 if finalization is completely successful, otherwise -1.
 */
int jobs_fini(void) {
    // TO BE IMPLEMENTED
    //When you free everything and end the program
    //When the User Inputs EOF
    return 0;
}

/**
 * @brief  Print the current jobs table.
 * @details  This function is used to print the current contents of the jobs
 * table to a specified output stream.  The output should consist of one line
 * per existing job.  Each line should have the following format:
 *
 *    <jobid>\t<pgid>\t<status>\t<pipeline>
 *
 * where <jobid> is the numeric job ID of the job, <status> is one of the
 * following strings: "new", "running", "completed", "aborted", or "canceled",
 * and <pipeline> is the job's pipeline, as printed by function show_pipeline()
 * in the syntax module.  The \t stand for TAB characters.
 *
 * @param file  The output stream to which the job table is to be printed.
 * @return 0  If the jobs table was successfully printed, -1 otherwise.
 */
int jobs_show(FILE *file) {
    // TO BE IMPLEMENTED

    int i;

    for (i = 0; i < MAX_JOBS; i++){
        
        if (&jobtable[i] == NULL){
            continue;
        }

        if(jobtable[i].status == 0){
            fprintf(file, "%d\t%d\t%s\t",jobtable[i].jobid, jobtable[i].parentid, "new");
            show_pipeline(file, jobtable[i].pipelineptr);
        }

        if(jobtable[i].status == 1){
            fprintf(file, "%d\t%d\t%s\t",jobtable[i].jobid, jobtable[i].parentid, "running");
            show_pipeline(file, jobtable[i].pipelineptr);
        }

        if(jobtable[i].status == 2){
            fprintf(file, "%d\t%d\t%s\t",jobtable[i].jobid, jobtable[i].parentid, "completed");
            show_pipeline(file, jobtable[i].pipelineptr);
        }

        if(jobtable[i].status == 3){
            fprintf(file, "%d\t%d\t%s\t",jobtable[i].jobid, jobtable[i].parentid, "aborted");
            show_pipeline(file, jobtable[i].pipelineptr);
        }

        if(jobtable[i].status == 4){
            fprintf(file, "%d\t%d\t%s\t",jobtable[i].jobid, jobtable[i].parentid, "canceled");
            show_pipeline(file, jobtable[i].pipelineptr);
        }
     }
     
     return 0; //if Sucessful

  }

/**
 * @brief  Create a new job to run a pipeline.
 * @details  This function creates a new job and starts it running a specified
 * pipeline.  The pipeline will consist of a "leader" process, which is the direct
 * child of the process that calls this function, plus one child of the leader
 * process to run each command in the pipeline.  All processes in the pipeline
 * should have a process group ID that is equal to the process ID of the leader.
 * The leader process should wait for all of its children to terminate before
 * terminating itself.  The leader should return the exit status of the process
 * running the last command in the pipeline as its own exit status, if that
 * process terminated normally.  If the last process terminated with a signal,
 * then the leader should terminate via SIGABRT.
 *
 * If the "capture_output" flag is set for the pipeline, then the standard output
 * of the last process in the pipeline should be redirected to be the same as
 * the standard output of the pipeline leader, and this output should go via a
 * pipe to the main Mush process, where it should be read and saved in the data
 * store as the value of a variable, as described in the assignment handout.
 * If "capture_output" is not set for the pipeline, but "output_file" is non-NULL,
 * then the standard output of the last process in the pipeline should be redirected
 * to the specified output file.   If "input_file" is set for the pipeline, then
 * the standard input of the process running the first command in the pipeline should
 * be redirected from the specified input file.
 *
 * @param pline  The pipeline to be run.  The jobs module expects this object
 * to be valid for as long as it requires, and it expects to be able to free this
 * object when it is finished with it.  This means that the caller should not pass
 * a pipeline object that is shared with any other data structure, but rather should
 * make a copy to be passed to this function.
 * 
 * @return  -1 if the pipeline could not be initialized properly, otherwise the
 * value returned is the job ID assigned to the pipeline.
 */
int jobs_run(PIPELINE *pline) {
    PIPELINE* copyofpipe = copy_pipeline(pline);     //Copy of Pipeline
    
    jobtable->pipelineptr = copyofpipe;
    jobtable->status = RUNNING; //sets it to Running
    int savingchild = 0;

    int idofWork = fork(); //forks once

    if (idofWork == -1){
        fprintf(stderr, "Failed to Fork");
        return -1; //returns error
    }
    //setpgid(0,0);

    if(idofWork == 0){ //ID CHILD OF LEADER
        
        COMMAND *commanditerator = copyofpipe->commands;
        //int index = 1;
        while(commanditerator != NULL){
            int idtwo = fork(); //id value for this child forks

              if(idtwo == 0){ //child process of the forks child ID
               

                ARG *argpntr = commanditerator->args; //first pline
                int numofargs = 0;

                while (argpntr != NULL){
                    argpntr = argpntr->next;
                    numofargs = numofargs + 1; //getting the numofargs
                }

                argpntr = commanditerator->args; //first pline
                char *argsArray[numofargs+1]; //creates an array to be passed onto execvp
                int j;
        
                for(j=0; j < numofargs; j++){
                    argsArray[j] = eval_to_string(argpntr->expr);
                    argpntr = argpntr->next;
                }        
                argsArray[numofargs] = NULL;

  
                execvp(*argsArray, argsArray);
                return -1;

             } else {
                 savingchild = idtwo;
                 //Parent process
             }
            
            numrows = numrows+1;
            commanditerator = commanditerator->next; //iterates
        }
             int var;
             int stat;
             int savingstatus;


             while((var = wait(&stat)) > 0){ //saving the status and exiting with the status
                 if(var != savingchild){
                     savingstatus = savingstatus; //stays the same

                 }else{
                    savingstatus = stat;

                 }
             }

             exit(savingstatus);
 
            //every child you make you have to exit
     } else{
            jobtable->jobid = idofWork; //Leaders process ID
            
     }
    return jobtable->jobid;

}

/**
 * @brief  Wait for a job to terminate.
 * @details  This function is used to wait for the job with a specified job ID
 * to terminate.  A job has terminated when it has entered the COMPLETED, ABORTED,
 * or CANCELED state.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(),
 * or -1 if any error occurs that makes it impossible to wait for the specified job.
 */
int jobs_wait(int jobid) {
    // TO BE IMPLEMENTED
    for(int i=0; i < MAX_JOBS; i++){
       int random;

       if(jobtable[i].jobid == jobid){
            waitpid(jobid, &random, 0);
            return random;
       }

    }

    return -1;
}

/**
 * @brief  Poll to find out if a job has terminated.
 * @details  This function is used to poll whether the job with the specified ID
 * has terminated.  This is similar to jobs_wait(), except that this function returns
 * immediately without waiting if the job has not yet terminated.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(), if the job
 * has terminated, or -1 if the job has not yet terminated or if any other error occurs.
 */
int jobs_poll(int jobid) {
    // TO BE IMPLEMENTED
    return -1;
}

/**
 * @brief  Expunge a terminated job from the jobs table.
 * @details  This function is used to expunge (remove) a job that has terminated from
 * the jobs table, so that space in the table can be used to start some new job.
 * In order to be expunged, a job must have terminated; if an attempt is made to expunge
 * a job that has not yet terminated, it is an error.  Any resources (exit status,
 * open pipes, captured output, etc.) that were being used by the job are finalized
 * and/or freed and will no longer be available.
 *
 * @param  jobid  The job ID of the job to expunge.
 * @return  0 if the job was successfully expunged, -1 if the job could not be expunged.
 */
int jobs_expunge(int jobid) {
    // TO BE IMPLEMENTED
    //You find the job that has the same jobid as the arguement
    //based on that job if that job is completed cancelled or aborted then you remove it from the job table and you free it
    //if its not you return -1

    int i;

    for(i=0; i < MAX_JOBS; i++){

        if(&jobtable[i] == NULL){
            return -1;
        }
        else{
            if(jobtable[i].jobid == jobid && jobtable[i].status == COMPLETED){
                free_pipeline(jobtable[i].pipelineptr);
                jobtable[i].pipelineptr = NULL; //set it to NULL 
            }

            if(jobtable[i].jobid == jobid && jobtable[i].status == ABORTED){
                free_pipeline(jobtable[i].pipelineptr);
                jobtable[i].pipelineptr = NULL; //set it to NULL 
            }

            if(jobtable[i].jobid == jobid && jobtable[i].status == CANCELED){
                free_pipeline(jobtable[i].pipelineptr);
                jobtable[i].pipelineptr = NULL; //set it to NULL 
            }

        }

    }
    return 0;

}

/**
 * @brief  Attempt to cancel a job.
 * @details  This function is used to attempt to cancel a running job.
 * In order to be canceled, the job must not yet have terminated and there
 * must not have been any previous attempt to cancel the job.
 * Cancellation is attempted by sending SIGKILL to the process group associated
 * with the job.  Cancellation becomes successful, and the job actually enters the canceled
 * state, at such subsequent time as the job leader terminates as a result of SIGKILL.
 * If after attempting cancellation, the job leader terminates other than as a result
 * of SIGKILL, then cancellation is not successful and the state of the job is either
 * COMPLETED or ABORTED, depending on how the job leader terminated.
 *
 * @param  jobid  The job ID of the job to cancel.
 * @return  0 if cancellation was successfully initiated, -1 if the job was already
 * terminated, a previous attempt had been made to cancel the job, or any other
 * error occurred.
 */
int jobs_cancel(int jobid) {
    // TO BE IMPLEMENTED
    return 0;
    //Use KILLPG TO KILL THE GROUP ID
}

/**
 * @brief  Get the captured output of a job.
 * @details  This function is used to retrieve output that was captured from a job
 * that has terminated, but that has not yet been expunged.  Output is captured for a job
 * when the "capture_output" flag is set for its pipeline.
 *
 * @param  jobid  The job ID of the job for which captured output is to be retrieved.
 * @return  The captured output, if the job has terminated and there is captured
 * output available, otherwise NULL.
 */
char *jobs_get_output(int jobid) {
    // TO BE IMPLEMENTED
    //we have to read from the file descripter of pipe that the leader just output too
    return NULL;
}

/**
 * @brief  Pause waiting for a signal indicating a potential job status change.
 * @details  When this function is called it blocks until some signal has been
 * received, at which point the function returns.  It is used to wait for a
 * potential job status change without consuming excessive amounts of CPU time.
 *
 * @return -1 if any error occurred, 0 otherwise.
 */
int jobs_pause(void) {
    // TO BE IMPLEMENTED
    return -1;
}
