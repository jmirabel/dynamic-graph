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

#include <dynamic-graph/debug.h>
#include <dynamic-graph/entity.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/interpreter-helper.h>
#include <dynamic-graph/plugin-loader.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/signal-base.h>

namespace dynamicgraph
{
  void
  InterpreterHelper::cmdPlug (const std::string& obj1,
			      const std::string& signame1,
			      const std::string& obj2,
			      const std::string& signame2,
			      std::ostream&)
  {
    dgDEBUG (20) << "Get Ent1 <" << obj1 << "> ." << std::endl;
    Entity& ent1 = g_pool.getEntity (obj1);
    dgDEBUG (20) << "Get Sig1 <" << signame1 << "> ." << std::endl;
    SignalBase<int>& sig1 = ent1.getSignal (signame1);

    dgDEBUG (20) << "Get Ent2 <" << obj2 << "> ." << std::endl;
    Entity& ent2 = g_pool.getEntity (obj2);
    dgDEBUG (20) << "Get Sig2 <" << signame2 << "> ." << std::endl;
    SignalBase<int>& sig2 = ent2.getSignal (signame2);

    dgDEBUG (25) << "Plug..." << std::endl;
    sig2.plug (&sig1);
  }

  void
  InterpreterHelper::cmdNew (const std::string& className,
			     const std::string& objName,
			     std::ostream& os)
  {
    dgDEBUG(15) << "New <" << className << "> requested." << std::endl;
    if (g_factory.existEntity (className))
      {
	dgDEBUG(15) << "New entity<" << className << "> "
		    << objName << std::endl;
	g_factory.newEntity (className, objName);
      }
    else
      os << "  !! Class <" << className << "> does not exist." << std::endl;
  }


  void
  InterpreterHelper::cmdDestroy (const std::string& objName, std::ostream&)
  {
    dgDEBUG(15) << "Destroy <" << objName << "> requested." << std::endl;
    delete &(g_pool.getEntity (objName));
  }

  void
  InterpreterHelper::cmdLoadPlugin (const std::string& directory,
				    const std::string& pluginName,
				    std::ostream&)
  {
    if (directory.length () != 0)
      dlPtr.setDirectory (directory);
    dlPtr.addPlugin (pluginName);

    try
      {
	dgDEBUG(15) << "Try to load  " << pluginName<< std::endl;
	dlPtr.loadPlugins ();
      }
    catch (const ExceptionAbstract& e)
      {
	dgDEBUG(5) << e << std::endl;
	throw;
      }
  }

  void
  InterpreterHelper::cmdUnloadPlugin (const std::string& pluginName,
				      std::ostream&)
  {
    dgDEBUGIN (15);
    try
      {
	dgDEBUG (25) << "Try short name " << pluginName << std::endl;
	const std::string& fullname = dlPtr.searchPlugin (pluginName);
	dgDEBUG(25) << "Full name " << fullname << std::endl;
	dlPtr.unloadPlugin (fullname);
      }
    catch (...)
      {
	dgDEBUG(25) << "Full name " << pluginName << std::endl;
	dlPtr.unloadPlugin (pluginName);
      }

    dgDEBUGOUT (15);
  }

  void
  InterpreterHelper::cmdSetSignal (const std::string& objname,
				   const std::string& signame,
				   const std::string& value,
				   std::ostream&)
  {
    dgDEBUGIN (15);

    Entity& obj = g_pool.getEntity (objname);
    SignalBase<int>& sig = obj.getSignal (signame);

    std::istringstream cmdArg (value);
    sig.set (cmdArg);

    dgDEBUGOUT (15);
  }

  void
  InterpreterHelper::cmdGetSignal (const std::string& objname,
				   const std::string& signame,
				   std::ostream& os)
  {
    dgDEBUGIN (15);

    Entity& obj = g_pool.getEntity (objname);
    SignalBase<int>& sig = obj.getSignal (signame);

    os << signame << " = ";
    sig.get (os);

    dgDEBUGOUT (15);
  }

  void
  InterpreterHelper::cmdComputeSignal (const std::string& objname,
				       const std::string& signame,
				       const int& time,
				       std::ostream&)
  {
    dgDEBUGIN (15);
    Entity& obj = g_pool.getEntity (objname);
    SignalBase<int>& sig = obj.getSignal (signame);

    sig.recompute (time);

    dgDEBUGOUT (15);
  }
} // end of namespace dynamicgraph.
