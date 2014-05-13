/* Copyright 2014, TeamWau Software.
 * Licensed under the two-clause BSD license.
 * See COPYING for details
 *
 * TODO: (in order of importance)
 * 1. Comment code.
 * 2. Squash compiler warnings (about printing uint64_t's with the format string %llu)
 * 
 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <getopt.h>

#define VERSION_NUMBER "1.0.0"
char colour;

uint64_t getTimeDiff( struct timespec *time_A, struct timespec *time_B ) {
    return ( ( time_A->tv_sec * 1000000000 ) + time_A->tv_nsec ) -
           ( ( time_B->tv_sec * 1000000000 ) + time_B->tv_nsec );
}

int measureTime( char* program, char** program_args ) {
    struct timespec start, end;
    pid_t pID;
    int rs;

    if( program_args == NULL ) {
        program_args[0] = " ";
    }

    clock_gettime( CLOCK_MONOTONIC, &start );

    pID = fork();
    if( pID == 0 ) {
        execvp( program, program_args );
    }
    else if( pID < 0 ) {
        return 1;
    }
    else {
        waitpid( pID, &rs, 0 );

        clock_gettime( CLOCK_MONOTONIC, &end );

        uint64_t tdiff = getTimeDiff( &end, &start );

        if( colour == 'y' ) {
            printf( "\n\033[31;1mntime approx. wall time result: \033[32m%llu\033[36mns\033[0m\n", tdiff );
            return 0;
        }
        else if( colour == 'n' ) {
            printf( "\nntime approx. wall time result: %lluns\n", tdiff );
            return 0;
        }
    }
}

int main( int argc, char **argv ) {
    if( argc == 1 ) {
        printf( "%s - precise time program\nInvocation: %s [-nv] <program> <args for program>\n", argv[0], argv[0] );

        printf( "Arguments for %s: \n'-n': disable coloured output.\n'-v': print version and exit.\n", argv[0] ); 

        printf( "\nNOTICE: Times are \033[1mapproximate\033[0m! As this is a very accurate timer, it measures the overhead time of its own execution, as well as any work done by the kernel.\nWhat this means is that the times are likely to vary heavily and should probably be averaged versus used as-is as a benchmark.\n" );

        printf( "\nError: no program specified, terminating.\n" );

        return 1;

    }
        int opt, flags;
        opt = getopt( 2, argv, "nv" );
        switch( opt ){
            case 'n':
                flags = 1;
                break;
            case 'v':
                printf( "%s - version %s\n", argv[0], VERSION_NUMBER );
                return 0;
                break;
            default:
                flags = 0;
                break;
            }

        if( flags == 0 ) {
            colour = 'y';
            measureTime( argv[1], argv + 1 );
            return 0;
        }
        else if( flags == 1 ) {
            colour = 'n';
            measureTime( argv[2], argv + 2 );
            return 0;
        }
}
