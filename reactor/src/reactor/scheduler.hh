#ifndef INFINIT_REACTOR_SCHEDULER_HH
# define INFINIT_REACTOR_SCHEDULER_HH

# include <memory>
# include <set>
# include <thread>
# include <unordered_map>

# include <boost/thread.hpp>

# include <elle/Printable.hh>

# include <reactor/asio.hh>
# include <reactor/duration.hh>
# include <reactor/fwd.hh>
# include <reactor/backend/thread.hh>

namespace reactor
{
  /** Scheduler
   *
   */
  class Scheduler:
    public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Scheduler();

  /*------------------.
  | Current scheduler |
  `------------------*/
  public:
    static
    Scheduler*
    scheduler();

  /*----.
  | Run |
  `----*/
  public:
    void run();
    bool step();
  private:
    void _step(Thread* t);

  /*-------------------.
  | Threads management |
  `-------------------*/
  public:
    typedef std::set<Thread*> Threads;
    Thread* current() const;
    void terminate();
    void terminate_later();
  private:
    bool _shallstop = false;

  private:
    void _freeze(Thread& thread);
    void _thread_register(Thread& thread);
    void _unfreeze(Thread& thread);
  private:
    void _terminate(Thread* thread);
    void _terminate_now(Thread* thread);
    Thread* _current;
    Threads _starting;
    boost::mutex _starting_mtx;
    Threads _running;
    Threads _frozen;

    /*-------.
    | Status |
    `-------*/
    public:
      void dump(std::ostream&);
      void debug();

  /*-------------------------.
  | Thread Exception Handler |
  `-------------------------*/
  private:
    std::exception_ptr _eptr;

  /*----------------.
  | Multithread API |
  `----------------*/
  public:
    template <typename R>
    R
    mt_run(const std::string& name,
           const boost::function<R ()>& action);
  private:
    void
    _mt_run_void(const std::string& name,
                 const boost::function<void ()>& action);

  /*----------.
  | Printable |
  `----------*/
  public:
    void print(std::ostream& s) const;

  /*----------.
  | Shortcuts |
  `----------*/
  public:
    void CallLater(const boost::function<void ()>&      f,
                   const std::string&                   name,
                   Duration                             delay);
    void
    every(const boost::function<void ()>& f,
          const std::string& name,
          Duration delay);

    /*-----.
    | Asio |
    `-----*/
    public:
      boost::asio::io_service& io_service();
    private:
      boost::asio::io_service _io_service;
      boost::asio::io_service::work* _io_service_work;

    /*--------.
    | Details |
    `--------*/
    private:
      friend class Thread;
      backend::Manager _manager;
  };
}

# include <reactor/scheduler.hxx>

#endif
