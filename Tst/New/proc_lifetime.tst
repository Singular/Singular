LIB "tst.lib"; tst_init();

int first_call = 1;

proc keepalive()
{
  if (first_call)
  {
    first_call = 0;
    attrib(keepalive, "marker", 7);
    kill keepalive;
    defined(keepalive);
    attrib(keepalive, "marker");
  }
  return(11);
}

keepalive();
proc keepalive_copy = keepalive;
defined(keepalive);
kill keepalive;
defined(keepalive);
keepalive_copy();
defined(keepalive_copy);
kill keepalive_copy;
defined(keepalive_copy);

proc outer()
{
  proc inner()
  {
    kill outer;
    defined(outer);
  }

  inner();
  defined(outer);
}

outer();
defined(outer);
kill outer;
defined(outer);

proc alias_cleanup()
{
  proc local_alias = alias_cleanup;
}

alias_cleanup();
defined(local_alias);
kill alias_cleanup;
defined(alias_cleanup);

proc self_attr()
{
  attrib(self_attr, "self", self_attr);
  kill self_attr;
  defined(self_attr);
}

self_attr();
defined(self_attr);
kill self_attr;
defined(self_attr);

int assign_first = 1;

proc self_assign()
{
  if (assign_first)
  {
    assign_first = 0;
    self_assign = "return(123);";
  }
  return(17);
}

self_assign();
defined(self_assign);
self_assign();
kill self_assign;
defined(self_assign);

option(noredefine);
proc export_guard()
{
  proc export_guard() { return(99); }
  export export_guard;
}

export_guard();
kill export_guard;
defined(export_guard);

proc package_member()
{
  kill LifetimePack;
  defined(LifetimePack);
}

package LifetimePack;
exportto(LifetimePack, package_member);
LifetimePack::package_member();
defined(LifetimePack);
kill LifetimePack;
defined(LifetimePack);

proc package_list_member()
{
  kill package_list_member;
  defined(package_list_member);
  kill LifetimeListPack;
  defined(LifetimeListPack);
}

list package_list_holder = package_list_member;
package LifetimeListPack;
exportto(LifetimeListPack, package_list_member);
exportto(LifetimeListPack, package_list_holder);
LifetimeListPack::package_list_member();
defined(LifetimeListPack);
kill LifetimeListPack;
defined(LifetimeListPack);

proc list_owner_release()
{
  list holder = list_owner_release;
  kill list_owner_release;
  defined(list_owner_release);
  kill holder;
  defined(holder);
  return(42);
}

list_owner_release();
defined(list_owner_release);
kill list_owner_release;
defined(list_owner_release);

proc attr_owner_release()
{
  int holder = 1;
  attrib(holder, "p", attr_owner_release);
  kill attr_owner_release;
  defined(attr_owner_release);
  kill holder;
  defined(holder);
  return(43);
}

attr_owner_release();
defined(attr_owner_release);
kill attr_owner_release;
defined(attr_owner_release);

proc shared_htable_attr()
{
  htable H = htable("p" -> shared_htable_attr);
  attrib(shared_htable_attr, "a", H);
  attrib(shared_htable_attr, "b", H);
  kill H;
  kill shared_htable_attr;
  defined(shared_htable_attr);
}

shared_htable_attr();
defined(shared_htable_attr);
kill shared_htable_attr;
defined(shared_htable_attr);

proc ring_member()
{
  kill ring_member;
  defined(ring_member);
  kill active_ring;
  defined(active_ring);
  kill RingPack;
  defined(RingPack);
}

ring active_ring = 0,(x),dp;
poly active_poly = x;
attrib(active_poly, "holder", ring_member);
package RingPack;
exportto(RingPack, ring_member);
exportto(RingPack, active_ring);
RingPack::ring_member();
defined(RingPack);
kill RingPack;
defined(RingPack);

int victim_b = 2;
int victim_a = 1;

proc nested_member()
{
  kill OuterPack;
  defined(OuterPack);
}

package InnerPack;
exportto(InnerPack, nested_member);
package OuterPack;
exportto(OuterPack, InnerPack);
exportto(OuterPack, victim_b);
exportto(OuterPack, victim_a);
OuterPack::InnerPack::nested_member();
defined(OuterPack);
defined(OuterPack::victim_a);
defined(OuterPack::victim_b);
defined(OuterPack::InnerPack);
kill OuterPack;
defined(OuterPack);

tst_status(1);$
