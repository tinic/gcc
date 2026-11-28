// { dg-do compile }
// { dg-options "-O2 -fomit-frame-pointer" }

// A call that can throw ends its basic block, so the insn that pops the
// arguments of the call is in the next block.  LRA did not follow the
// stack pointer offset across blocks and gave such a function a frame
// pointer.  Each function below has such a call: in a try block, or with
// a destructor to run.

struct M { int a, b; void pop (); };
struct D { ~D (); };
void unw (const char *, ...);

struct S
{
  M *m;
  int t;
  void cleanup ();
  void catch_all ();
  void rethrow ();
};

void
S::cleanup ()
{
  D d;
  if (m->a != t)
    m->pop ();
  m->b = t;
}

void
S::catch_all ()
{
  try
    {
      if (m->a != t)
	m->pop ();
      m->b = t;
    }
  catch (...)
    {
      unw ("x");
    }
}

void
S::rethrow ()
{
  try
    {
      if (m->a != t)
	m->pop ();
      m->b = t;
    }
  catch (char *e)
    {
      throw e;
    }
  catch (...)
    {
      unw ("%s", "S::rethrow");
      throw;
    }
}

// { dg-final { scan-assembler-not "\tlink" } }
// { dg-final { scan-assembler-not "\tunlk" } }
