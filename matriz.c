#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matriz.h"

/**
 * Função que gera valores para para ser usado em uma matriz
 * @param i,j coordenadas do elemento a ser calculado (0<=i,j<n)
*  @return valor gerado para a posição i,j
  */
static inline double generateRandomA( unsigned int i, unsigned int j)
{
  static double invRandMax = 1.0 / (double)RAND_MAX;
  return ( (i==j) ? (double)(BASE<<1) : 1.0 )  * (double)rand() * invRandMax;
}

/**
 * Função que gera valores aleatórios para ser usado em um vetor
 * @return valor gerado
 *
 */
static inline double generateRandomB( )
{
  static double invRandMax = 1.0 / (double)RAND_MAX;
  return (double)(BASE<<2) * (double)rand() * invRandMax;
}



/* ----------- FUNÇÕES ---------------- */

/**
 *  Funcao geraMatPtr: gera matriz como vetor de ponteiros para as suas linhas
 *
 *  @param m  número de linhas da matriz
 *  @param n  número de colunas da matriz
 *  @param zerar se 0, matriz  tem valores aleatórios, caso contrário,
 *               matriz tem valores todos nulos
 *  @return  ponteiro para a matriz gerada
 *
 */

MatPtr geraMatPtr (int m, int n, int zerar)
{
  MatPtr matriz = (double **) malloc(m * sizeof(double *));

  if (matriz) {
    for (int i=0; i < m; ++i) {
      if (matriz[i] = (double *) malloc(n * sizeof(double)))
        for (int j=0; matriz[i] && j < n; ++j) {
          if (zerar) matriz[i][j] = 0.0;
          else       matriz[i][j] = generateRandomA(i, j);
        }
      else
          return NULL;
    }
  }

  return (matriz);
}

/**
 *  \brief: libera matriz alocada como vetor de ponteiros para as suas linhas
 *
 *  @param  ponteiro para matriz
 *
 */
void liberaMatPtr (MatPtr matriz, int m)
{
  if (matriz) {
    for (int i=0; i < m; ++i) {
      free (matriz[i]);
    }
    free (matriz);
  }
}



/**
 *  Funcao geraMatRow: gera matriz como vetor único, 'row-oriented'
 *
 *  @param m     número de linhas da matriz
 *  @param n     número de colunas da matriz
 *  @param zerar se 0, matriz  tem valores aleatórios, caso contrário,
 *               matriz tem valores todos nulos
 *  @return  ponteiro para a matriz gerada
 *
 */

MatRow geraMatRow (int m, int n, int zerar)
{
  MatRow matriz = (double *) malloc(m*n*sizeof(double));

  if (matriz) {
    for (int i=0; i < m; ++i) {
      for (int j=0; j < n; ++j) {
        if (zerar) matriz[i*m + j] = 0.0;
        else       matriz[i*m + j] = generateRandomA(i, j);
      }
    }
  }

  return (matriz);
}


/**
 *  Funcao geraVetor: gera vetor de tamanho 'n'
 *
 *  @param n  número de elementos do vetor
 *  @param zerar se 0, vetor  tem valores aleatórios, caso contrário,
 *               vetor tem valores todos nulos
 *  @return  ponteiro para vetor gerado
 *
 */

Vetor geraVetor (int n, int zerar)
{
  Vetor vetor = (double *) malloc(n*sizeof(double));

  if (vetor)
    for (int i=0; i < n; ++i) {
      if (zerar) vetor[i] = 0.0;
      else       vetor[i] = generateRandomB();
    }

  return (vetor);
}

/**
 *  \brief: libera vetor
 *
 *  @param  ponteiro para vetor
 *
 */
void liberaVetor (void *vet)
{
        free(vet);
}


/**
 *  Funcao multMatRowVet:  Efetua multiplicacao entre matriz 'mxn' por vetor
 *                       de 'n' elementos
 *  @param mat matriz 'mxn'
 *  @param m número de linhas da matriz
 *  @param n número de colunas da matriz
 *  @param res vetor que guarda o resultado. Deve estar previamente alocado e com
 *             seus elementos inicializados em 0.0 (zero)
 *  @return vetor de 'm' elementos
 *
 */

void multMatRowVet (MatRow mat, Vetor v, int m, int n, Vetor res)
{
  /* Efetua a multiplicação */
  if (res) {
    for (int i=0; i < m; ++i)
      for (int j=0; j < n; ++j)
        res[i] += mat[m*i + j] * v[j];
  }
}

/* 
Otimizações feitas:
- Unroll & Jam
- Uso de constantes em ponteiros e valores imutáveis ao longo do código
- Passagem dos parâmetros m e n por referência
- Localização de variáveis para serem armazenadas em registrador
- Cálculos de índice são guardados em variáveis para não precisarem ser recalculados
- Uso de unsigned int ao invés de int
- Uso de restrict nos ponteiros
*/
void multMatRowVet_otimiz (const MatRow restrict mat, const Vetor restrict v, const unsigned int *m, const unsigned int *n, const Vetor restrict res)
{
  if (!res)
    return;

  const unsigned int M = *m, N = *n;
  const unsigned stride = 8;
  const unsigned limit = M - M % stride;

  unsigned int i, j, op1, op2;
  double element;
  
  for (i = 0; i < limit; i += stride)
  {
    op1 = M * i;
    for (j=0; j < N; ++j)
    {
      element = v[j];
      op2 = op1 + j;
      res[i]     += mat[op2]     * element;
      res[i + 1] += mat[op2 + 1] * element;
      res[i + 2] += mat[op2 + 2] * element;
      res[i + 3] += mat[op2 + 3] * element;
      res[i + 4] += mat[op2 + 4] * element;
      res[i + 5] += mat[op2 + 5] * element;
      res[i + 6] += mat[op2 + 6] * element;
      res[i + 7] += mat[op2 + 7] * element;
    }
  }

  for (i = limit; i < M; ++i)
  {
    op1 = M * i;
    for (j=0; j < N; ++j)
      res[i] += mat[op1 + j] * v[j];
  }
}


/**
 *  Funcao multMatMatPtr: Efetua multiplicacao de duas matrizes 'n x n'
 *  @param A matriz 'n x n'
 *  @param B matriz 'n x n'
 *  @param n ordem da matriz quadrada
 *  @param C   matriz que guarda o resultado. Deve ser previamente gerada com 'geraMatPtr()'
 *             e com seus elementos inicializados em 0.0 (zero)
 *
 */


/**
 *  Funcao multMatMatPtr: Efetua multiplicacao de duas matrizes 'n x n' 
 *  @param A matriz 'n x n'
 *  @param B matriz 'n x n'
 *  @param n ordem da matriz quadrada
 *  @param C   matriz que guarda o resultado. Deve ser previamente gerada com 'geraMatPtr()'
 *             e com seus elementos inicializados em 0.0 (zero)
 *
 */

//#################################AQUI ESSA AQUI O ESSA AQUI###################################################################

void multMatMatRow (MatRow A, MatRow B, int n, MatRow C)
{

  /* Efetua a multiplicação */
  for (int i=0; i < n; ++i)
    for (int j=0; j < n; ++j)
      for (int k=0; k < n; ++k)
	C[i*n+j] += A[i*n+k] * B[k*n+j];
}

/* 
Otimizações feitas:
- Unroll & Jam com blocagem
- Uso de constantes em ponteiros e valores imutáveis ao longo do código
- Passagem do parâmetro n por referência
- Localização de variáveis para serem armazenadas em registrador
- Cálculos de índice são guardados em variáveis para não precisarem ser recalculados
- Uso de unsigned int ao invés de int
- Uso de restrict nos ponteiros para as matrizes
- Uso de shift nas operações de multiplicação
*/
void multMatMatRow_otimiz (const MatRow restrict A, const MatRow restrict B, const unsigned int *n, const MatRow restrict C)
{
  unsigned int i, j, k, ii, jj, kk, op1, op2, op3, op4, istart, iend, kstart, kend, jstart, jend;
  const unsigned int N = *n, block_size = 8, stride = 8, div = N >> 3;

  for (ii=0; ii < div; ++ii)
  {
    istart= ii << 3; iend= istart + block_size;
    for (jj = 0; jj < div; ++jj)
    {
      jstart = jj << 3;
      jend = jstart + block_size;
      for (kk = 0; kk < div; ++kk)
      {
        kstart = kk << 3;
        kend = kstart + block_size;
        for (i = istart; i < iend; ++i)
          op1 = i * N;
          for (j = jstart; j < jend; j += stride)
          {
            op2 = op1 + j;

            C[op2] = C[op2+1] = C[op2+2] = C[op2+3] =
            C[op2+4] = C[op2+5] = C[op2+6] = C[op2+7] = 0.0;

            for (k = kstart; k < kend; ++k)
            {
              op3 = op1 + k;
              op4 = k * N + j;
              C[op2]   += A[op3] * B[op4];
              C[op2+1] += A[op3] * B[op4+1];
              C[op2+2] += A[op3] * B[op4+2];
              C[op2+3] += A[op3] * B[op4+3];
              C[op2+4] += A[op3] * B[op4+4];
              C[op2+5] += A[op3] * B[op4+5];
              C[op2+6] += A[op3] * B[op4+6];
              C[op2+7] += A[op3] * B[op4+7];
            }
          }
      }
    }
  }
}


/**
 *  Funcao prnMatPtr:  Imprime o conteudo de uma matriz em stdout
 *  @param mat matriz
 *  @param m número de linhas da matriz
 *  @param n número de colunas da matriz
 *
 */

void prnMatPtr (MatPtr mat, int m, int n)
{
  for (int i=0; i < m; ++i) {
    for (int j=0; j < n; ++j)
      printf(DBL_FIELD, mat[i][j]);
    printf("\n");
  }
  printf(SEP_RES);
}

/**
 *  Funcao prnMatRow:  Imprime o conteudo de uma matriz em stdout
 *  @param mat matriz
 *  @param m número de linhas da matriz
 *  @param n número de colunas da matriz
 *
 */

void prnMatRow (MatRow mat, int m, int n)
{
  for (int i=0; i < m; ++i) {
    for (int j=0; j < n; ++j)
      printf(DBL_FIELD, mat[m*i + j]);
    printf("\n");
  }
  printf(SEP_RES);
}

/**
 *  Funcao prnVetor:  Imprime o conteudo de vetor em stdout
 *  @param vet vetor com 'n' elementos
 *  @param n número de elementos do vetor
 *
 */

void prnVetor (Vetor vet, int n)
{
  for (int i=0; i < n; ++i)
    printf(DBL_FIELD, vet[i]);
  printf(SEP_RES);
}

