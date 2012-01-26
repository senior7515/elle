//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 16:34:43 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Directory.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/gear/Operation.hh>

#include <etoile/automaton/Directory.hh>
#include <etoile/automaton/Rights.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/shrub/Shrub.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new directory object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status        Directory::Create(
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Directory*  context;
      gear::Actor*      actor;

      enterx(instance(actor),
             slab(scope, gear::Scope::Annihilate));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Create()\n");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::StatusError)
        escape("unable to supply the scope");

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // allocate an actor.
        actor = new gear::Actor(scope);

        // return the identifier.
        identifier = actor->identifier;

        // apply the create automaton on the context.
        if (automaton::Directory::Create(*context) == elle::StatusError)
          escape("unable to create the directory");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;

        // waive the actor.
        waive(actor);

        // waive the scope.
        waive(scope);
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method loads the directory referenced through the given
    /// chemin.
    ///
    elle::Status        Directory::Load(
                          const path::Chemin&                   chemin,
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Directory*  context;
      gear::Actor*      actor;

      enterx(instance(actor),
             slab(scope, gear::Scope::Annihilate));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
        escape("unable to acquire the scope");

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // allocate an actor.
        actor = new gear::Actor(scope);

        // return the identifier.
        identifier = actor->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::StatusError)
          escape("unable to locate the directory");

        // apply the load automaton on the context.
        if (automaton::Directory::Load(*context) == elle::StatusError)
          escape("unable to load the directory");

        // waive the actor.
        waive(actor);

        // waive the scope.
        waive(scope);
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method locks the given directory.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status        Directory::Lock(
                          const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Lock()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method releases a previously locked directory.
    ///
    elle::Status        Directory::Release(
                          const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Release()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method adds an entry to the given directory.
    ///
    elle::Status        Directory::Add(
                          const gear::Identifier&               parent,
                          const path::Slab&                     name,
                          const gear::Identifier&               child)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  directory;
      gear::Object*     object;
      nucleus::Address  address;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Add()\n");

      // select the actor.
      if (gear::Actor::Select(child, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(object) == elle::StatusError)
        escape("unable to retrieve the context");

      // keep the address.
      address = object->location.address;

      // select the actor.
      if (gear::Actor::Select(parent, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(directory) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the add automaton on the context.
        if (automaton::Directory::Add(*directory,
                                      name,
                                      address) == elle::StatusError)
          escape("unable to add the directory entry");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method returns the directory entry associated with the
    /// given name.
    ///
    /// note that this method should be used careful as a pointer to the
    /// target entry is returned. should this entry be destroyed by another
    /// actor's operation, accessing it could make the system crash.
    /// 
    elle::Status        Directory::Lookup(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     name,
                          nucleus::Entry*&                      entry)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Lookup()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeRead);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the lookup automaton on the context.
        if (automaton::Directory::Lookup(*context,
                                         name,
                                         entry) == elle::StatusError)
          escape("unable to lookup the directory entry");
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method returns a set [offset, offset + size[ of entries
    /// (name, address) from the directory identified by _identifier_.
    ///
    /// note that this method should be used careful as a set of pointers to
    /// the target entries is returned. should one of the entries be destroyed
    /// by another actor's operation, accessing it could make the system crash.
    /// 
    elle::Status        Directory::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::Offset&                offset,
                          const nucleus::Offset&                size,
                          nucleus::Range<nucleus::Entry>&       range)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Consult()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeRead);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the consult automaton on the context.
        if (automaton::Directory::Consult(*context,
                                          offset,
                                          size,
                                          range) == elle::StatusError)
          escape("unable to consult the directory entries");
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method renames a directory entry.
    ///
    elle::Status        Directory::Rename(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     from,
                          const path::Slab&                     to)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Rename()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the rename automaton on the context.
        if (automaton::Directory::Rename(*context,
                                         from,
                                         to) == elle::StatusError)
          escape("unable to rename the directory entry");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;

        //
        // invalidate the _from_ route from the shrub.
        //
        {
          path::Route   route;

          // build the route associated with the previous version of
          // the renamed entry.
          if (route.Create(scope->chemin.route, from) == elle::StatusError)
            escape("unable to create the route");

          // evict the route from the shrub.
          if (shrub::Shrub::Evict(route) == elle::StatusError)
            escape("unable to evict the route from the shrub");
        }

        //
        // invalidate the _to_ route from the shrub.
        //
        {
          path::Route   route;

          // build the route associated with the new version of
          // the renamed entry.
          if (route.Create(scope->chemin.route, to) == elle::StatusError)
            escape("unable to create the route");

          // evict the route from the shrub.
          if (shrub::Shrub::Evict(route) == elle::StatusError)
            escape("unable to evict the route from the shrub");
        }
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status        Directory::Remove(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     name)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;
      path::Route       route;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Remove()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the remove automaton on the context.
        if (automaton::Directory::Remove(*context,
                                         name) == elle::StatusError)
          escape("unable to remove the directory entry");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;

        //
        // invalidate the route in the shrub.
        //
        {
          // build the route associated with the removed entry.
          if (route.Create(scope->chemin.route, name) == elle::StatusError)
            escape("unable to create the route");

          // evict the route from the shrub.
          if (shrub::Shrub::Evict(route) == elle::StatusError)
            escape("unable to evict the route from the shrub");
        }
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method discards the scope along with the possible
    /// modifications having been performed.
    ///
    elle::Status        Directory::Discard(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDiscard) == elle::StatusError)
          escape("the user does not seem to have the necessary permission for "
                 "discarding this directory");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDiscard) == elle::StatusError)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        delete actor;

        // waive actor.
        waive(actor);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDiscard) == elle::StatusError)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::StatusError)
          escape("unable to trigger the shutdown");
      }
      zone.Unlock();

      // depending on the context's state.
      switch (context->state)
        {
        case gear::Context::StateDiscarded:
        case gear::Context::StateStored:
        case gear::Context::StateDestroyed:
          {
            //
            // if the directory has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::StatusError)
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::StatusError)
              escape("unable to record the scope in the journal");

            break;
          }
        default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }

      leave();
    }

    ///
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status        Directory::Store(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationStore) == elle::StatusError)
          escape("the user does not seem to have the necessary permission for "
                 "storing this directory");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationStore) == elle::StatusError)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        delete actor;

        // waive actor.
        waive(actor);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationStore) == elle::StatusError)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::StatusError)
          escape("unable to trigger the shutdown");
      }
      zone.Unlock();

      // depending on the context's state.
      switch (context->state)
        {
        case gear::Context::StateDiscarded:
        case gear::Context::StateStored:
        case gear::Context::StateDestroyed:
          {
            //
            // if the directory has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::StatusError)
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::StatusError)
              escape("unable to record the scope in the journal");

            break;
          }
        default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }

      leave();
    }

    ///
    /// this method destroys a directory.
    ///
    elle::Status        Directory::Destroy(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*              actor;
      gear::Scope*              scope;
      gear::Directory*          context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDestroy) == elle::StatusError)
          escape("the user does not seem to have the necessary permission for "
                 "destroying this directory");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDestroy) == elle::StatusError)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        delete actor;

        // waive actor.
        waive(actor);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDestroy) == elle::StatusError)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::StatusError)
          escape("unable to trigger the shutdown");
      }
      zone.Unlock();

      // depending on the context's state.
      switch (context->state)
        {
        case gear::Context::StateDiscarded:
        case gear::Context::StateStored:
        case gear::Context::StateDestroyed:
          {
            //
            // if the directory has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::StatusError)
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::StatusError)
              escape("unable to record the scope in the journal");

            break;
          }
        default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }

      leave();
    }

    ///
    /// this method purges a directory i.e removes all the blocks of all
    /// the versions associated with this directory.
    ///
    elle::Status        Directory::Purge(
                          const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Directory::Purge()\n");

      // XXX to implement.

      leave();
    }

  }
}
