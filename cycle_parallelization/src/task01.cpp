#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

namespace
{
  const int Isize = 1000;
  const int Jsize = 1000;

  const char* Result_filename = "result.txt";
}

int main()
{
  double a[Isize][Jsize];

  //подготовительная часть – заполнение некими данными
  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      a[i][j] = 10 * i + j;
    }
  }

  // требуется обеспечить измерение времени работы данного цикла
  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      a[i][j] = sin(2 * a[i][j]);
    }
  }

  FILE* ff = fopen(Result_filename, "w");
  assert(ff != NULL);
  
  for(int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      fprintf(ff,"%f ",a[i][j]);
    }
    
    fprintf(ff,"\n");
  }
  
  int res = fclose(ff);
  assert(res == 0);
}
