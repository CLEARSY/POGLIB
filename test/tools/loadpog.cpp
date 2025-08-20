/** loadpog.cpp

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
#include "pogXmlWriter.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: loadpog <pog_file>" << std::endl;
    return 1;
  }

  std::string pog_file = argv[1];

  std::filesystem::path pog_path(pog_file);
  if (!std::filesystem::exists(pog_path)) {
    std::cerr << "Error: File " << pog_file << " does not exist." << std::endl;
    return 1;
  }

  pog::pog pog;
  try {
    pog = pog::read(pog_path);
  } catch (const pog::PogException &e) {
    std::cerr << "POGLIB error: " << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  tinyxml2::XMLPrinter printer(stdout);
  Xml::pogXmlWriter writer(&printer);
  pog.accept(writer);

  return 0;
}