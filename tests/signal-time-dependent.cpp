// Copyright 2010 Thomas Moulard.
//
// This file is part of dynamic-graph.
// dynamic-graph is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dynamic-graph is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public License
// along with dynamic-graph.  If not, see <http://www.gnu.org/licenses/>.

#include <boost/foreach.hpp>

#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-time-dependent.h>

#define BOOST_TEST_MODULE signal_time_dependent

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

using boost::test_tools::output_test_stream;

typedef dynamicgraph::SignalTimeDependent<double, int> sigDouble_t;
typedef dynamicgraph::SignalTimeDependent<std::string, int> sigString_t;

template<class T>
class DummyClass
{
public:

  std::string proname;
  std::list<sigDouble_t*> inputsig;
  std::list<sigString_t*> inputsigV;

  DummyClass (const std::string& n)
    : proname (n),
      res (),
      call (),
      timedata ()
  {}

  T& fun (T& res, int t)
  {
    ++call;
    timedata=t;

    BOOST_FOREACH (sigDouble_t* ptr, inputsig)
      ptr->access(timedata);

    BOOST_FOREACH (sigString_t* ptr,
		   inputsigV)
      ptr->access(timedata);

    res = (*this) ();
    return res;
  }

  void add (sigDouble_t& sig)
  {
    inputsig.push_back (&sig);
  }
  void add (sigString_t& sig)
  {
    inputsigV.push_back (&sig);
  }

  T operator() ();

  T res;
  int call;
  int timedata;
};

template<>
double
DummyClass<double>::operator() ()
{
  res=call * timedata;
  return res;
}
template<>
std::string
DummyClass<std::string>::operator() ()
{
  std::ostringstream oss;
  oss << call * timedata;
  return oss.str ();
}

template< class T >
T DummyClass<T>::operator() ()
{
  return this->res;
}

BOOST_AUTO_TEST_CASE (signaltimedependent)
{
  DummyClass<double> pro1 ("pro1"), pro3 ("pro3"), pro5 ("pro5");
  DummyClass<std::string> pro2 ("pro2"), pro4 ("pro4"), pro6 ("pro6");

  sigDouble_t sig5 ("Sig5");
  sigString_t sig6 ("Sig6");

  std::string name;
  sig5.getClassName(name) ;
  BOOST_CHECK_EQUAL(name, "double");
  sig6.getClassName(name) ;
  BOOST_CHECK_EQUAL(name, "string");

  sigString_t sig4(sig5, "Sig4");
  sigString_t sig2(sig4 << sig4 << sig4 << sig6, "Sig2");
  sigDouble_t sig3(sig2 << sig5 << sig6, "Sig3");
  sigDouble_t sig1
    (boost::bind (&DummyClass<double>::fun, &pro1, _1, _2),
     sig2 << sig3, "Sig1");

  sig2.setFunction (boost::bind (&DummyClass<std::string>::fun,&pro2, _1, _2));
  sig3.setFunction (boost::bind (&DummyClass<double>::fun,&pro3, _1, _2));
  sig4.setFunction (boost::bind (&DummyClass<std::string>::fun,&pro4, _1, _2));
  sig5.setFunction (boost::bind (&DummyClass<double>::fun,&pro5, _1, _2));
  sig6.setFunction (boost::bind (&DummyClass<std::string>::fun,&pro6, _1, _2));

  pro1.add(sig2);
  pro1.add(sig3);
  pro2.add(sig4);
  pro2.add(sig4);
  pro2.add(sig4);
  pro3.add(sig2);
  pro4.add(sig5);
  pro2.add(sig6);
  pro3.add(sig5);
  pro3.add(sig6);

  sig5.setDependencyType (dynamicgraph::TimeDependency<int>::ALWAYS_READY);
  sig6.setDependencyType (dynamicgraph::TimeDependency<int>::BOOL_DEPENDENT);

  sig6.setReady();

  {
    output_test_stream output;
    sig1.displayDependencies (output);
    BOOST_CHECK (output.is_equal
		 ("-- Sig:Sig1 (Type Fun) (t=0 (/1) )\n"
		  "   |-- Sig:Sig3 (Type Fun) (t=0 (/1) )\n"
		  "   |   |-- Sig:Sig6 (Type Fun) (ready=TRUE)\n"
		  "   |   |-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |   `-- Sig:Sig2 (Type Fun) (t=0 (/1) )\n"
		  "   |       |-- Sig:Sig6 (Type Fun) (ready=TRUE)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=0 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=0 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       `-- Sig:Sig4 (Type Fun) (t=0 (/1) )\n"
		  "   |           `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   `-- Sig:Sig2 (Type Fun) (t=0 (/1) )\n"
		  "       |-- Sig:Sig6 (Type Fun) (ready=TRUE)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=0 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=0 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       `-- Sig:Sig4 (Type Fun) (t=0 (/1) )\n"
		  "           `-- Sig:Sig5 (Type Fun) (A)"
		  ));
  }

  BOOST_CHECK (sig1.needUpdate (2));
  sig1.access (2);

  {
    output_test_stream output;
    sig1.displayDependencies (output);
    BOOST_CHECK (output.is_equal
		 (
		  "-- Sig:Sig1 (Type Fun) (t=2 (/1) )\n"
		  "   |-- Sig:Sig3 (Type Fun) (t=2 (/1) )\n"
		  "   |   |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "   |   |-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |   `-- Sig:Sig2 (Type Fun) (t=2 (/1) )\n"
		  "   |       |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=2 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=2 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       `-- Sig:Sig4 (Type Fun) (t=2 (/1) )\n"
		  "   |           `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   `-- Sig:Sig2 (Type Fun) (t=2 (/1) )\n"
		  "       |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=2 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=2 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       `-- Sig:Sig4 (Type Fun) (t=2 (/1) )\n"
		  "           `-- Sig:Sig5 (Type Fun) (A)"
		  ));
  }
  sig2.access (4);

  {
    output_test_stream output;
    sig1.displayDependencies (output);
    BOOST_CHECK (output.is_equal
		 (
		  "-- Sig:Sig1 (Type Fun) (t=2 (/1) )\n"
		  "   |-- Sig:Sig3 (Type Fun) (t=2 (/1) )\n"
		  "   |   |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "   |   |-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |   `-- Sig:Sig2 (Type Fun) (t=4 (/1) )\n"
		  "   |       |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       `-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "   |           `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   `-- Sig:Sig2 (Type Fun) (t=4 (/1) )\n"
		  "       |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       `-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "           `-- Sig:Sig5 (Type Fun) (A)"
		  ));
  }
  sig1.access (4);

  {
    output_test_stream output;
    sig1.displayDependencies (output);
    BOOST_CHECK (output.is_equal
		 (
		  "-- Sig:Sig1 (Type Fun) (t=4 (/1) )\n"
		  "   |-- Sig:Sig3 (Type Fun) (t=4 (/1) )\n"
		  "   |   |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "   |   |-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |   `-- Sig:Sig2 (Type Fun) (t=4 (/1) )\n"
		  "   |       |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "   |       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   |       `-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "   |           `-- Sig:Sig5 (Type Fun) (A)\n"
		  "   `-- Sig:Sig2 (Type Fun) (t=4 (/1) )\n"
		  "       |-- Sig:Sig6 (Type Fun) (ready=FALSE)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       |-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "       |   `-- Sig:Sig5 (Type Fun) (A)\n"
		  "       `-- Sig:Sig4 (Type Fun) (t=4 (/1) )\n"
		  "           `-- Sig:Sig5 (Type Fun) (A)"
		  ));
  }

  sig1.needUpdate (6);
  sig1.needUpdate (6);
}
