/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/utils/Exception.hpp>
#include <vle/value/Double.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/Vpz.hpp>

#include "utils/i18n.hpp"
#include "vpz/SaxParser.hpp"

#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace vle {
namespace vpz {

Vpz::Vpz(const std::string& filename)
  : m_filename(filename)
{
    parseFile(filename);
}

Vpz::Vpz(const Vpz& /*vpz*/) = default;

void
Vpz::write(std::ostream& out) const
{
    out << std::showpoint << std::fixed
        << std::setprecision(std::numeric_limits<double>::digits10)
        << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        << "<!DOCTYPE vle_project PUBLIC \"-//VLE TEAM//DTD Strict//EN\" "
        << "\"https://www.vle-project.org/vle-" << vle::string_version_abi()
        << ".dtd\">\n";

    m_project.write(out);
}

void
Vpz::parseFile(const std::string& filename)
{
    clear();
    m_filename.assign(filename);

    vpz::SaxParser saxparser(*this);
    saxparser.parseFile(filename);

    auto& cnd = project().experiment().conditions().get(
      Experiment::defaultSimulationEngineCondName());

    if (not cnd.valueOfPort("begin").get()) {
        cnd.setValueToPort("begin", value::Double::create(0));
        cnd.setValueToPort("duration", value::Double::create(100));
    }
}

void
Vpz::parseMemory(const std::string& buffer)
{
    clear();
    m_filename.clear();

    vpz::SaxParser saxparser(*this);
    saxparser.parseMemory(buffer);

    auto& cnd = project().experiment().conditions().get(
      Experiment::defaultSimulationEngineCondName());

    if (not cnd.valueOfPort("begin").get()) {
        cnd.setValueToPort("begin", value::Double::create(0));
        cnd.setValueToPort("duration", value::Double::create(100));
    }
}

std::shared_ptr<value::Value>
Vpz::parseValue(const std::string& buffer)
{
    Vpz vpz;
    SaxParser sax(vpz);
    sax.parseMemory(buffer);

    if (not sax.isValue()) {
        throw utils::ArgError(_("The buffer [%s] is not a value."),
                              buffer.c_str());
    }

    return sax.getValue();
}

void
Vpz::write()
{
    std::ofstream out(m_filename.c_str());

    if (out.fail() or out.bad()) {
        throw utils::FileError(_("Vpz: cannot open file '%s' for writing"),
                               m_filename.c_str());
        ;
    }

    out << std::showpoint << std::fixed
        << std::setprecision(std::numeric_limits<double>::digits10) << *this;
}

void
Vpz::write(const std::string& filename)
{
    m_filename.assign(filename);
    write();
}

std::string
Vpz::writeToString() const
{
    std::ostringstream out;

    out << std::showpoint << std::fixed
        << std::setprecision(std::numeric_limits<double>::digits10) << *this;

    return out.str();
}

void
Vpz::clear()
{
    m_filename.clear();
    m_project.clear();
    m_isGzip = false;
}

void
Vpz::fixExtension(std::string& filename)
{
    const std::string::size_type dot = filename.find_last_of('.');
    if (dot == std::string::npos) {
        filename += ".vpz";
    } else {
        if (filename.size() >= 4) {
            const std::string extension(filename, dot, 4);
            if (extension != ".vpz") {
                filename += ".vpz";
            }
        } else {
            filename += ".vpz";
        }
    }
}
}
} // namespace vle vpz
