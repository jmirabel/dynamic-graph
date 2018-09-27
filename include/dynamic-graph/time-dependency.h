// -*- mode: c++ -*-
// Copyright 2010, François Bleibel, Thomas Moulard, Olivier Stasse,
// JRL, CNRS/AIST.
//
// This file is part of dynamic-graph.
// dynamic-graph is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// dynamic-graph is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

#ifndef DYNAMIC_GRAPH_TIME_DEPENDENCY_H
# define DYNAMIC_GRAPH_TIME_DEPENDENCY_H
# include <list>

# include <dynamic-graph/fwd.hh>
# include <dynamic-graph/signal-base.h>
# include <dynamic-graph/signal-array.h>

namespace dynamicgraph
{
  /** \brief A helper class for setting and specifying dependencies
      between signals.
  */
  template<class Time>
  class TimeDependency
  {
  public:
    enum DependencyType
      {
	TIME_DEPENDENT,
	BOOL_DEPENDENT,
	ALWAYS_READY
      };

    mutable Time lastAskForUpdate;

  public:

    SignalBase< Time >& leader;

    typedef std::list< const SignalBase<Time> * > Dependencies;
    static const DependencyType DEPENDENCY_TYPE_DEFAULT = TIME_DEPENDENT;

    Dependencies dependencies;
    bool updateFromAllChildren;
    static const bool ALL_READY_DEFAULT = false;

    DependencyType dependencyType;

    Time periodTime;
    static const Time PERIOD_TIME_DEFAULT = 1;

  public:

    TimeDependency( SignalBase<Time>* sig,
		    const DependencyType dep = DEPENDENCY_TYPE_DEFAULT );
    TimeDependency( SignalBase<Time>* sig,
		    const SignalArray_const<Time>& arr,
		    const DependencyType dep = DEPENDENCY_TYPE_DEFAULT );
    virtual ~TimeDependency  () {}

    void addDependency( const SignalBase<Time>& sig );
    void removeDependency( const SignalBase<Time>& sig );
    void clearDependency  ();


    virtual std::ostream & writeGraph(std::ostream &os) const;
    std::ostream& displayDependencies( std::ostream& os,const int depth=-1,
				       std::string space="",
				       std::string next1="",std::string next2="" ) const;

    bool needUpdate( const Time& t1 ) const;

    void setDependencyType( DependencyType dep ) { dependencyType = dep; }

    void setNeedUpdateFromAllChildren( const bool b = true ){ updateFromAllChildren=b; }
    bool getNeedUpdateFromAllChildren  () const { return updateFromAllChildren; }

    void setPeriodTime( const Time& p ) { periodTime = p; }
    Time getPeriodTime  () const { return periodTime; }

  };

} // end of namespace dynamicgraph

# include <dynamic-graph/time-dependency.t.cpp>
#endif //! DYNAMIC_GRAPH_TIME_DEPENDENCY_H
