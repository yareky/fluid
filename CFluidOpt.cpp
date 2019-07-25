#include "CFluidOpt.h"

void CFluidOpt::SwapU()
{
    float *t;
    t = u;
    u = u_prev;
    u_prev = t;
}

void CFluidOpt::SwapV()
{
    float *t;
    t = v;
    v = v_prev;
    v_prev = t;
}

void CFluidOpt::SwapDens()
{
    float *t;

    t = dens;
    dens = dens_prev;
    dens_prev = t;
}

void CFluidOpt::SetBnd(int b, float * x )
{

    for(int i=1 ; i<=N ; i++ )
    {
        x[IX(0 ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
        x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
        x[IX(i,0 )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
        x[IX(i,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
    }

    x[IX(0 ,0 )] = 0.5*(x[IX(1,0 )]+x[IX(0 ,1)]);
    x[IX(0 ,N+1)] = 0.5*(x[IX(1,N+1)]+x[IX(0 ,N )]);
    x[IX(N+1,0 )] = 0.5*(x[IX(N,0 )]+x[IX(N+1,1)]);
    x[IX(N+1,N+1)] = 0.5*(x[IX(N,N+1)]+x[IX(N+1,N )]);
}

void CFluidOpt::AddSource (float * x, float * s, float dt )
{
    for(int i=0 ; i<size ; i++ )
    x[i] += dt*s[i];
}


void CFluidOpt::Diffuse(int b, float * x, float * x0, float diff, float dt )
{
    int i, j, k;
    float a=dt*diff*N*N;

    for ( k=0 ; k<20 ; k++ )
    {
        for ( i=1 ; i<=N ; i++ )
        {
            for ( j=1 ; j<=N ; j++ )
            {
                x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)] + x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a);
            }
        }
        SetBnd(b, x );
    }
}

void CFluidOpt::Project(float * u, float * v, float * p, float * div )
{
    int i, j, k;

    for ( i=1 ; i<=N ; i++ )
    {
        for ( j=1 ; j<=N ; j++ )
        {
            div[IX(i,j)] = -0.5*h*(u[IX(i+1,j)]-u[IX(i-1,j)]+
            v[IX(i,j+1)]-v[IX(i,j-1)]);
            p[IX(i,j)] = 0;
        }
    }

    SetBnd(0, div ); SetBnd(0, p );

    for ( k=0 ; k<20 ; k++ )
    {
        for ( i=1 ; i<=N ; i++ )
        {
            for ( j=1 ; j<=N ; j++ )
            {
            p[IX(i,j)] = (div[IX(i,j)]+p[IX(i-1,j)]+p[IX(i+1,j)] + p[IX(i,j-1)]+p[IX(i,j+1)])/4;
            }
        }
        SetBnd(0, p );
    }

    for ( i=1 ; i<=N ; i++ )
    {
        for ( j=1 ; j<=N ; j++ )
        {
        u[IX(i,j)] -= 0.5*(p[IX(i+1,j)]-p[IX(i-1,j)])/h;
        v[IX(i,j)] -= 0.5*(p[IX(i,j+1)]-p[IX(i,j-1)])/h;
        }
    }

    SetBnd(1, u ); SetBnd(2, v );
}

void CFluidOpt::Advect(int b, float * d, float * d0, float * u, float * v, float dt )
{
    int i, j, i0, j0, i1, j1;

    float x, y, s0, t0, s1, t1, dt0;

    dt0 = dt*N;

    for ( i=1 ; i<=N ; i++ )
    {
        for ( j=1 ; j<=N ; j++ )
        {
            x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];

            if (x<0.5) x=0.5; if (x>N+0.5) x=N+ 0.5; i0=(int)x; i1=i0+1;
            if (y<0.5) y=0.5; if (y>N+0.5) y=N+ 0.5; j0=(int)y; j1=j0+1;

            s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;

            d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)]) + s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
        }
    }

    SetBnd(b, d );
}


 void CFluidOpt::AddBlow(int x, int y)
{
    u_prev[IX(x,y)] = (rand()%200)-100;
    v_prev[IX(x,y)] = (rand()%200)-100;

}

void CFluidOpt::AddGas(int x, int y, int r)
{
    r = r*r;
    int tx, ty;

    for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
    {
        tx = i-x;   ty = j-y;

        if(tx*tx+ty*ty < r)
        {
            dens_prev[IX(i,j)] = 10;
        }
    }
}

void CFluidOpt::VelStep (float visc, float dt )
{
    AddSource ( u, u_prev, dt ); AddSource ( v, v_prev, dt );
    SwapU(); Diffuse ( 1, u, u_prev, visc, dt );
    SwapV(); Diffuse ( 2, v, v_prev, visc, dt );
    Project(u, v, u_prev, v_prev );
    SwapU(); SwapV();
    Advect (1, u, u_prev, u_prev, v_prev, dt ); Advect (2, v, v_prev, u_prev, v_prev, dt );
    Project (u, v, u_prev, v_prev );
}


 void CFluidOpt::DensStep(float diff, float dt )
{
    AddSource(dens, dens_prev, dt );
    SwapDens(); Diffuse(0, dens, dens_prev, diff, dt );
    SwapDens(); Advect(0, dens, dens_prev, u, v, dt );

    for(int i = 0; i < size; i++)
    dens_prev[i] = 0;
}
