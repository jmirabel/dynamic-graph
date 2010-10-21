//
// Copyright 2010 CNRS
//
// Author: Florent Lamiraux
//
// This file is part of dynamic-graph.
// dynamic-graph is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
// dynamic-graph is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.  You should
// have received a copy of the GNU Lesser General Public License along
// with dynamic-graph.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DYNAMIC_GRAPH_VALUE_H
#define DYNAMIC_GRAPH_VALUE_H

#include <iostream>
#include <string>
#include <cassert>
#include <typeinfo>
#include "dynamic-graph/dynamic-graph-api.h"

namespace dynamicgraph {
  namespace command {
    class DYNAMICGRAPH_EXPORT Value {
    public:
      enum Type {
	NONE,
	INT,
	DOUBLE,
	STRING,
	NB_TYPES
      };
      ~Value();
      /// template constructor
      template <class T> Value(const T& value);
      /// Copy constructor
      Value(const Value& value);
      // Construct an empty value (None)
      Value();
      /// Return the type of the value
      Type type() const;

      /// Return the value if it is a double and throw otherwise
      double doubleValue () const;
      /// Return the value if it is a int and throw otherwise
      int intValue () const;
      /// Return the value if it is a string and throw otherwise
      std::string stringValue () const;
      /// Return the name of the type
      static std::string typeName(Type type);

      /// Output in a stream
      friend std::ostream& operator<<(std::ostream& os, const Value& value);
    private:
      Type type_;
      const void* value_;
    };

    // Template constructors
    template <> inline Value::Value(const int& value)
    {
      std::cout << "Constructor of int value" << std::endl;
      value_ = new int(value);
      type_ = INT;
    }
    template <> inline Value::Value(const double& value)
    {
      std::cout << "Constructor of double value" << std::endl;
      value_ = new double(value);
      type_ = DOUBLE;
    }
    template <> inline Value::Value(const std::string& value)
    {
      std::cout << "Constructor of string value" << std::endl;
      value_ = new std::string(value);
      type_ = STRING;
    }
    template <class T> Value::Value(const T& value)
    {
      assert(false);
    }
  } // namespace command
} //namespace dynamicgraph

#endif //DYNAMIC_GRAPH_VALUE_H