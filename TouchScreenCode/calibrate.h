#ifndef		_CALIBRATE_C_
  #define		EXTERN         extern
#else
  #define		EXTERN
#endif



#ifndef		OK
  #define		OK		        0
  #define		NOT_OK		   -1
#endif



#define			INT32				long




/****************************************************/
/*                                                  */
/* Structures                                       */
/*                                                  */
/****************************************************/

struct POINT { 
  INT32    x;
  INT32    y;
};


#define  MATRIX_Type  INT32

struct MATRIX {
  /* This arrangement of values facilitates 
  *  calculations within getDisplayPoint() 
  */
  MATRIX_Type    An;     /* A = An/Divider */
  MATRIX_Type    Bn;     /* B = Bn/Divider */
  MATRIX_Type    Cn;     /* C = Cn/Divider */
  MATRIX_Type    Dn;     /* D = Dn/Divider */
  MATRIX_Type    En;     /* E = En/Divider */
  MATRIX_Type    Fn;     /* F = Fn/Divider */
  MATRIX_Type    Divider ;
};

