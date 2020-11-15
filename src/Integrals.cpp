/*
Implementación de la clase Integration
*/

#include <iostream>
#include <cmath>
#include "../include/Integrals.h"

using namespace std;

//constructor 
Integrals::Integrals( void )
{
		//lo que se necesite inicializar

}

/*--------------------
Función que realiza integrales 2D
INPUTS: Triangulo de la clase Triangle, F(x, y) a integrar
OUTPUT: Valor de la integral de F(x, y) en el Triangulo
----------------------*/
double Integrals::Integration2D(Triangle &Tr, function<double(const double &_x, const double &_y)> & toInt)
{
    double xa, xb, yc;
    function<double(const double&)> yd; 
    set2D_params(Tr, xa, xb, yc, yd);
    //llamamos integrador
    double result;
    result = Gaussian_quad2D(toInt, xa, xb, yc, yd);
    return result;
}


//fprime(x) = (f(x+dx) - f(x-dx)) / (2*dx);
Point Integrals::derivativeS(function<Point(const double &t_ )> SE, const double & t)
{
	double dt = 1e-9;
	Point p2 = SE(t-dt);
	Point p1 = SE(t+dt);
	double xprime = (p2.getX()-p1.getX())/(2*dt);
	double yprime = (p2.getY()-p1.getY())/(2*dt);
	return Point(xprime, yprime);
}

/*----------------------
Función que realiza integral 2D de linea sobre contorno parametrizado
INPUTS: F(x, y) a integrar, SE(t) contorno parametrizado, valores extremos ta y tb
OUTPUTS: double - valor de la integral de línea sobre el contorno
----------------------*/
double Integrals::lineIntegration(function<double(const double &_x, const double &_y)> & toInt, function<Point(const double &t_ )> &SE, const double ta, const double tb)
{
	//Ingresar al integrador 
	double result;
	function<double(const double&)> f = [this, toInt, SE](const double & t){
		double al = this->derivativeS(SE, t).length();
		Point p = SE(t); 
		return toInt(p.getX(), p.getY())*al;
	};

	result = Simpsons_comp1D(f, ta, tb);

	return result;
}

/* ---------------------
Integrador 2D usando el método cuadratura Gaussiana
INPUTS: función 2D (integrando), límites a y b númericos, límites funcionales c y d
OUTPUT: aproximación númerica al valor de la integral en el intervalo dado
-----------------------*/ 
double Integrals::Gaussian_quad2D(function<double(const double &_x, const double &_y)> & Function,
				double a, double b, double c, function<double(const double &yy_)> & Fd)
{
	int m = 5 - 1;
	int n = 5 - 1; 
	double x, y, d1, k1, k2, JX, Q;

	double h1 = (b-a)/2.0;
	double h2 = (b+a)/2.0;
	double J = 0;
	

	for ( int i = 0; i<=m; i++)
	{
		JX = 0;
		x = h1*r_ij[m][i] + h2;
		d1 = Fd(x);
		k1 = (d1 - c)/2.0;
		k2 = (d1 + c)/2.0;

		for ( int j = 0; j<=n; j++)
		{
			y = k1*r_ij[n][j] + k2;
			Q = Function( x, y );
			JX += C_ij[n][j]*Q;
		}

		J += C_ij[m][i]*k1*JX;
	}

	J = h1*J;

	return J;
}


/*----------------------
Función que establece los límites de integración 2D basados en el triangulo dado
INPUTS: vertices x1,y1 x2,y2 x3,y3 del triangulo 
OUTPUT: Ninguno - establece en variables privadas los límites
------------------------*/
void Integrals::set2D_params(Triangle &Tr, double& a, double &b, double &c, function<double(const double&)> &yd )
{
	double x2, x3, y2, y3;
	x2 = Tr.getV2().getX();
	y2 = Tr.getV2().getY();
	x3 = Tr.getV3().getX();
	y3 = Tr.getV3().getY();
	
	double d;
	if(x3 > x2)
	{
		a = x2;
		b = x3;
	}
	
	else
	{
		a = x3;
		b = x2;
	}
	
	if (y3 > y2)
	{
		c = y2;
		d = y3;
	}
	else{
		c = y3;
		d = y2;
	}
	
	
	yd = [&a, &b, &c, &d](const double & x) { double m = (d-c)/(b-a); double b = c - m*a; return m*x + b; };

}

/*---------------------
Integrador numerico 1D
INPUTS: función a integrar F(x), limites a,b
OUTPUTS: double - valor de la integral de F(x) en el intervalo [a,b] 
---------------------*/
double Integrals::Simpsons_comp1D(function<double(const double &)> & func, double a, double b)
{
	int n = 100;
	double X, XI;

	double h = ( b - a )/n;
	double XI0 = func(a) + func(b);
	double XI1 = 0;
	double XI2 = 0;

	for (int i=0; i < n; i++)
	{
		X = a + i*h;

		if ( i%2 == 0 )
		{
			XI2 += func(X);
		}
		else
		{
			XI1 += func(X);
		}
	}
	XI = h*(XI0 + 2*XI2 + 4*XI1)/3.0;

	return XI;
}