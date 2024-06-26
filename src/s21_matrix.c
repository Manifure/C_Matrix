#include "s21_matrix.h"

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int err = 0;
  if (rows < 1 || columns < 1) err = 1;
  if (err == 0) {
    result->rows = rows;
    result->columns = columns;
    result->matrix = calloc(rows, sizeof(double *));
    if (result->matrix != NULL) {
      for (int i = 0; i < rows; i++) {
        result->matrix[i] = calloc(columns, sizeof(double));
      }
    } else {
      err = 1;
      free(result->matrix);
    }
  }
  return err;
}

void s21_remove_matrix(matrix_t *A) {
  if (A->matrix && A->rows > 0 && A->columns > 0) {
    for (int i = 0; i < A->rows; i++) {
      free(A->matrix[i]);
    }
    free(A->matrix);
    A->matrix = NULL;
  }
  if (A->rows) {
    A->rows = 0;
  }
  if (A->columns) {
    A->columns = 0;
  }
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
  int status = 0;
  if ((A->matrix && B->matrix) && A->rows == B->rows &&
      A->columns == B->columns) {
    status = SUCCESS;
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        if (fabs(A->matrix[i][j] - B->matrix[i][j]) > 1e-6) {
          status = FAILURE;
        }
      }
    }
  }
  return status;
}

int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = 0;
  if (A->matrix && B->matrix) {
    if (A->rows == B->rows && A->columns == B->columns) {
      status = s21_create_matrix(A->rows, A->columns, result);
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
        }
      }

    } else {
      status = 2;
    }

  } else {
    status = 1;
  }
  return status;
}

int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = 0;
  if (A->matrix && B->matrix) {
    if (A->rows == B->rows && A->columns == B->columns) {
      status = s21_create_matrix(A->rows, A->columns, result);
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
        }
      }

    } else {
      status = 2;
    }

  } else {
    status = 1;
  }
  return status;
}

int s21_mult_number(matrix_t *A, double number, matrix_t *result) {
  int status = 0;
  if (A->matrix) {
    status = s21_create_matrix(A->rows, A->columns, result);
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] * number;
      }
    }
  } else {
    status = 1;
  }
  return status;
}

int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = 0;
  if (A->matrix && B->matrix) {
    if (A->columns == B->rows) {
      status = s21_create_matrix(A->rows, B->columns, result);
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->columns; j++) {
          for (int k = 0; k < B->rows; k++) {
            result->matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
          }
        }
      }
    } else {
      status = 2;
    }
  } else {
    status = 1;
  }
  return status;
}

int s21_transpose(matrix_t *A, matrix_t *result) {
  int status = 0;
  if (A->matrix) {
    if (A->columns != NAN) {
      status = s21_create_matrix(A->columns, A->rows, result);
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[j][i] = A->matrix[i][j];
        }
      }
    } else {
      status = 2;
    }
  } else {
    status = 1;
  }
  return status;
}

int s21_determinant(matrix_t *A, double *result) {
  int err = 1;
  if (A->matrix) {
    err = 2;
    if (A->rows == A->columns) {
      err = 0;
      *result = A->matrix[0][0];
      if (A->rows != 1) {
        *result = s21_determinant_recursive(A);
      }
    }
  }
  return err;
}

double s21_determinant_recursive(matrix_t *A) {
  double result = 0;
  if (A->rows == 2) {
    result =
        A->matrix[0][0] * A->matrix[1][1] - A->matrix[0][1] * A->matrix[1][0];
  } else {
    for (int i = 0; i < A->rows; i++) {
      matrix_t minor;
      Minor(1, i + 1, A, &minor);
      result +=
          pow((-1), i) * A->matrix[0][i] * s21_determinant_recursive(&minor);
      s21_remove_matrix(&minor);
    }
  }
  return result;
}

int Minor(int row, int column, matrix_t *A, matrix_t *result) {
  int err = 1;
  if (A->matrix != NULL) {
    err = s21_create_matrix(A->rows - 1, A->columns - 1, result);
    if (err == 0) {
      int m, n;
      for (int i = 0; i < A->rows; i++) {
        m = i;
        if (i > row - 1) {
          m--;
        }
        for (int j = 0; j < A->columns; j++) {
          n = j;
          if (j > column - 1) {
            n--;
          }
          if (i != row - 1 && j != column - 1) {
            result->matrix[m][n] = A->matrix[i][j];
          }
        }
      }
    }
  }
  return err;
}

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  int err = 1;
  if (A->matrix) {
    err = 2;
    if (A->rows == A->columns) {
      err = s21_create_matrix(A->rows, A->columns, result);
      if (err == 0) {
        err = calc_helper(A, result);
      }
    }
  }
  return err;
}

int calc_helper(matrix_t *A, matrix_t *result) {
  int err = 0;
  result->matrix[0][0] = 1;
  if (A->rows != 1) {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        double deted;
        matrix_t minored;
        err = Minor(i + 1, j + 1, A, &minored);
        if (err == 0) {
          err = s21_determinant(&minored, &deted);
          if (err == 0) {
            result->matrix[i][j] = pow((-1), i + j) * deted;
          }
        }
        s21_remove_matrix(&minored);
      }
    }
  }
  return err;
}

int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
  int err = 1;
  if (A->matrix) {
    err = 2;
    double det;
    s21_determinant(A, &det);
    if (fabs(det - 0) > 1e-6) {
      matrix_t tmp_calc;
      err = s21_calc_complements(A, &tmp_calc);
      if (err == 0) {
        matrix_t tmp_trans;
        err = s21_transpose(&tmp_calc, &tmp_trans);
        if (err == 0) {
          s21_mult_number(&tmp_trans, 1 / det, result);
        }
        s21_remove_matrix(&tmp_trans);
      }
      s21_remove_matrix(&tmp_calc);
    }
  }
  return err;
}
