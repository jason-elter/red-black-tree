/**
 * @file Structs.c
 * @author  Jason Elter <jason.elter@mail.huji.ac.il>
 * @version 1.0
 * @date 10 December 2019
 *
 * @brief This file implements example functions to use with the red-black tree.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "RBTree.h"

#ifndef TA_EX3_STRUCTS_H
#define TA_EX3_STRUCTS_H

/**
 * Represents a vector. The double* should be dynamically allocated
 */
typedef struct Vector
{
    int len;
    double *vector;
} Vector;


/**
 * CompFunc for strings (assumes strings end with "\0")
 * @param a - char* pointer
 * @param b - char* pointer
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a. (lexicographic
 * order)
 */
int stringCompare(const void *a, const void *b)
{
    return strcmp((char *) a, (char *) b);
}

/**
 * ForEach function that concatenates the given word to pConcatenated. pConcatenated is already allocated with
 * enough space.
 * @param word - char* to add to pConcatenated
 * @param pConcatenated - char*
 * @return 0 on failure, other on success
 */
int concatenate(const void *word, void *pConcatenated)
{
    if (word == NULL || pConcatenated == NULL)
    {
        return 0;
    }

    strcat((char *) pConcatenated, (char *) word);
    strcat((char *) pConcatenated, "\n");
    return 1;
}

/**
 * FreeFunc for strings
 */
void freeString(void *s)
{
    if (s != NULL)
    {
        free((char *) s);
    }
}

/**
 * CompFunc for Vectors, compares element by element, the vector that has the first larger
 * element is considered larger. If vectors are of different lengths and identify for the length
 * of the shorter vector, the shorter vector is considered smaller.
 * @param a - first vector
 * @param b - second vector
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a.
 */
int vectorCompare1By1(const void *a, const void *b)
{
    Vector *vector1 = (Vector *) a, *vector2 = (Vector *) b;
    double *arr1 = vector1->vector, *arr2 = vector2->vector;
    int len1 = vector1->len, len2 = vector2->len;
    int minLen = (len1 < len2) ? len1 : len2;

    for (int i = 0; i < minLen; i++)
    {
        if (arr1[i] != arr2[i])
        {
            return (arr1[i] < arr2[i]) ? -1 : 1;
        }
    }

    if (len1 == len2)
    {
        return 0;
    }
    return (len1 < len2) ? -1 : 1;
}

/**
 * FreeFunc for vectors
 */
void freeVector(void *pVector)
{
    if (pVector != NULL)
    {
        Vector *vector = (Vector *) pVector;
        free(vector->vector);
        free(vector);
    }
}

// Helper function that returns the norm^2 of the vector(assumes valid vector).
static double getNormSquared(const Vector *vector)
{
    int len = vector->len;
    double *arr = vector->vector, norm = 0;
    for (int i = 0; i < len; i++)
    {
        norm += arr[i] * arr[i];
    }
    return norm;
}

/**
 * copy pVector to pMaxVector if : 1. The norm of pVector is greater then the norm of pMaxVector.
 * 								   2. pMaxVector == NULL.
 * @param pVector pointer to Vector
 * @param pMaxVector pointer to Vector
 * @return 1 on success, 0 on failure (if pVector == NULL: failure).
 */
int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    if (pVector == NULL || pMaxVector == NULL)
    {
        return 0;
    }

    Vector *vector = (Vector *) pVector, *maxVector = (Vector *) pMaxVector;
    int flag = (maxVector->vector != NULL && (getNormSquared(vector) > getNormSquared(maxVector)));
    if (flag)
    {
        free(maxVector->vector);
    }

    if (flag || maxVector->vector == NULL)
    {
        maxVector->len = vector->len;
        maxVector->vector = (double *) malloc(sizeof(double) * vector->len);
        if (maxVector->vector == NULL)
        {
            return 0;
        }

        memcpy(maxVector->vector, vector->vector, sizeof(double) * vector->len);
    }

    return 1;
}

/**
 * @param tree a pointer to a tree of Vectors
 * @return pointer to a *copy* of the vector that has the largest norm (L2 Norm).
 */
Vector *findMaxNormVectorInTree(RBTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    Vector *maxVector = (Vector *) malloc(sizeof(Vector));
    if (maxVector == NULL)
    {
        return NULL;
    }

    maxVector->vector = NULL;
    maxVector->len = 0;
    int success = forEachRBTree(tree, copyIfNormIsLarger, maxVector);
    return success ? maxVector : NULL;
}


#endif //TA_EX3_STRUCTS_H
