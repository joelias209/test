//
//  main.c
//  eField
//
//  Created by Miguel Lastras on 10/09/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WORLD_WIDTH 20
#define WORLD_HEIGHT 5

#define Q 1
#define M 1
#define K 1
#define DT 0.1

typedef struct{
    double x;
    double y;
} component;

typedef struct{
    component p; // position
    component v; // velocity
    component a; // acceleration
    double q;
} particle;

typedef struct{
    particle *P; // particles
    size_t n_neg;
    size_t n_pos;
} object;

void print_pos(object *sea){
    for(size_t j=0; j<(sea->n_neg+sea->n_pos); j++){
        if(sea->P[j].q < 0){
            printf("%f, %f\n", sea->P[j].p.x, sea->P[j].p.y);
        }
    }
}

void time_step(object *sea){
    
    // compute all resulting accelerations (without moving particles)
    for(size_t j=0; j<(sea->n_neg+sea->n_pos); j++){
        component aj;
        aj.x = 0;
        aj.y = 0;
        component rj = sea->P[j].p;
        double qj = sea->P[j].q;
        if(qj < 0){ // only for electrons
            for(size_t i=0; i<(sea->n_neg+sea->n_pos); i++){
                if(i != j){
                    component ri = sea->P[i].p;
                    component r;
                    r.x = rj.x - ri.x;
                    r.y = rj.y - ri.y;
                    double r_mag = sqrt(r.x*r.x + r.y*r.y);
                    double qi = sea->P[i].q;
                    double f = K*qj*qi/(r_mag*r_mag);
                    aj.x = f * r.x / (r_mag * M);
                    aj.y = f * r.y / (r_mag * M);
                }
            }
        }
        sea->P[j].a = aj;
    }
    
    // move the particles on the resulting acceleration vector
    for(size_t j=0; j<(sea->n_neg+sea->n_pos); j++){
        //printf("P%zu: q=%f p.x=%f p.y=%f v.x=%f v.y=%f a.x=%.10f a.y=%.10f\n", j, sea->P[j].q, sea->P[j].p.x, sea->P[j].p.y, sea->P[j].v.x, sea->P[j].v.y, sea->P[j].a.x, sea->P[j].a.y);
        if(sea->P[j].q < 0){ // only for electrons
            sea->P[j].p.x += sea->P[j].v.x*DT + 0.5*sea->P[j].a.x*DT*DT;
            sea->P[j].p.y += sea->P[j].v.y*DT + 0.5*sea->P[j].a.y*DT*DT;
            sea->P[j].v.x += sea->P[j].a.x*DT;
            sea->P[j].v.y += sea->P[j].a.y*DT;
        }
    }
}

int main(int argc, const char * argv[]) {
    
    object sea;
    sea.n_neg = 1;
    sea.n_pos = 1;
    sea.P = malloc(sizeof(particle)*(sea.n_neg+sea.n_pos));
    if(sea.P == NULL){
        fprintf(stderr, "Cannot allocate memory for sea!\n");
        exit(1);
    }
    
    for(size_t i=0; i<(sea.n_neg+sea.n_pos); i++){
        double x = WORLD_WIDTH*(double)rand()/((double)RAND_MAX);
        double y = WORLD_HEIGHT*(double)rand()/((double)RAND_MAX);
        sea.P[i].p.x = x;
        sea.P[i].p.y = y;
        sea.P[i].v.x = 0;
        sea.P[i].v.y = 0;
        if(i<sea.n_neg)
            sea.P[i].q = -Q; // negative charge
        else
            sea.P[i].q = Q; // positive charge
    }
    
    for(int i=0; i<50; i++){
        print_pos(&sea);
        time_step(&sea);
    }
    
    free(sea.P);
    
    return 0;
}
