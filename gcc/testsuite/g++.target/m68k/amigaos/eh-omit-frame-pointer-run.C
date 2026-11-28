// { dg-do run }
// { dg-options "-O2 -fomit-frame-pointer" }

// Without a frame pointer the locals of a function are addressed from the
// stack pointer.  An exception can leave a call while arguments of that
// call, and of an outer call, are on the stack.  The unwinder removes
// them before it enters the handler, so the handler and the code after it
// must find the locals at the offsets of a stack with no arguments on it.
// The path without an exception continues with the outer arguments still
// on the stack.

extern "C" void abort ();

static int dtors;

struct D
{
  int v;
  __attribute__ ((noinline)) D (int x) : v (x) {}
  __attribute__ ((noinline)) ~D () { dtors += v; }
};

__attribute__ ((noinline, noclone)) static int
may_throw (int a, int b, int c, int doit)
{
  if (doit)
    throw a + b + c;
  return a + b + c;
}

__attribute__ ((noinline, noclone)) static int
sum5 (int a, int b, int c, int d, int e)
{
  return a + 10 * b + 100 * c + 1000 * d + 10000 * e;
}

__attribute__ ((noinline, noclone)) static void
fill (volatile int *p, int n, int base)
{
  for (int i = 0; i < n; i++)
    p[i] = base + i;
}

// The inner call throws while two arguments of sum5 are on the stack.
__attribute__ ((noinline, noclone)) static int
nested (int doit)
{
  volatile int local[4];
  fill (local, 4, 100);
  int r = 0;
  try
    {
      D d (1);
      r = sum5 (may_throw (1, 2, 3, doit), local[0], local[1], 4, 5);
      r += local[2];
    }
  catch (int e)
    {
      r = e + local[3];
    }
  return r + local[0] + local[1] + local[2] + local[3];
}

// A handler that throws again, and a destructor in the same function.
__attribute__ ((noinline, noclone)) static int
rethrow (int doit)
{
  volatile int local[3];
  fill (local, 3, 7);
  D d (10);
  try
    {
      return may_throw (local[0], local[1], local[2], doit) + local[2];
    }
  catch (int e)
    {
      if (e != 24 || local[1] != 8)
	abort ();
      throw;
    }
}

// A loop with a call that can throw in each iteration.
__attribute__ ((noinline, noclone)) static int
loop (int n, int bad)
{
  volatile int local[2];
  fill (local, 2, 3);
  int r = 0;
  for (int i = 0; i < n; i++)
    {
      try
	{
	  D d (100);
	  r += sum5 (i, may_throw (i, local[0], local[1], i == bad), 0, 0, 0);
	}
      catch (int e)
	{
	  r += 1000000 + e + local[1];
	}
    }
  return r + local[0];
}

int
main ()
{
  // sum5 (6, 100, 101, 4, 5) = 65106; + 102; + 100 + 101 + 102 + 103.
  if (nested (0) != 65106 + 102 + 406)
    abort ();
  // e = 6; + 103; + 406.
  if (nested (1) != 6 + 103 + 406)
    abort ();
  if (dtors != 2)
    abort ();

  if (rethrow (0) != 24 + 9)
    abort ();
  int caught = 0;
  try
    {
      rethrow (1);
    }
  catch (int e)
    {
      caught = e;
    }
  if (caught != 24 || dtors != 22)
    abort ();

  // i = 0: 0 + 10 * 7; i = 1: throws 8, + 1000000 + 8 + 4;
  // i = 2: 2 + 10 * 9; + 3.
  if (loop (3, 1) != 70 + 1000012 + 92 + 3)
    abort ();
  if (dtors != 322)
    abort ();
  return 0;
}
