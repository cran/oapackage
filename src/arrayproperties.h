/*! \file arrayproperties.h
 \brief Contains functions to calculate properties of arrays
 
 Author: Pieter Eendebak <pieter.eendebak@gmail.com>
 Copyright: See LICENSE.txt file that comes with this distribution
*/

#ifndef ARRAYPROPERTIES_H
#define ARRAYPROPERTIES_H


#include <Eigen/Core>
#include <Eigen/SVD>
//#include <Eigen/Dense>

#include "oaoptions.h"
#include "arraytools.h"
#include "tools.h"


#define stringify( name ) # name

/// calculate determinant of X^T X by using the SVD
double detXtX(const Eigen::MatrixXd &mymatrix, int verbose=1);
double detXtXfloat(const Eigen::MatrixXf &mymatrix, int verbose=1);

/// Calculate D-efficiency and VIF-efficiency and E-efficiency values using SVD
void DAEefficiecyWithSVD(const Eigen::MatrixXd &x, double &Deff, double &vif, double &Eeff, int &rank, int verbose);

/// Calculate the rank of the second order interaction matrix of an orthogonal array, the rank, D-efficiency, VIF-efficiency and E-efficiency are appended to the second argument
int array_rank_D_B(const array_link &al, std::vector<double> *ret = 0, int verbose=0);

/// Calculate D-efficiency for a 2-level array using symmetric eigenvalue decomposition
double Defficiency(const array_link &al, int verbose=0);

std::vector<double> Defficiencies ( const array_link &al, const arraydata_t & arrayclass, int verbose, int addDs0 );

/// Calculate VIF-efficiency of matrix
double VIFefficiency(const array_link &al, int verbose=0);

/// Calculate A-efficiency of matrix
double Aefficiency(const array_link &al, int verbose=0);

/// Calculate E-efficiency of matrix (1 over the VIF-efficiency)
double Eefficiency(const array_link &al, int verbose=0);


#ifdef FULLPACKAGE
/// Return the D-efficiencies for the projection designs
std::vector<double> projDeff(const array_link &al, int kp, int verbose);

/// Return the projection estimation capacity sequence of a design
std::vector<double> PECsequence(const array_link &al, int verbose=0);
#endif


/// Return the distance distribution of a design
std::vector<double> distance_distribution(const array_link &al);

/// Calculate J-characteristics of matrix
std::vector<int> Jcharacteristics(const array_link &al, int jj=4, int verbose=0);

/** @brief calculate GWLP (generalized wordlength pattern)
 * 
 *  The method used for calculation is from Xu and Wu (2001), "Generalized minimum aberration for asymmetrical fractional factorial desings"
 *  The non-symmetric arrays see "Algorithmic Construction of Efficient Fractional Factorial Designs With Large Run Sizes", Xu
 * 
 */
std::vector<double> GWLP(const array_link &al, int verbose=0, int truncate=1);

std::vector<double> GWLPmixed(const array_link &al, int verbose=0, int truncate=1);


// SWIG has some issues with typedefs, so we use a define
//typedef double GWLPvalue;
//typedef mvalue_t<double> GWLPvalue;
#define GWLPvalue mvalue_t<double>

typedef mvalue_t<double> DOFvalue;


/// calculate delete-one-factor GWLP (generalized wordlength pattern) projections
std::vector< GWLPvalue > projectionGWLPs ( const array_link &al );

std::vector< GWLPvalue > sortGWLP ( std::vector< GWLPvalue > );

/// calculate delete-one-factor GWLP (generalized wordlength pattern) projection values
std::vector<double> projectionGWLPvalues ( const array_link &al );

/** calculate centered L2-discrepancy 
 * 
 * The method is from "A connection between uniformity and aberration in regular fractions of two-level factorials", Fang and Mukerjee, 2000
 */
double CL2discrepancy(const array_link &al);

/// add second order interactions to an array
array_link array2xf(const array_link &al);

/// calculate the rank of an array
int arrayrank(const array_link &al);


#ifdef FULLPACKAGE

#include "pareto.h"

// Calculate the Pareto optimal desings from a list of arrays
Pareto<mvalue_t<long>,long> parsePareto(const arraylist_t &arraylist, int verbose);

template <class IndexType>
/** Add array to list of Pareto optimal arrays
 * 
 * The values to be optimized are:
 * 
 * 1) Rank (higher is better)
 * 2) A3, A4 (lower is better)
 * 3) F4 (?? is better, sum of elements is constant)
 * 
 * */
inline typename Pareto<mvalue_t<long>,IndexType>::pValue calculateArrayPareto ( const array_link &al, int verbose  )
{
   int N = al.n_rows;
   //int k = al.n_columns;

   typename Pareto<mvalue_t<long>,IndexType>::pValue p;
   std::vector<double> gwlp = al.GWLP();
   long w3 = 0;
   if (gwlp.size()>3) w3 = N*N*gwlp[3]; // the maximum value for w3 is N*choose(k, jj)
   long w4 = 0;
   if (gwlp.size()>4) w4 = N*N*gwlp[4]; // the maximum value for w3 is N*choose(k, jj)
   //long xmax=N*ncombs ( k, 4 );
   std::vector<long> w;
   w.push_back ( w3 );
   w.push_back ( w4 ); // )); = xmax*w3+w4;

   mvalue_t<long> wm ( w, mvalue_t<long>::LOW );
	
   if ( verbose>=3 ) {
      myprintf ( "parseArrayPareto: A4 (scaled) %ld, %f\n", w4, gwlp[4] );
   }

   jstruct_t js ( al, 4 );
   std::vector<int> FF=js.calculateF();
#ifdef FULLPACKAGE
   if ( verbose>=3 ) {
      printf ( "  parseArrayPareto: F (high to low): " );
      display_vector ( FF );
      std::cout << std::endl;
	  //std::vector<int> Fval=js.Fval();
      //display_vector ( Fval ); std::cout << std::endl;
   }
#endif

   // long v = F2value ( FF );
   mvalue_t<long> v ( FF, mvalue_t<long>::LOW );

   // add the 3 values to the combined value
   p.push_back ( al.rank() ); // rank
   p.push_back ( wm ); // A4
   p.push_back ( v ); // F

   	if (verbose>=2) {
	  myprintf("  parseArrayPareto: rank %d, verbose %d\n", al.rank(), verbose );
	}

  return p;
  
}

template <class IndexType>
/** Add array to list of Pareto optimal arrays
 * 
 * The values to be optimized are:
 * 
 * 1) Rank (higher is better)
 * 2) A3, A4 (lower is better)
 * 3) F4 (?? is better, sum of elements is constant)
 * 
 * */
inline void parseArrayPareto ( const array_link &al, IndexType i, Pareto<mvalue_t<long>,IndexType> & pset, int verbose )
{
   typename Pareto<mvalue_t<long>,IndexType>::pValue p = calculateArrayPareto<IndexType>(al, verbose);

   // add the new tuple to the Pareto set
   pset.addvalue ( p, i );
}

#endif // FULLPACKAGE

/// convert C value to D-efficiency value
inline double Cvalue2Dvalue ( double C, int ka )
{
    double ma = 1 + ka + ka* ( ka-1 ) /2;
    double A= pow ( C, 1./ma );

    return A;
}

/// convert D-efficiency value to C value
inline double Dvalue2Cvalue ( double A, int ka )
{
    int ma = 1 + ka + ka* ( ka-1 ) /2;
    double C= pow ( A, ma );

    return C;
}

/// Return index of an array
inline int get_oaindex(const array_t *s, const colindex_t strength, const colindex_t N)
{
    int oaindex = N;
    for(colindex_t z=0; z<strength; z++)
        oaindex /= s[z];

    return oaindex;
}


#endif // ARRAYPROPERTIES_H

