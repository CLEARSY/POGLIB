/** pog.cpp

   \copyright Copyright © CLEARSY 2022
   \license This file is part of POGLIB.

   POGLIB is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    POGLIB is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with POGLIB. If not, see <https://www.gnu.org/licenses/>.
*/
#include "pog.h"

#include <iostream>

#include "exprDesc.h"
#include "exprReader.h"
#include "exprWriter.h"
#include "gpredReader.h"
#include "predDesc.h"
#include "predReader.h"
#include "predWriter.h"
#include "substReader.h"
#include "tinyxml2.h"

bool isConj(const Pred& p) { return (p.getTag() == Pred::PKind::Conjunction); }

BType readType(const tinyxml2::XMLElement* dom) {
  if (dom == nullptr) throw pog::PogException("Null dom element.");

  const char* tag = dom->Name();
  if (strcmp(tag, "Id") == 0) {
    const char* value = dom->Attribute("value");
    if (strcmp(value, "INTEGER") == 0) {
      return BType::INT;
    } else if (strcmp(value, "FLOAT") == 0) {
      return BType::FLOAT;
    } else if (strcmp(value, "REAL") == 0) {
      return BType::REAL;
    } else if (strcmp(value, "STRING") == 0) {
      return BType::STRING;
    } else if (strcmp(value, "BOOL") == 0) {
      return BType::BOOL;
    } else {
      if (dom->Attribute("suffix") != nullptr)
        throw pog::PogException(
            "Abstract or Concrete Set with suffix.");  // this constraint could
                                                       // be removed
      return BType::INT;
    }
  } else if (strcmp(tag, "Unary_Exp") == 0) {
    const char* op = dom->Attribute("op");
    if (op == nullptr ||
        strcmp(op, "POW") != 0) {  // Added null and value check
      throw pog::PogException(
          "Expected 'op' attribute with value 'POW' in 'Unary_Exp' tag.");
    }
    const tinyxml2::XMLElement* firstChild = dom->FirstChildElement();
    if (firstChild == nullptr) {  // Added null check
      throw pog::PogException("Expected child element in 'Unary_Exp' tag.");
    }
    return BType::POW(readType(firstChild));
  } else if (strcmp(tag, "Binary_Exp") == 0) {
    const char* op = dom->Attribute("op");
    if (op == nullptr || strcmp(op, "*") != 0) {  // Added null and value check
      throw pog::PogException(
          "Expected 'op' attribute with value '*' in 'Binary_Exp' tag.");
    }
    const tinyxml2::XMLElement* fst = dom->FirstChildElement();
    if (fst == nullptr) {  // Added null check
      throw pog::PogException(
          "Expected first child element in 'Binary_Exp' tag.");
    }
    const tinyxml2::XMLElement* snd = fst->NextSiblingElement();
    if (snd == nullptr) {  // Added null check
      throw pog::PogException(
          "Expected second child element in 'Binary_Exp' tag.");
    }
    return BType::PROD(readType(fst), readType(snd));
  } else if (strcmp(tag, "Struct") == 0) {
    std::vector<std::pair<std::string, BType>> fields;
    for (tinyxml2::XMLElement const* item =
             dom->FirstChildElement("Record_Item");
         item != nullptr; item = item->NextSiblingElement("Record_Item")) {
      const char* label = item->Attribute("label");
      if (label == nullptr) {  // Added null check
        throw pog::PogException(
            "Missing 'label' attribute in 'Record_Item' tag.");
      }
      const tinyxml2::XMLElement* fieldTypeElement = item->FirstChildElement();
      if (fieldTypeElement == nullptr) {  // Added null check
        throw pog::PogException("Missing child element in 'Record_Item' tag.");
      }
      fields.push_back({label, readType(fieldTypeElement)});
    }
    return BType::STRUCT(fields);
  } else {
    throw pog::PogException("Unexpected Tag: " +
                            std::string(tag));  // More informative exception
  }
  assert(false);      // unreachable
  return BType::INT;  // Added default return to avoid warning, though ideally
                      // unreachable
}

void readTypeInfos(
    const tinyxml2::XMLElement* dom,
    std::vector<BType>& typeInfosOut) {  // Modified to take output vector and
                                         // const input vector
  assert(typeInfosOut.empty());  // Renamed to typeInfosOut and check if empty
  if (dom != nullptr) {
    int cpt = 0;
    for (tinyxml2::XMLElement const* typ = dom->FirstChildElement("Type");
         typ != nullptr; typ = typ->NextSiblingElement("Type")) {
      bool ok = false;
      int typref = 0;
      const char* idAttr = typ->Attribute("id");
      if (idAttr != nullptr) {
        typref = std::stoi(idAttr);  // Use std::stoi for safer conversion
        ok = true;
      }
      if (!ok)
        throw pog::PogException(
            "Integer expected for 'id' attribute in 'Type' tag.");
      if (typref != cpt)
        throw pog::PogException("Unexpected typref. Expecting '" +
                                std::to_string(cpt) + "'. Found '" +
                                std::to_string(typref) + "'.");
      const tinyxml2::XMLElement* typeElement = typ->FirstChildElement();
      if (typeElement == nullptr) {  // Added null check
        throw pog::PogException("Expected child element in 'Type' tag.");
      }
      typeInfosOut.push_back(readType(typeElement));  // Use typeInfosOut
      cpt++;
    }
  }
}

pog::Set readSet(const std::vector<BType>& typeInfos,
                 const tinyxml2::XMLElement* dom) {
  std::vector<TypedVar> vec;
  const tinyxml2::XMLElement* enumeratedValues =
      dom->FirstChildElement("Enumerated_Values");
  if (enumeratedValues != nullptr) {  // Check if Enumerated_Values exists
    for (tinyxml2::XMLElement const* elt =
             enumeratedValues->FirstChildElement("Id");
         elt != nullptr; elt = elt->NextSiblingElement("Id")) {
      vec.push_back(Xml::VarNameFromId(elt, typeInfos));
    }
  }
  auto id = dom->FirstChildElement("Id");
  if (id == nullptr)
    throw pog::PogException("Missing 'Id' element in Set definition.");
  return pog::Set(Xml::VarNameFromId(id, typeInfos), vec);
}

pog::pog pog::read(tinyxml2::XMLDocument& pogDoc) {
  pog res;
  auto root = pogDoc.RootElement();
  if (root == nullptr)
    throw PogException("Proof_Obligations root element expected.");
  // TypeInfos
  auto typeInfosElement = root->FirstChildElement("TypeInfos");
  readTypeInfos(typeInfosElement, res.typeInfos);

  // Defines
  for (tinyxml2::XMLElement const* e = root->FirstChildElement("Define");
       e != nullptr; e = e->NextSiblingElement("Define")) {
    const char* nameAttr = e->Attribute("name");
    if (!nameAttr)
      throw PogException("Attribute 'name' expected in 'Define' tag.");

    size_t hash = 0;
    const char* hashAttr = e->Attribute("hash");
    if (hashAttr) {
      hash = std::stoul(hashAttr, nullptr, 0);
    }
    auto def = Define(nameAttr, hash);
    for (tinyxml2::XMLElement const* ch = e->FirstChildElement(); ch != nullptr;
         ch = ch->NextSiblingElement()) {
      if (strcmp(ch->Name(), "Set") == 0) {
        Set s = readSet(res.typeInfos, ch);
        def.contents.push_back(std::move(s));
      } else {
        Pred p = Xml::readPredicate(ch, res.typeInfos);
        assert(!isConj(p));
        def.contents.push_back(std::move(p));
      }
    }
    res.defines.push_back(std::move(def));
  }
  // Proof_Obligation
  for (tinyxml2::XMLElement const* po =
           root->FirstChildElement("Proof_Obligation");
       po != nullptr; po = po->NextSiblingElement("Proof_Obligation")) {
    // goalHash
    size_t goalHash = 0;
    const char* goalHashAttr = po->Attribute("goalHash");
    if (goalHashAttr) {
      goalHash = std::stoul(goalHashAttr, nullptr, 0);
    }
    // Tag
    std::string tagStr{};
    const tinyxml2::XMLElement* tagElement = po->FirstChildElement("Tag");
    if (tagElement != nullptr) {
      const char* tagText = tagElement->GetText();
      if (tagText) tagStr = tagText;  // GetText() can return nullptr
    }
    // Definitions
    std::vector<std::string> definitions;
    for (tinyxml2::XMLElement const* e = po->FirstChildElement("Definition");
         e != nullptr; e = e->NextSiblingElement("Definition")) {
      const char* defNameAttr = e->Attribute("name");
      if (!defNameAttr)
        throw PogException("Attribute 'name' expected in 'Definition' tag.");
      definitions.push_back(defNameAttr);
    }
    // Hypothesis
    std::vector<Pred> hyps;
    for (tinyxml2::XMLElement const* e = po->FirstChildElement("Hypothesis");
         e != nullptr; e = e->NextSiblingElement("Hypothesis")) {
      const tinyxml2::XMLElement* predElement = e->FirstChildElement();
      if (predElement == nullptr)
        throw PogException(
            "Missing predicate element within 'Hypothesis' tag.");
      Pred p = Xml::readPredicate(predElement, res.typeInfos);
      assert(!isConj(p));
      hyps.push_back(std::move(p));
    }
    // Local Hypotheses
    std::vector<Pred> localHyps;
    for (tinyxml2::XMLElement const* e = po->FirstChildElement("Local_Hyp");
         e != nullptr; e = e->NextSiblingElement("Local_Hyp")) {
      const tinyxml2::XMLElement* predElement = e->FirstChildElement();
      if (predElement == nullptr)
        throw PogException("Missing predicate element within 'Local_Hyp' tag.");
      Pred p = Xml::readPredicate(predElement, res.typeInfos);
      assert(!isConj(p));
      localHyps.push_back(std::move(p));
    }
    // Simple Goal
    std::vector<PO> simpleGoals;
    for (tinyxml2::XMLElement const* e = po->FirstChildElement("Simple_Goal");
         e != nullptr; e = e->NextSiblingElement("Simple_Goal")) {
      // Tag
      const tinyxml2::XMLElement* tagElementSG = e->FirstChildElement("Tag");
      std::string _tag;
      if (tagElementSG != nullptr) {
        const char* tagText = tagElementSG->GetText();
        if (tagText) _tag = tagText;
      }
      // Ref Hyps
      std::vector<int> _localHypRefs;
      for (tinyxml2::XMLElement const* ch = e->FirstChildElement("Ref_Hyp");
           ch != nullptr; ch = ch->NextSiblingElement("Ref_Hyp")) {
        const char* numAttr = ch->Attribute("num");
        if (numAttr) {
          try {
            _localHypRefs.push_back(std::stoi(numAttr));
          } catch (const std::invalid_argument& /*e*/) {
            throw PogException(
                "Invalid integer value for 'num' attribute in 'Ref_Hyp' tag.");
          } catch (const std::out_of_range& /*e*/) {
            throw PogException(
                "Out of range integer value for 'num' attribute in 'Ref_Hyp' "
                "tag.");
          }
        } else {
          throw PogException("Missing 'num' attribute in 'Ref_Hyp' tag.");
        }
      }
      // Goal
      const tinyxml2::XMLElement* goalElement = e->FirstChildElement("Goal");
      if (goalElement == nullptr) {
        throw PogException("Missing 'Goal' element in 'Simple_Goal' tag.");
      }
      const tinyxml2::XMLElement* predElementGoal =
          goalElement->FirstChildElement();
      if (predElementGoal == nullptr) {
        throw PogException(
            "Missing predicate element within 'Goal' element in 'Simple_Goal' "
            "tag.");
      }

      Pred _goal = Xml::readPredicate(predElementGoal, res.typeInfos);
      simpleGoals.push_back({_tag, _localHypRefs, std::move(_goal)});
    }
    res.pos.push_back(POGroup(tagStr, goalHash, definitions, std::move(hyps),
                              std::move(localHyps), std::move(simpleGoals)));
  }
  return res;
}

pog::pog pog::read(const std::filesystem::path& pogFile) {
  tinyxml2::XMLDocument doc;
  if (doc.LoadFile(pogFile.string().c_str()) != tinyxml2::XML_SUCCESS)
    throw PogException("Failed to load file: " + pogFile.string());
  return read(doc);
}

void pog::pog::accept(pogVisitor& v) const { v.visitPog(*this); }
void pog::Define::accept(pogVisitor& v) const { v.visitDefine(*this); }
void pog::POGroup::accept(pogVisitor& v) const { v.visitPOGroup(*this); }
void pog::PO::accept(pogVisitor& v) const { v.visitPO(*this); }
void pog::Set::accept(pogVisitor& v) const { v.visitSet(*this); }
