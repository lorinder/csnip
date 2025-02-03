# What is csnip?

Csnip is an MIT licensed collection of frequently used algorithms and
datastructures for C.  Some highlights:

* Generic containers such as hash tables and sets, lists, ring buffers, lists.

* Generic sorting and searching algorithms.

* Often used things such as convenient command line options parser, or
  logging tools.

* Many more things.

Csnip comes with fairly extensive online documentation.  For further
information, see https://www.lorinder.net/csnip

# What makes csnip special?

Most C libraries have APIs that are predominantly function calls.  Csnip
on the other hand makes somewhat heavy use of macros in addition to
conventional functions.  This has some significant advantages:

* Most of csnip's datastructures and algorithms are _truly generic_.
  For example, the element type in csnip's containers can be
  parametrized at compile time, and their usage is _type safe_, similar to
  C++ templates.

  By contrast, most other C libraries achive genericity, if any, by the
  use of void pointers, which is not type safe.

* Functional aspects such as, e.g., the comparison operator used when
  sorting, can be provided as macro arguments as well.

  By contrast, the conventional approach is to use function pointers
  (like, e.g., in `qsort`), which are inconvenient to use.

* Csnip's macros are also often faster than conventional
  approaches, since the compiler has more information for static
  analysis, and e.g. slow function pointer dereferences in tight inner
  loops are avoided.
