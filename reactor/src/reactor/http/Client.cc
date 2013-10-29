#include <cassert>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>

#include <boost/asio.hpp>

#include <elle/log.hh>
#include <elle/memory.hh>

#include <reactor/http/Client.hh>
#include <reactor/http/RequestImpl.hh>
#include <reactor/http/Service.hh>
#include <reactor/http/exceptions.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.http.Client");

namespace reactor
{
  namespace http
  {
    class Client::Impl
    {
    public:
      Impl():
        _curl(boost::asio::use_service<Service>(
                Scheduler::scheduler()->io_service())),
        _share(curl_share_init(), &curl_share_cleanup)
      {
        if (!this->_share)
          throw std::bad_alloc();
        curl_share_setopt(this->_share.get(),
                          CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
      }

      ~Impl()
      {}

    private:
      friend class Client;
      Service& _curl;
      ELLE_ATTRIBUTE(elle::generic_unique_ptr<CURLSH>, share);
    };

    Client::Client():
      _impl(new Impl)
    {}

    Client::~Client()
    {
      delete this->_impl;
    }

    elle::Buffer
    Client::get(std::string const& url,
                Request::Configuration conf)
    {
      Request r = this->request(url, reactor::http::Method::GET, conf);
      return r.response();
    }

    void
    Client::_register(Request const& request)
    {
      ELLE_TRACE_SCOPE("%s: register %s", *this, request);
      auto res = curl_easy_setopt(request._impl->_handle,
                                  CURLOPT_SHARE, this->_impl->_share.get());
      if (res != CURLE_OK)
        throw RequestError(request.url(),
                           elle::sprintf("unable to set cookie jar: %s",
                                         curl_easy_strerror(res)));
    }
  }
}
