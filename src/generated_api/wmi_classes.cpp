
#include "pch.h"
#include <core.h>
#include "wmi_classes.h"

namespace wmi{


void WmiMonitorID::deserialize(pugi::xml_node_struct* src, WmiMonitorID& destination)
{
  const pugi::xml_node doc{src};
  (void)destination;
  Deserialize<bool>::to(destination.Active, 
    doc.select_node("PROPERTY[@NAME=\"Active\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.WeekOfManufacture, 
    doc.select_node("PROPERTY[@NAME=\"WeekOfManufacture\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.InstanceName, 
    doc.select_node("PROPERTY[@NAME=\"InstanceName\"]/VALUE").node().text().as_string());
  Deserialize<uint16_t>::to(destination.YearOfManufacture, 
    doc.select_node("PROPERTY[@NAME=\"YearOfManufacture\"]/VALUE").node().text().as_string());

  pugi::xpath_node_set nodes;

  nodes = doc.select_node("PROPERTY.ARRAY[@NAME=\"ManufacturerName\"]/VALUE.ARRAY").node().select_nodes("VALUE");
  for(int i = 0; i < 16; ++i)
    Deserialize<uint16_t>::to(destination.ManufacturerName[i], nodes[i].node().text().as_string());
  nodes = doc.select_node("PROPERTY.ARRAY[@NAME=\"ProductCodeID\"]/VALUE.ARRAY").node().select_nodes("VALUE");
  for(int i = 0; i < 16; ++i)
    Deserialize<uint16_t>::to(destination.ProductCodeID[i], nodes[i].node().text().as_string());
  nodes = doc.select_node("PROPERTY.ARRAY[@NAME=\"SerialNumberID\"]/VALUE.ARRAY").node().select_nodes("VALUE");
  for(int i = 0; i < 16; ++i)
    Deserialize<uint16_t>::to(destination.SerialNumberID[i], nodes[i].node().text().as_string());
  nodes = doc.select_node("PROPERTY.ARRAY[@NAME=\"UserFriendlyName\"]/VALUE.ARRAY").node().select_nodes("VALUE");
  destination.UserFriendlyName.resize(nodes.size());
  for(int i = 0; i < nodes.size(); ++i)
    Deserialize<uint16_t>::to(destination.UserFriendlyName[i], nodes[i].node().text().as_string());
}


std::vector<WmiMonitorID> WmiMonitorID::get_all_objects()
{
  std::vector<WmiMonitorID> result;
  WMIProvider::get().query("select * from WmiMonitorID", [&](const pugi::xml_document& doc) {
    WmiMonitorID cpp_obj;
    WmiMonitorID::deserialize(doc.child("INSTANCE").internal_object(), cpp_obj);
    result.emplace_back(std::move(cpp_obj));
  });
  return result;
}


std::string WmiMonitorID::to_string() const
{
  std::string result;
  ((result += "Active: ") += std::to_string(Active)) += '\n';
  ((result += "WeekOfManufacture: ") += std::to_string(WeekOfManufacture)) += '\n';
  ((result += "InstanceName: ") += InstanceName) += '\n';
  ((result += "YearOfManufacture: ") += std::to_string(YearOfManufacture)) += '\n';


  result += "ManufacturerName: ";
  for(const auto & e : ManufacturerName)
      (result += std::to_string(e)) += ' ';
  result += '\n';

  result += "ProductCodeID: ";
  for(const auto & e : ProductCodeID)
      (result += std::to_string(e)) += ' ';
  result += '\n';

  result += "SerialNumberID: ";
  for(const auto & e : SerialNumberID)
      (result += std::to_string(e)) += ' ';
  result += '\n';

  result += "UserFriendlyName: ";
  for(const auto & e : UserFriendlyName)
      (result += std::to_string(e)) += ' ';
  result += '\n';


  return result;
}


} //namespace wmi
