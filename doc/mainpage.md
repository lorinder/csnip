# Introduction					{#mainpage}

## What is Csnip?

Csnip is a library of C code macros and functions (snippets, hence the
name), that are frequently useful for general purpose C programming.
It augments the standard C library with generic container data
structures, such as dynamic arrays, lists, hash tables, and so on.  It
also provides high quality implementations of frequently used
functionality such as for example random number generation.

In short, it provides functionality similar to C++'s standard template
library for C programs.

## Csnip design goals

We strive to provide components that are:

- **Quality**.  Csnip aims to provide higher quality algorithms and
  datastructures than what you would typically get if you implemented
  the same in an ad hoc fashion.

- **Transparency**.  The implementation of csnip's datastructures is
  documented in addition to the interfaces.  You may opt to modify them
  directly; and in some cases this can come in handy, say, if you want
  to do something that's not possible or easy with the official
  interfaces.  For example, you can use direct access to sample a random
  entry from a hash set, or can read data from a file into a dynamic
  array that way.

  In contrast, most other libraries are designed to be opaque, providing
  _data abstraction_ as a means to compartmentalize funcionality.  In
  csnip, the same can be achieved with CSNIP\_\*\_{DECL,DEF}\_FUNCS()
  (see XXX), but it is entirely optional.  It can be used when it makes
  sense, but avoided when it gets into the way.

- **General**.  Csnip wants to be useful for as wide a class of users
  and environments as possible.  A liberal license is used so it can be
  used in as many contexts as possible.  We try to stay portable where
  this is possible, but to provide the extras for environments that have
  them.  Thus, e.g., type generic macros via \_Generic are used for C11
  users along with non-generic versions for those which don't have that
  functionality available.

  Csnip is generic, thus e.g. the sorting functions do not depend on the
  data type being sorted, or what type of random access container is
  used to store the elements.

  We aim to design the library in a way that they can also be useful in
  uncommon use cases, provided that doesn't result in unduly burdening
  typical use cases.  

- **Pragmatic**.  Csnip strives to be pragmatic.  For example, we like
  consistent interfaces, but if they get in the way of e.g. usability,
  we'll do something else.  Csnip tries to be general in the definition
  of its datastructures and function, but not to the point of making
  them unnecessarily painful.

## Library conventions

### Notation

Csnip uses the prefixes csnip\_ or CSNIP\_ for its symbols and macros.
Users should not define their own symbols or macros with such names.

Many functions or macros are grouped by functionality, e.g. things
related to memory management are called csnip\_mem\_\*.  Though some very
frequently used functionality that does not nicely fit into a group is
directly under csnip\_\*, without further grouping.

Macros that expand to statements or expression (_statement macros_ or
_expression macros_) have upper CamelCase names; thus for example
csnip\_arr\_Init() is a macro.  Unless the behavior of the macro is
documented for the case when the argument has side effects, the user is
expected to use arguments without side effects, since the arguments may
be evaluated more than once.

Macros and enumeration values that expand to constants are all-caps
after the prefix, e.g. csnip\_err\_SUCCESS is such a constant.

Macros that evaluate to more than an expression or a statement are
written in all-caps.  Examples include macros that generate functions for
container manipulation, such as CSNIP\_LPHASH\_TABLE\_DEF\_FUNCS().

Normal C functions and variables are written in snake case, for example
csnip\_time\_timespec\_as\_float() is a function.

