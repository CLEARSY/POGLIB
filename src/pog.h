/** pog.h

   \copyright Copyright © CLEARSY 2022
   \license This file is part of POGLoader.

   POGLoader is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    POGLoader is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with POGLoader. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef POG_H
#define POG_H

#include <filesystem>
#include <variant>
using std::variant;
#include <string>
#include <vector>

#include "gpred.h"
#include "pred.h"
#include "tinyxml2.h"

namespace pog {

class pog;
class PO;
class POGroup;
class Set;
class Define;

class pogVisitor;

/**
 * @brief Reads the XML-based Domain Object Model from a POG file into a Pog
 * object.
 *
 * @param pog Reference to a tinyxml2::XMLDocument object containing the POG
 * data.
 * @return Pog The Pog instance containing the data in the read POG file.
 */
pog read(tinyxml2::XMLDocument &pog);
pog read(const std::filesystem::path &filename);

/**
 * @brief Represents the proof obligations of a B component
 *
 * This class encapsulates all the proof obligations of a B component.
 * Sets of hypotheses are grouped in Define objects.
 * Proof obligations are grouped in POGroup objects: each POGroup contains the
 * proof obligations corresponding to a B operation, initialisation, assertions,
 * well-definedness, etc.
 */
class pog {
 public:
  std::vector<Define> defines;
  std::vector<POGroup> pos;
  std::vector<BType> typeInfos;

  void accept(pogVisitor &v) const;
};

class Define {
 public:
  Define(const std::string &name, size_t hash = 0) : name{name}, hash{hash} {};
  std::string name;
  size_t hash;
  std::vector<variant<Set, Pred>> contents;

  void accept(pogVisitor &v) const;
};

class POGroup {
 public:
  std::string tag;
  size_t goalHash;
  std::vector<std::string> definitions;
  std::vector<Pred>
      hyps;  // chaque element d'une conjonction est stocké séparement
  std::vector<Pred>
      localHyps;  // chaque element d'une conjonction est stocké séparement
  std::vector<PO> simpleGoals;
  POGroup(const std::string &tag, size_t goalHash,
          const std::vector<std::string> &definitions, std::vector<Pred> &&hyps,
          std::vector<Pred> &&localHyps, std::vector<PO> &&simpleGoals)
      : tag{tag},
        goalHash{goalHash},
        definitions{definitions},
        hyps{std::move(hyps)},
        localHyps{std::move(localHyps)},
        simpleGoals{std::move(simpleGoals)} {}

  void accept(pogVisitor &v) const;
};

class PO {
 public:
  std::string tag;
  std::vector<int> localHypsRef;
  Pred goal;
  PO(const std::string &tag, const std::vector<int> &localHypsRef, Pred &&goal)
      : tag{tag}, localHypsRef{localHypsRef}, goal{std::move(goal)} {}
  PO copy() const { return PO(tag, localHypsRef, goal.copy()); }

  void accept(pogVisitor &v) const;
};

class Set {
 public:
  Set(const TypedVar &setName, const std::vector<TypedVar> &elts)
      : setName{setName}, elts{elts} {};
  TypedVar setName;
  std::vector<TypedVar> elts;

  static int compare(const Set &v1, const Set &v2);
  inline bool operator==(const Set &other) const {
    return compare(*this, other) == 0;
  }
  inline bool operator!=(const Set &other) const {
    return compare(*this, other) != 0;
  }
  inline bool operator<(const Set &other) const {
    return compare(*this, other) < 0;
  }
  inline bool operator>(const Set &other) const {
    return compare(*this, other) > 0;
  }
  inline bool operator<=(const Set &other) const {
    return compare(*this, other) <= 0;
  }
  inline bool operator>=(const Set &other) const {
    return compare(*this, other) >= 0;
  }

  void accept(pogVisitor &v) const;
};

class PogException : public std::exception {
 public:
  PogException(const std::string desc) : description{desc} {}
  ~PogException() throw() {}
  const char *what() const throw() { return description.c_str(); }

 private:
  std::string description;
};

class pogVisitor {
 public:
  virtual ~pogVisitor() = default;

  // Top level
  virtual void visitPog(const pog &pog) = 0;

  // Components
  virtual void visitDefine(const Define &define) = 0;
  virtual void visitPOGroup(const POGroup &poGroup) = 0;
  virtual void visitPO(const PO &po) = 0;
  virtual void visitSet(const Set &set) = 0;
};

}  // namespace pog

#endif  // POG_H
