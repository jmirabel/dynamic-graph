/*
 * Copyright 2010,
 * François Bleibel,
 * Olivier Stasse,
 *
 * CNRS/AIST
 *
 * This file is part of dynamic-graph.
 * dynamic-graph is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 * dynamic-graph is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.  You should
 * have received a copy of the GNU Lesser General Public License along
 * with dynamic-graph.  If not, see <http://www.gnu.org/licenses/>.
 */

/* --------------------------------------------------------------------- */
/* --- INCLUDE --------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* DG */
#include <dynamic-graph/tracer.h>
#include <dynamic-graph/debug.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/factory.h>
#include <boost/bind.hpp>

using namespace std;
using namespace dynamicgraph;

DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(Tracer,"Tracer");

/* --------------------------------------------------------------------- */
/* --- CLASS ----------------------------------------------------------- */
/* --------------------------------------------------------------------- */

Tracer::Tracer( const std::string n )
  :Entity(n)
   ,toTraceSignals ()
   ,traceStyle(TRACE_STYLE_DEFAULT)
   ,frequency(1)
   ,basename ()
   ,suffix(".dat")
   ,rootdir ()
   ,namesSet( false )
   ,files ()
   ,names ()
   ,play(false)
   ,timeStart(0)
   ,triger( boost::bind(&Tracer::recordTrigger,this,_1,_2),
	    sotNOSIGNAL,
	   "Tracer("+n+")::triger" )
{
  signalRegistration( triger );
}

/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */


void Tracer::
addSignalToTrace( const SignalBase<int>& sig,
		  const string& filename )
{
  dgDEBUGIN(15);
  toTraceSignals.push_back( &sig ); dgDEBUGF(15,"%p",&sig);
  names.push_back( filename );
  if( namesSet ) openFile( sig,filename );
  triger.addDependency( sig );
  dgDEBUGOUT(15);
}

/*! Empty the list of signals to trace. This function
 * does not modify the file list (it does not close
 * the files in particular.
 */
void Tracer::
clearSignalToTrace  ()
{
  toTraceSignals.clear ();
  triger.clearDependencies ();
}


// void Tracer::
// parasite( SignalBase<int>& sig )
// {
//   triger.parasite(sig);
// }

void Tracer::
openFiles( const std::string& rootdir_, const std::string& basename_,
	   const std::string& suffix_  )
{
  dgDEBUGIN(15);
  int n = rootdir_.length ();
  rootdir=rootdir_;
  if( (0<n)&('/'!=rootdir[n-1]) ) rootdir+='/';

  basename=basename_;
  suffix=suffix_;

  if( files.size () ) closeFiles ();

  SignalList::const_iterator iter = toTraceSignals.begin ();
  NameList::const_iterator iterName = names.begin ();
  while( toTraceSignals.end ()!=iter )
    {
      dgDEBUG(15) << "Open <" << (*iter)->getName ()
		   << "> in <" << *iterName << ">." << std::endl;
      openFile( **iter,*iterName );
      ++iter; ++iterName;
    }

  namesSet = true;
  dgDEBUGOUT(15);
}

void Tracer::
openFile( const SignalBase<int> & sig,
	  const string& givenname )
{
  dgDEBUGIN(15);
  string signame;
  if( givenname.length () )
    { signame = givenname;  } else { signame = sig.shortName (); }

  string filename = rootdir + basename + signame + suffix;

  dgDEBUG(5) << "Sig <"<< sig.getName () << ">: new file "<< filename << endl;
  std::ofstream * newfile = new std::ofstream( filename.c_str () );
  files.push_back( newfile );
  dgDEBUGOUT(15);
}


void Tracer::
closeFiles  ()
{
  dgDEBUGIN(15);

  for( FileList::iterator iter = files.begin ();files.end ()!=iter;++iter )
    {
      std::ostream * filePtr = *iter;
      delete filePtr;
    }
  files.clear ();

  dgDEBUGOUT(15);
}

/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */

void Tracer::
record  ()
{
  if(! play) { dgDEBUGINOUT(15); return;}

  dgDEBUGIN(15);

  if( files.size ()!=toTraceSignals.size () )
    { DG_THROW ExceptionTraces( ExceptionTraces::NOT_OPEN,
				    "No files open for tracing"," (file=%d != %d=sig).",
				    files.size (),toTraceSignals.size ()); }

  FileList::iterator iterFile = files.begin ();
  SignalList::iterator iterSig = toTraceSignals.begin ();

  while( toTraceSignals.end ()!=iterSig )
    {
      dgDEBUG(45) << "Try..." <<endl;
      recordSignal( **iterFile,**iterSig );
      ++iterSig;      ++iterFile;
    }
  dgDEBUGOUT(15);
}

void Tracer::
recordSignal(  std::ostream& os,
	       const SignalBase<int>& sig )
{
  dgDEBUGIN(15);

  try {
    if( sig.getTime ()>timeStart )
      {
	os<< sig.getTime () << "\t";
	sig.trace(os); os<<endl;
      }
  }
  catch( ExceptionAbstract& exc ) { os << exc << std::endl; }
  catch( ... ) { os << "Unknown error occured while reading signal." << std::endl; }

   dgDEBUGOUT(15);

}


int& Tracer::
recordTrigger( int& dummy, const int& time )
{
  dgDEBUGIN(15) << "    time="<<time <<endl;
  record ();
  dgDEBUGOUT(15);
  return dummy;
}


void Tracer::
trace  ()
{
}

/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */


void Tracer::
display( std::ostream& os ) const
{
  os << CLASS_NAME << " " << name << " [mode=" << (play?"play":"pause")
     << "] : "<< endl
     << "  - Dep list: "<<endl;
  for( SignalList::const_iterator iter = toTraceSignals.begin ();
       toTraceSignals.end ()!=iter;++iter )
    {      os << "     -> "<<(*iter)->getName ()<<endl;    }
}


std::ostream& operator<< ( std::ostream& os,const Tracer& t )
{
  t.display(os);
  return os;
}


void Tracer::
commandLine( const std::string& cmdLine
	     ,std::istringstream& cmdArgs
	     ,std::ostream& os )
{
  if( cmdLine=="help" )
    {
      os << "Tracer: "<<endl
	 << "  - add <obj.signal> [<filename>=<signal>] "<<endl
	 << "  - clear"<<endl
	 << "  - open  [<root> [<file> [<suffix>=.dat]]]"<<endl
	 << "  - close "<<endl
	 << "  - record "<<endl
	 << "  - trace "<<endl
	 << "  - start/stop" << endl;
	//	 << "  - parasite <obj.signal> "<<endl;

      Entity::commandLine( cmdLine,cmdArgs,os );
    }
  else if( cmdLine=="add" )
    {
      SignalBase<int> &sig = g_pool.getSignal(cmdArgs);
      string r; cmdArgs>>ws>>r;
      addSignalToTrace(sig,r);
      dgDEBUG(14)<<"Add <" <<sig.getName ()<<"> with nick \""<<r<<"\""<<endl;
    }
  else if( cmdLine=="clear" )
    { closeFiles (); toTraceSignals.clear (); }
  else if( cmdLine=="open" )
    {
      string n,r="",s=".dat";
      cmdArgs>>ws>>r;
      if( cmdArgs.good () )
	{
	  cmdArgs>>ws>>n;
	  if( cmdArgs.good () )
	    {
	      cmdArgs>>ws>>s;
	    }
	}

      //>>r>>s;
      dgDEBUGF( 15,"Close files.");
      closeFiles ();
      dgDEBUGF( 15,"Open files \"%s\" \"%s\" \"%s\".",
		r.c_str (),n.c_str (),s.c_str ());
      openFiles(r,n,s);
    }
  else if( cmdLine=="close" ) { closeFiles (); }
  else if( cmdLine=="trace" ) { trace (); }
  else if( cmdLine=="record" )
    {
      //unsigned int t;
      //cmdArgs >> ws>>t; if(! cmdArgs.good () ) t=0;
      record ();
    }
//   else if( cmdLine=="parasite" )
//     {
//        SignalBase<int> &sig = g_pool.getSignal( cmdArgs );
//        parasite(sig);
//     }
  else if( cmdLine == "start" )    {  play=true; }
  else if( cmdLine == "stop" )    {  play=false; }
  else if( cmdLine == "timeStart" )
    {
      cmdArgs >> std::ws; if(! cmdArgs.good () )
	{ os << "timeStart = " << timeStart << std::endl; }
      else { cmdArgs >> timeStart; }
    }


  else  //sotTaskAbstract::
    Entity::commandLine( cmdLine,cmdArgs,os );



}
