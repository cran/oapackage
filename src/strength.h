/*! \file strength.h
	\brief Contains code to perform a strength check on an OA

   Author: Pieter Eendebak <pieter.eendebak@gmail.com>
   Copyright: See LICENSE.txt file that comes with this distribution
*/

#ifndef STRENGTH_H
#define STRENGTH_H

#include "printfheader.h"
#include "tools.h"
#include "arraytools.h"
#include "arrayproperties.h"

#ifdef FULLPACKAGE
#include "extend.h"
#endif

struct rev_index;

typedef int freq_t;		/* used for counting t-tuples in strength check */
typedef freq_t** strength_freq_table;

strength_freq_table new_strength_freq_table ( int ncolcombs, int *nvalues, int &nelements );
//strength_freq_table new_strength_freq_table ( int ncolcombs, int *nvalues );


rev_index *create_reverse_colcombs_fixed ( const int ncolcombs );
rev_index *create_reverse_colcombs ( colindex_t **colcombs, const int ncols, const int strength );
void free_colcombs_fixed ( colindex_t **colcombs, int *lambda, int *nvalues );


/*!
  Structure serves as an index, gives the combinations in which a certain column participates. Is used to for the strength check, the program needs to know which column participate in the combination
  \brief Reverse Index
 */
struct rev_index {
	///Number of combination a column is involved in, equal to ncombs(n-1, k-1)
	int			nr_elements;
	///List of combination a column is involved in
	int			*index;
	///pointer to struct with more information+settings for combination
};

struct strength_check_t {
	int freqtablesize;
	strength_freq_table freqtable;
	vindex_t **indices;
	rev_index *r_index;

	colindex_t ** colcombs;
	int *nvalues;
	int *lambda;
	int ncolcombs;
	const int strength;

	strength_check_t ( int str ) : freqtable ( 0 ), indices ( 0 ), r_index ( 0 ), colcombs ( 0 ), nvalues ( 0 ),  lambda ( 0 ), strength ( str ) {} ;
	~strength_check_t() {
		free2d_irr ( freqtable );
		free2d ( indices );
		free_colcombs_fixed ( colcombs, lambda, nvalues );
		if ( r_index!=0 ) {
			free ( r_index->index );
			free ( r_index );
		}

	};

	void set_colcombs ( const arraydata_t &ad ) {
		int verbose=0;

		{
			int		prod;
			int n = ad.ncols;
			int N = ad.N;
			const carray_t *s=ad.s;

			int k = strength ;	// we keep 1 column fixed, choose k columns
			ncolcombs = ncombs ( n, k );

			if ( verbose )
				myprintf ( "strength_check_t: set_colcombs: ncolcombs %d, strength %d\n", ncolcombs, strength );
			this->colcombs = malloc2d<colindex_t> ( ncolcombs, strength );
			this->lambda = ( int* ) malloc ( ncolcombs * sizeof ( int ) );
			this->nvalues = ( int* ) malloc ( ncolcombs * sizeof ( int ) );

			//log_print(DEBUG, "ncolcombs: %d\n", ncolcombs);

			//set initial combination
			for ( int i = 0; i < k; i++ )
				colcombs[0][i] = i;


			for ( int i = 1; i < ncolcombs; i++ ) {
				memcpy ( colcombs[i], colcombs[i-1], k * sizeof ( colindex_t ) );
				next_combination<colindex_t> ( colcombs[i], k, n );

				prod = 1;
				for ( int j = 0; j < strength; j++ ) {
					if ( verbose>=2 )
						myprintf ( "i %d j %d: %d\n", i, j, colcombs[i][j] );
					prod *= s[colcombs[i][j]];
				}
				nvalues[i] = prod;
				lambda[i] = N/prod;
			}
			//return colcombs;

			prod = 1;	//First lambda manually, because of copy-for-loop
			for ( int j = 0; j < strength; j++ )
				prod *= s[colcombs[0][j]];
			nvalues[0] = prod;
			lambda[0] = N/prod;

			//return colcombs;
		}
	}


	void info() const {
		myprintf ( "strength_check_t: %d column combintations: \n", ncolcombs );
		for ( int i=0; i<ncolcombs; i++ ) {
			myprintf ( "   " );
			print_perm ( colcombs[i], strength );
		}

	}

	void create_reverse_colcombs_fixed() {
		r_index = ::create_reverse_colcombs_fixed ( ncolcombs );
	}

	void print_frequencies() const {
		register int	i, j;

		for ( i = 0; i < ncolcombs; i++ ) {
			myprintf ( "%i:\t", i );
			for ( j = 0; j < nvalues[i]; j++ ) {
				myprintf ( "%2i ", freqtable[i][j] );
			}
			myprintf ( "\n" );
		}
		myprintf ( "\n" );
	}

};

/** @brief Contains static data for the extend loop
 *
 */
struct  extend_data_t {

	/* static data: not changed during calculations */
	const arraydata_t *adata;
	const colindex_t extcolumn;
	rowindex_t oaindextmin;	/** index of t-1 columns */

	//! number of rows
	const rowindex_t N;

	//! column combinations used in strength check
	colindex_t **colcombs;

	int **indices;	/* todo: type? */

	int ncolcombs;	/** number of relevant column combinations */
	rev_index *r_index;		/** reverse pointer to column combinations */
	rev_index *r_index_total;

	//! index of each column
	int *lambda;

	//!
	int *nvalues;

	//! for row symmetry calculations
	rowindex_t *gidx;
	rowindex_t *gstart;
	rowindex_t *gsize;

	/* dynamic data: can be changed during extension */

	//! elements count, for strength 1 check
	int *elements;

	int freqtablesize;
	//! frequency table for strength check
	strength_freq_table freqtable;

	//! strength check, cache
	strength_freq_table *freqtable_cache;

	//! strength check, cache for each element
	int **freqtable_elem;

	//! used for settings the range
	array_t range_low, range_high;
	// the range is are the values that can be taken at a certain position in the array
	// rangemax is equal max(values in current col)+1
	// rangemin only occurs if row symmetries are used (define: USE_ROW_SYMMETRY)

	/* functions */

	extend_data_t ( const arraydata_t *ad, colindex_t extcol );
	~extend_data_t();

};



/** @brief Copy the frequency count table
 */
inline void copy_freq_table ( strength_freq_table source, strength_freq_table target, int ftsize )
{
	memcpy ( ( void * ) target[0], ( void * ) source[0], ftsize*sizeof ( freq_t ) );
}

/// check whether an array passes divisibility test
bool check_divisibility ( const arraydata_t * );


void print_frequencies ( int **frequencies, const int nelements, const int *lambda, const int N );

colindex_t **set_colcombs_fixed ( int *& xlambda, int *& nvalues, int &ncolcombs, const array_t *s, const int strength, const int fixedcol, const int N );

/// Add row to frequency table using cache system
void add_element_freqtable ( extend_data_t *es, rowindex_t activerow, carray_t *array, strength_freq_table freqtable );

/// Initialize the table of t-tuple frequencies
void init_frequencies ( extend_data_t *es, array_t *array );

void recount_frequencies ( int **frequencies, extend_data_t *es, colindex_t currentcol, rowindex_t rowstart, rowindex_t rowlast, carray_t *array );

/// perform strength check on an array
bool strength_check ( const arraydata_t &ad, const array_link &al, int verbose=1 );

template <class basetype>
/// Helper function
vindex_t **set_indices ( colindex_t **colcombs, basetype *bases, const int k, colindex_t ncolcombs )
{
	int		prod, **indices = 0;

	indices = malloc2d<int> ( ncolcombs, k );

	for ( int j = 0; j < ncolcombs; j++ ) {
		//numtype* init_valueindex_forward(numtype *valueindex, const numtype *bases, const numtype n)
		prod = 1;
		for ( int i = 0; i < k; i++ ) {
			indices[j][i] = prod;
			log_print ( DEBUG,  "indices[%i][%i] = %i\n", j, i, prod );
			prod *= bases[colcombs[j][i]];
		}
	}
	return indices;
}

/// perform strength check on an array
inline bool strength_check ( const array_link &al, int strength,  int verbose = 0 )
{
	if ( strength==0 )
		return true;


	arraydata_t ad = arraylink2arraydata ( al, 0, strength );
	strength_check_t strengthcheck ( strength );

	int oaindextmin = get_oaindex ( ad.s, ad.strength-1, ad.N );

	/* set column combinations with extending column fixed */

	if ( verbose>=2 )
		myprintf ( "strength_check array: N %d, k %d, strength %d\n", ad.N, al.n_columns, ad.strength );
	strengthcheck.set_colcombs ( ad );

	//myprintf ( "nvalues: " ); print_perm ( nvalues, strengthcheck.ncolcombs );
	strengthcheck.indices = set_indices ( strengthcheck.colcombs, ad.s, ad.strength, strengthcheck.ncolcombs );	//sets indices for frequencies, does the malloc as well

	strengthcheck.create_reverse_colcombs_fixed();

	int val=true;
	strengthcheck.freqtable = new_strength_freq_table ( strengthcheck.ncolcombs, strengthcheck.nvalues, strengthcheck.freqtablesize );

	if ( verbose>=2 )
		strengthcheck.info();

	if ( verbose>=2 ) {
		myprintf ( "before:\n" );
		strengthcheck.print_frequencies ( );
	}
//   myprintf ( "  table of size %d\n", strengthcheck.freqtablesize );
//   myprintf ( "  strength %d: %d\n", ad.strength, val );

	for ( int i=0; i<strengthcheck.ncolcombs; i++ ) {
		//myprintf ( "columns %d: ", i ); print_perm ( strengthcheck.colcombs[i], strength );

		for ( int r=0; r<ad.N; r++ ) {

			int valindex=0;
			for ( int t=0; t<ad.strength; t++ ) {
				colindex_t cc = strengthcheck.colcombs[i][t];
				int s = ad.s[cc];
				array_t val = al.array[cc*ad.N+r];
				valindex = valindex*s+val;
			}
			if ( verbose>=2 ) {
				myprintf ( "  row %d: ", r );
				myprintf ( " value index %d\n", valindex );
			}
			strengthcheck.freqtable[i][valindex]++;
		}

		for ( int j=0; j<strengthcheck.nvalues[i]; j++ ) {
			//    myprintf ( "strength: i %d, j %d: %d %d\n", i, j, strengthcheck.freqtable[i][j], nvalues[i] );
			if ( strengthcheck.freqtable[i][j]!=ad.N/strengthcheck.nvalues[i] ) {
				if ( verbose>=2 )
					myprintf ( "no good strength: i %d, j %d: %d %d\n", i, j, strengthcheck.freqtable[i][j], strengthcheck.nvalues[i] );
				val=false;
				break;
			}
		}

		if ( val==false )
			break;
	}
	//myprintf ( "nvalues: " ); print_perm ( nvalues, strengthcheck.ncolcombs );
	if ( verbose>=2 ) {
		myprintf ( "table of counted value pairs\n" );
		strengthcheck.print_frequencies ( );
	}

	return val;
}

#ifdef FULLPACKAGE
bool valid_element ( const extend_data_t *es, const extendpos *p, carray_t *array );
#endif

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
