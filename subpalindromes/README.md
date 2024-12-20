### Поиск всех подпалиндромов в заданной строке с использование **OpenMP**.

#### Постановка задачи

Необходимо разработать однопоточную и многопоточную реализации алгоритма, осуществляющего поиск всех подпалиндромов в строке. Многопоточная реализация должна быть выполнена с использованием **OpenMP**.

#### OpenMP

**OpenMP** (Open Multi-Processing) — это стандарт, определяющий набор директив компилятора, библиотечных процедур и переменных среды окружения для создания многопоточных программ
использующих общую память.

#### Реализация

В качестве версий реализации алгоритма были рассмотрены:

1. Тривиальный алгоритм. Это алгоритм, который для поиска ответа в позиции ``idx`` раз за разом пробует увеличить ответ на единицу, каждый раз сравнивая пару соответствующих символов. Такой алгоритм слишком медленен, весь ответ он может посчитать лишь за время *O(n^2)*.

```cpp
template <typename CharT>
subpali_info find_subpalindromes_trivial(const std::basic_string<CharT>& source)
{
  auto len = source.size();
  subpali_info results(len);

  /* pragma's are here */
  for (std::size_t idx = 0U; idx != len; ++idx)
  {
    results[idx].odd  = 1U;
    results[idx].even = 0U;

    // Odd-length subpalindromes
    while (idx >= results[idx].odd && idx + results[idx].odd < len
           && source[idx - results[idx].odd] == source[idx + results[idx].odd])
    {
      ++results[idx].odd;
    }

    // Even-length subpalindromes
    while (idx >= results[idx].even + 1U && idx + results[idx].even < len
           && source[idx - results[idx].even - 1U] == source[idx + results[idx].even])
    {
      ++results[idx].even;
    }
  }

  return results;
}
```

2. Алгоритм Манакера. Для быстрого вычисления будем поддерживать границы ``l`` и ``r`` самого правого из обнаруженных подпалиндрома (т.е. подпалиндрома с наибольшим значением ``r``). Изначально можно положить ``l=0``, ``r=-1``.

```cpp
template <typename CharT>
subpali_info find_subpalindromes_manaker(const std::basic_string<CharT>& source)
{
  subpali_info results(source.size());
  auto num = static_cast<intmax_t>(source.size());

  // Odd-length subpalindromes
  {
    // Left and right borders of the rightmost subpalindrome
    intmax_t l = 0, r = -1;

    for (intmax_t idx = 0; idx < num; ++idx)
    {
      intmax_t k =
        (idx > r) ? 1 : std::min(static_cast<intmax_t>(results[l + r - idx].odd), r - idx + 1);

      while (idx + k < num && idx >= k && source[idx + k] == source[idx - k])
      {
        ++k;
      }

      results[idx].odd = k;
      if (idx + k - 1 > r)
      {
        // Update left and right borders
        l = idx - k + 1;
        r = idx + k - 1;
      }
    }
  }

  // Even-length subpalindromes
  {
    /* Code is almost the same. */
  }

  return results;
}
```

3. Многопоточная реализация. Ускорение за счет использования множественных потоком достигается при распределении итераций цикла равномерно по потокам. Расссмотрим приведенные выше алгоритмы:
  - Тривиальный алгоритм - итерации цикла не имеют зависимостей между собой, таким образом основной цикл функции можно успешно распараллелить.
  - Алгоритм Манакера - итерации цикла имеют зависимость одна от другой - на каждой очередной итерации могут обновиться значения левой и правой границ самого правого подпалиндрома. Данный алгоритм так просто адаптировать для многопоточного вычисления не получится.

Приведем многопоточную версию тривиального алгоритма: 

```cpp
template <typename CharT>
subpali_info find_subpalindromes_trivial(const std::basic_string<CharT>& source)
{
  auto len = source.size();
  subpali_info results(len);

#pragma omp parallel default(none) shared(results, len, source)
  #pragma omp for
  for (std::size_t idx = 0U; idx != len; ++idx)
  {
    /* Same as single-threaded trivial. */
  }

  return results;
}
```

Как можно видеть, **OpenMP** дает возможность распределить итерации цикла между потоками путем добавления всего лишь двух директив препроцессора. 

Используются директивы:
  1. ``parallel`` - определение параллельной секции. Опции ``default`` и ``shared`` определяют политику использования переменных в параллельной секции (в нашем случае каждая отдельная итерация цикла изменяет значение отдельной переменной, а переменные ``len`` и ``source`` использует только для чтения, поэтому никаких дополнительных мер синхронизации не требуется).
  2. ``for`` - указание к распределению итераций цикла между потоками, исполняющими параллельную секцию. 

Количество потоков задается переменной окружения ``OMP_NUM_THREADS``.

#### Сборка 

Перед сборкой проекта, требуется установить **OpenMP**:
- debian-based linux: ``sudo apt-get install libomp-dev``
- MacOS: ``brew install libomp`` 

Для сборки доступны три версии программы:
  - ``trivial`` - тривиальный алгоритм, однопоточная реализация
  - ``manaker`` - алгоритм Манакера, однопоточная реализация
  - ``parallel`` - тривиальный алгоритм, многопоточная реализация с помощью **OpenMP**

Чтобы собрать исполняемый файл для какого-либо значения ``target` `, воспользуйтесь следующими командами:
  1. ``cmake -B build [options]``
  2. ``cmake --build build --target <target>``, где ``<target>`` - одна из доступных версий программы.

Опции сборки (указываются в команде ``cmake -B build [options]``) :
  1. ``-DVERBOSE=ON`` - Дополнительная отладочая печать. По умолчанию опция отключена.
  2. ``-DQUIET=ON`` - Тихий запуск. Отключает какой-либо вывод программы, в том числе вывод результатов поиска подпалиндромов и дополнительную отладочную печать. По умолчанию опция включена.

После завершения сборки исполняемые файлы находятся в директории ``build``.

#### Запуск

Пример запуска программы:

```text
./build/trivial
abaaba
1 0
2 0
1 0
1 3
2 0
1 0
```

В данном примере запускается тривиальный алгоритм в его однопоточной реализации. Далее программа ожидает на вход строку, в которой будет произведен поиск всех подпалиномов. В нашем случае это ``"abaaba"``. 

Формат вывода: каждая строка соотвествует позиции в исходной строке. Первое число - количество подпалиндромов нечетной длины с центром в данной позиции, а второе число - четной длины.

#### Анализ результатов

Тесты с измерением времени были произведены на строке из 200,000 одинаковых символов. Для генерации входных данных в директории ``scripts`` располагается ``Python``-скрипт. 

Пример использования: сгенирировать текстовый файл ``in.txt`` с 200,000 символами, каждый из которых выбирается из 5 первых букв латинского алфавита. 
```text
./scripts/gen.py 200000 5 in.txt
```

1. *Trivial*
```text
❯ time ./build/trivial < in.txt
./build/trivial < in.txt  13,57s user 0,02s system 99% cpu 13,594 total
```

2. *Manaker*
```text
❯ time ./build/manaker < in.txt
./build/manaker < in.txt  0,02s user 0,00s system 96% cpu 0,018 total
```

3. *Parallel*

Ниже приведена зависимость времени исполнения программы в многопоточной реализации от количество потоков, указываемого в переменной окружения ``OMP_NUM_THREADS``.

|           | 1    | 2    | 4   | 8   | 16  | 32  | 64  | 128 | 256 | 512 | 1024 | 2048 |
|-----------|------|------|-----|-----|-----|-----|-----|-----|-----|-----|------|------|
| time      | 18,7 | 11,6 | 7,2 | 5,8 | 5,5 | 5,1 | 4,4 | 4,1 | 4,3 | 4,2 | 4,6  | 5,7  |
| system, % | 99   | 194  | 266 | 462 | 601 | 661 | 737 | 753 | 721 | 772 | 730  | 625  |

Можно видеть, что минимум время исполнения имеет при значении количества потоков в 128. При увеличении количества от 1 до 128 время исполнения увеличивается, а при дальнейшем увеличении производительность лишь падает. 
