//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 13:29:57 2011]
//

#ifndef ETOILE_AUTOMATON_DIRECTORY_HH
#define ETOILE_AUTOMATON_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Slice.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs to be forwarded declared in order to prevent
    /// conflits.
    ///
    class Directory;

  }

  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a set of automata for operating on directory
    /// contexts.
    ///
    class Directory
    {
    public:
      //
      // static methods
      //
      static elle::Status       Create(gear::Directory&);
      static elle::Status       Load(gear::Directory&);

      static elle::Status       Add(gear::Directory&,
                                    const path::Slice&,
                                    const nucleus::Address&);
      static elle::Status       Lookup(gear::Directory&,
                                       const path::Slice&,
                                       nucleus::Entry*&);
      static elle::Status       Consult(gear::Directory&,
                                        const nucleus::Index&,
                                        const nucleus::Size&,
                                        nucleus::Range<nucleus::Entry>&);
      static elle::Status       Rename(gear::Directory&,
                                       const path::Slice&,
                                       const path::Slice&);
      static elle::Status       Remove(gear::Directory&,
                                       const path::Slice&);

      static elle::Status       Discard(gear::Directory&);
      static elle::Status       Destroy(gear::Directory&);
      static elle::Status       Store(gear::Directory&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Directory.hh>

#endif
