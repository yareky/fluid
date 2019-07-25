#ifndef CFLUID_H_
#define CFLUID_H_

#include <stdio.h>
#include <algorithm>

class CFluid
{
private:
    float *data;
    int N, size;
    float h;

    void SwapU();
    void SwapV();
    void SwapDens();
    void SetBnd(int b, float * x);
    void AddSource(float * x, float * s, float dt);
    void Diffuse(int b, float * x, float * x0, float diff, float dt);
    void Project(float * u, float * v, float * p, float * div);
    void Advect(int b, float * d, float * d0, float * u, float * v, float dt);

public:
    float *u, *v, *u_prev, *v_prev;
    float *dens, *dens_prev;

    CFluid(int N)
    {
        this->N = N;
        size = (N+2)*(N+2);
        h = 1.0/N;

        data = new float[size*6];
        u = data;
        v = data + size;
        u_prev = data + 2*size;
        v_prev = data + 3*size;
        dens = data + 4*size;
        dens_prev = data + 5*size;
    }

    ~CFluid()
    {
        delete [] data;
        u = v = u_prev = v_prev = dens = dens_prev = NULL;
    }

    int GetN()
    {
        return N;
    }

    int IX(int i, int j)
    {
        return i+(N+2)*j;
    }


    void AddBlow(int x, int y);
    void AddGas(int x, int y, int r);
    void VelStep (float visc, float dt);
    void DensStep(float diff, float dt);
};


#endif // CFLUID_H_
