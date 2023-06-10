/*Itamar Assaf: 314699612
Lior Avadyayev: 206087611*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <string.h>

                
#define SALT_INIT 1e5           
#define N 1e4                     
#define RUNS 3
#define PROC_NUM 2  
                
double calc_mean(double vals[], int len){
    double avg = 0;
    for(int i=0; i < len; i++)
        avg += vals[i];
    avg = avg/len;
    return avg;
}

void print_results(double print_val, double print_time){
    printf("\nUSING THE \"SALT SHAKER\" METHOD WITH %d THREADS AND %d ITERATIONS:\n", (int)RUNS, (int)PROC_NUM);
    printf("The calculated average value of e is:        %lf\n", print_val);
    printf("The absolute value of e is:                  %lf, and the difference is: %lf \n", exp(1), exp(1)-print_val);
    printf("The average execution time of the program:   %lf seconds\n\n", print_time);
}

int main(){
    
    double e_vals[RUNS], e_val;
    double start_time, end_time, avg_time, p =(double)(1/N);
    unsigned int salt;
    int passed;

    start_time = omp_get_wtime();                                           // starting simulation                         
    for(int run=0; run < RUNS; run++){                                      // repiting the expiriment several times to avrage the results for better accuracy
        salt = SALT_INIT;                                                   // initializing the number of salt grains at the begining of the expiriment 
        for(int shake=0; shake<N; shake++){                                 // shaking N times (each time only the remaining grains)         
            unsigned int seed =  omp_get_wtime();                           // initializing a different seed for each shake
            #pragma omp parallel num_threads(PROC_NUM) shared (salt)
            {       // OMP initialization
                passed = 0;                                                 // initializing the number of grains that passed in this shake

                # pragma omp for                                            // OMP for loop
                    for(int grain=0; grain < salt; grain++)                 // claculating for each grain rather it passed or not
                        if(((double)(rand_r(&seed)%1000))/1000<p)
                            passed++;    

                #pragma omp atomic                                          // atomicly calculating the following so we wont get a wrong input
                    salt -= passed;                                         // claculating the number of salt grains that did not make it through to expiriment again
            }
        }
        printf("evals run = %lf\n", passed/SALT_INIT);
        e_vals[run] = passed/SALT_INIT;                                      // calculating the value of e^-1 for the current expiriment to later on avg
    }

    e_val = 1/calc_mean(e_vals, RUNS);                                      // avraging all results
    end_time = omp_get_wtime();                                             // getting the end time of all expiriments
    avg_time = (end_time - start_time)/RUNS;                                 // deviding by numver of runs to get avg
    print_results(e_val, avg_time);                                          // printing the results
       
    return 0;
}