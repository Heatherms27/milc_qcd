/*******************************************************************************
 *   PRIMME PReconditioned Iterative MultiMethod Eigensolver
 *   Copyright (C) 2005  James R. McCombs,  Andreas Stathopoulos
 *
 *   This file is part of PRIMME.
 *
 *   PRIMME is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   PRIMME is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: primme.h
 * 
 * Purpose - To be included in user applications that wish to call primme.
 * 
 ******************************************************************************/

#ifndef PRIMME_H
#define PRIMME_H

#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include "../include/Complex_primme.h"

#define PRIMME_MAX_NAME_LENGTH 128

#define PRIMME_INT long

typedef enum {
   primme_smallest,        /* leftmost eigenvalues */
   primme_largest,         /* rightmost eigenvalues */
   primme_closest_geq,     /* leftmost but greater than the target shift */
   primme_closest_leq,     /* rightmost but less than the target shift */
   primme_closest_abs,     /* the closest to the target shift */
   primme_largest_abs      /* the farthest to the target shift */
} primme_target;

/* projection methods for extraction */
typedef enum {
   primme_proj_default,
   primme_proj_RR,          /* Rayleigh-Ritz */
   primme_proj_harmonic,    /* Harmonic Rayleigh-Ritz */
   primme_proj_refined      /* refined with fixed target */
} primme_projection;

typedef enum {         /* Initially fill up the search subspace with: */
   primme_init_default,
   primme_init_krylov, /* a) Krylov with the last vector provided by the user or random */
   primme_init_random, /* b) just random vectors */
   primme_init_user    /* c) provided vectors or a single random vector */
} primme_init;


typedef enum {
   primme_thick,
   primme_dtr
} primme_restartscheme;


typedef enum {
   primme_full_LTolerance,
   primme_decreasing_LTolerance,
   primme_adaptive_ETolerance,
   primme_adaptive
} primme_convergencetest;


typedef struct primme_stats {
   PRIMME_INT numOuterIterations;
   PRIMME_INT numRestarts;
   PRIMME_INT numMatvecs;
   PRIMME_INT numPreconds;
   PRIMME_INT numGlobalSum;         /* times called globalSumReal */
   PRIMME_INT volumeGlobalSum;      /* number of SCALARs reduced by globalSumReal */
   double numOrthoInnerProds;       /* number of inner prods done by Ortho */
   double elapsedTime; 
   double timeMatvec;               /* time expend by matrixMatvec */
   double timePrecond;              /* time expend by applyPreconditioner */
   double timeOrtho;                /* time expend by ortho  */
   double timeGlobalSum;            /* time expend by globalSumReal  */
   double estimateMinEVal;          /* the leftmost Ritz value seen */
   double estimateMaxEVal;          /* the rightmost Ritz value seen */
   double estimateLargestSVal;      /* absolute value of the farthest to zero Ritz value seen */
   double maxConvTol;               /* largest norm residual of a locked eigenpair */
   double estimateResidualError;    /* accumulated error in V and W */
} primme_stats;
   
typedef struct JD_projectors {
   int LeftQ;
   int LeftX;
   int RightQ;
   int RightX;
   int SkewQ;
   int SkewX;
} JD_projectors;

typedef struct projection_params {
   primme_projection projection;
} projection_params;

typedef struct correction_params {
   int precondition;
   int robustShifts;
   int maxInnerIterations;
   struct JD_projectors projectors;
   primme_convergencetest convTest;
   double relTolBase;
} correction_params;


typedef struct restarting_params {
   primme_restartscheme scheme;
   int maxPrevRetain;
} restarting_params;
   

//-----------------------------------------------------------------------------
typedef struct primme_params {

   // The user must input at least the following two arguments 
   PRIMME_INT n;
   void (*matrixMatvec)
      ( void *x, PRIMME_INT *ldx, void *y, PRIMME_INT *ldy, int *blockSize,
        struct primme_params *primme, int *ierr);

   // Preconditioner applied on block of vectors (if available) 
   void (*applyPreconditioner)
      ( void *x, PRIMME_INT *ldx,  void *y, PRIMME_INT *ldy, int *blockSize,
        struct primme_params *primme, int *ierr);

   // Matrix times a multivector for mass matrix B for generalized Ax = xBl
   void (*massMatrixMatvec)
      ( void *x, PRIMME_INT *ldx, void *y, PRIMME_INT *ldy, int *blockSize,
        struct primme_params *primme, int *ierr);

   // input for the following is only required for parallel programs 
   int numProcs;
   int procID;
   PRIMME_INT nLocal;
   void *commInfo;
   void (*globalSumReal)
      (void *sendBuf, void *recvBuf, int *count, struct primme_params *primme,
       int *ierr );

   // Though primme_initialize will assign defaults, most users will set these
   int numEvals;          
   primme_target target; 
   int numTargetShifts;              // For targeting interior epairs,
   double *targetShifts;             // at least one shift must also be set

   /* the following will be given default values depending on the method */
   int dynamicMethodSwitch;
   int locking;
   int initSize;
   int numOrthoConst;
   int maxBasisSize;
   int minRestartSize;
   int maxBlockSize;
   PRIMME_INT maxMatvecs;
   PRIMME_INT maxOuterIterations;
   int intWorkSize;
   size_t realWorkSize;
   PRIMME_INT iseed[4];
   int *intWork;
   void *realWork;
   double aNorm;
   double eps;

   int printLevel;
   FILE *outputFile;
   
   void *matrix;
   void *preconditioner;
   double *ShiftsForPreconditioner;
   primme_init initBasisMode;
   PRIMME_INT ldevecs;
   PRIMME_INT ldOPs;

   struct projection_params projectionParams; 
   struct restarting_params restartingParams;
   struct correction_params correctionParams;
   struct primme_stats stats;
   
   void (*convTestFun)(double *eval, void *evec, double *rNorm, int *isconv, 
         struct primme_params *primme, int *ierr);
} primme_params;
//-----------------------------------------------------------------------------

typedef enum {
   PRIMME_DEFAULT_METHOD,
   PRIMME_DYNAMIC,
   PRIMME_DEFAULT_MIN_TIME,
   PRIMME_DEFAULT_MIN_MATVECS,
   PRIMME_Arnoldi,
   PRIMME_GD,
   PRIMME_GD_plusK,
   PRIMME_GD_Olsen_plusK,
   PRIMME_JD_Olsen_plusK,
   PRIMME_RQI,
   PRIMME_JDQR,
   PRIMME_JDQMR,
   PRIMME_JDQMR_ETol,
   PRIMME_SUBSPACE_ITERATION,
   PRIMME_LOBPCG_OrthoBasis,
   PRIMME_LOBPCG_OrthoBasis_Window
} primme_preset_method;

#undef PRIMME_INT

int dprimme(double *evals, double *evecs, double *resNorms, 
            primme_params *primme);
int zprimme(double *evals, Complex_Z *evecs, double *resNorms, 
            primme_params *primme);
void primme_initialize(primme_params *primme);
int  primme_set_method(primme_preset_method method, primme_params *params);
void primme_display_params(primme_params primme);
void *primme_valloc(size_t byteSize, const char *target);
void *primme_calloc(size_t nelem, size_t elsize, const char *target);
void primme_free(primme_params *primme);
void primme_seq_globalSumDouble(void *sendBuf, void *recvBuf, int *count,
					           primme_params *params);
/**
void primme_PushErrorMessage(const primme_function callingFunction, 
     const primme_function failedFunction, const int errorCode, 
     const char *fileName, const int lineNumber, primme_params *primme);
void primme_PrintStackTrace(const primme_params primme);
void primme_DeleteStackTrace(primme_params *primme);
**/

#endif /* PRIMME_H */
