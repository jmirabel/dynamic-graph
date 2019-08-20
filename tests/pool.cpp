// Copyright 2010 Thomas Moulard.
//

#include <dynamic-graph/entity.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <iostream>
#include <sstream>

#define BOOST_TEST_MODULE pool

#include <boost/test/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

using boost::test_tools::output_test_stream;

struct MyEntity : public dynamicgraph::Entity {
  static const std::string CLASS_NAME;

  dynamicgraph::SignalPtr<double, int> m_sigdSIN;
  dynamicgraph::SignalTimeDependent<double, int> m_sigdTimeDepSOUT;

  MyEntity(const std::string &name)
      : Entity(name),
        m_sigdSIN(NULL, "MyEntity(" + name + ")::input(double)::in_double"),
        m_sigdTimeDepSOUT(boost::bind(&MyEntity::update, this, _1, _2),
                          m_sigdSIN,
                          "MyEntity(" + name + ")::input(double)::out_double") {
    signalRegistration(m_sigdSIN << m_sigdTimeDepSOUT);
  }

  virtual void display(std::ostream &os) const {
    os << "Hello! My name is " << getName() << " !" << std::endl;
  }

  virtual const std::string &getClassName() const { return CLASS_NAME; }

  double &update(double &res, const int &inTime) {
    const double &aDouble = m_sigdSIN(inTime);
    res = aDouble;
    return res;
  }
};

DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(MyEntity, "MyEntity");

namespace dg = dynamicgraph;
BOOST_AUTO_TEST_CASE(pool_display) {
  /// Create Entity
  dg::Entity *entity =
      dg::FactoryStorage::getInstance()->newEntity("MyEntity", "MyEntityInst");

  /// Test exception catching when registering Entity
  bool res = false;
  try {
    dg::Entity *entity2 = dg::FactoryStorage::getInstance()->newEntity(
        "MyEntity", "MyEntityInst");

    bool res2 = (entity2 == entity);
    BOOST_CHECK(res2);
  } catch (const dg::ExceptionFactory &aef) {
    res = (aef.getCode() == dg::ExceptionFactory::OBJECT_CONFLICT);
  }
  BOOST_CHECK(res);

  /// Test exception catching when deregistering Entity
  res = false;
  try {
    dg::FactoryStorage::getInstance()->deregisterEntity("MyEntityInstFailure");
  } catch (const dg::ExceptionFactory &aef) {
    res = (aef.getCode() == dg::ExceptionFactory::OBJECT_CONFLICT);
  }
  BOOST_CHECK(res);

  /// Search for an entity inside the map
  output_test_stream output;
  dg::Entity &e = dg::PoolStorage::getInstance()->getEntity("MyEntityInst");
  e.display(output);
  BOOST_CHECK(output.is_equal("Hello! My name is MyEntityInst !\n"));

  /// Search for an entity inside the map
  res = false;
  try {
    dg::PoolStorage::getInstance()->getEntity("MyEntityInstFailure");
  } catch (const dg::ExceptionFactory &aef) {
    res = (aef.getCode() == dg::ExceptionFactory::UNREFERED_OBJECT);
  }
  BOOST_CHECK(res);

  /// Testing entityMap
  const dg::PoolStorage::Entities &anEntityMap =
      dg::PoolStorage::getInstance()->getEntityMap();

  bool testExistence = anEntityMap.find("MyEntityInst") == anEntityMap.end();
  BOOST_CHECK(!testExistence);

  /// Testing the existence of an entity
  testExistence =
      dg::PoolStorage::getInstance()->existEntity("MyEntityInst", entity);

  BOOST_CHECK(testExistence);

  /// Testing the completion list of pool storage
  dg::PoolStorage::getInstance()->writeCompletionList(output);
  BOOST_CHECK(
      output.is_equal("MyEntityInst.in_double\nMyEntityInst.out_double\n"
                      "print\nsignals\nsignalDep\n"));

  /// Checking the graph generated by the pool
  dg::PoolStorage::getInstance()->writeGraph("output.dot");
  std::fstream the_debug_file;
  the_debug_file.open("output.dot");
  std::ostringstream oss_output_wgph;
  oss_output_wgph << the_debug_file.rdbuf();
  the_debug_file.close();

  /// Use a predefined output
  std::string str_to_test =
      "/* This graph has been automatically generated.\n"
      "   2019 Month: 2 Day: 28 Time: 11:28 */\n"
      "digraph \"output\" { \t graph [ label=\"output\" "
      "bgcolor = white rankdir=LR ]\n"
      "\t node [ fontcolor = black, color = black,fillcolor = gold1,"
      " style=filled, shape=box ] ; \n"
      "\tsubgraph cluster_Entities { \n"
      "\t} \n"
      "\"MyEntityInst\" [ label = \"MyEntityInst\" ,\n"
      "   fontcolor = black, color = black, fillcolor=cyan, style=filled,"
      " shape=box ]\n"
      "}\n";

  /// Check the two substring (remove the date) -
  std::string s_output_wgph = oss_output_wgph.str();
  std::string s_crmk = "*/";

  std::size_t find_s_output_wgph = s_output_wgph.find(s_crmk);
  std::string sub_s_output_wgph =
      s_output_wgph.substr(find_s_output_wgph, s_output_wgph.length());
  std::size_t find_str_to_test = str_to_test.find(s_crmk);
  std::string sub_str_to_test =
      str_to_test.substr(find_str_to_test, str_to_test.length());

  bool two_sub_string_identical;
  two_sub_string_identical = sub_str_to_test == sub_s_output_wgph;
  std::cout << sub_str_to_test << std::endl;
  std::cout << sub_s_output_wgph << std::endl;
  std::cout << sub_str_to_test.compare(sub_s_output_wgph) << std::endl;
  BOOST_CHECK(two_sub_string_identical);

  /// Test name of a valid signal.
  std::istringstream an_iss("MyEntityInst.in_double");

  dg::SignalBase<int> &aSignal =
      dg::PoolStorage::getInstance()->getSignal(an_iss);

  std::string aSignalName = aSignal.getName();
  testExistence =
      aSignalName == "MyEntity(MyEntityInst)::input(double)::in_double";
  BOOST_CHECK(testExistence);

  /// Test name of an unvalid signal.
  an_iss.str("MyEntityInst.in2double");

  try {
    dg::PoolStorage::getInstance()->getSignal(an_iss);
  } catch (const dg::ExceptionFactory &aef) {
    res = (aef.getCode() == dg::ExceptionFactory::UNREFERED_SIGNAL);
  }
  BOOST_CHECK(res);

  /// Deregister the entity.
  dg::PoolStorage::getInstance()->deregisterEntity(entity->getName());

  /// Testing the existance of an entity
  testExistence =
      dg::PoolStorage::getInstance()->existEntity("MyEntityInst", entity);

  BOOST_CHECK(!testExistence);

  /// Create Entity
  std::string name_entity("MyEntityInst2");
  dg::PoolStorage::getInstance()->clearPlugin(name_entity);

  dg::PoolStorage::destroy();
}
