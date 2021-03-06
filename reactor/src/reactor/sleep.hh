#ifndef REACTOR_SLEEP_HH
# define REACTOR_SLEEP_HH

# include <reactor/asio.hh>

# include <reactor/operation.hh>

namespace reactor
{
  class Sleep: public Operation
  {
    public:
      Sleep(Scheduler& scheduler, Duration d);

    protected:
      virtual void _abort() override;
      virtual void _start() override;

  /*----------.
  | Printable |
  `----------*/
  public:
    virtual
    void
    print(std::ostream& stream) const override;

    private:
      void _wakeup(const boost::system::error_code& error);
      Duration _duration;
      boost::asio::deadline_timer _timer;
  };
}

#endif
