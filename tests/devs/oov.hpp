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

#ifndef VLE_DEVS_TEST_OOV_HPP
#define VLE_DEVS_TEST_OOV_HPP

#include <vle/oov/Plugin.hpp>
#include <vle/value/Double.hpp>

#include <algorithm>
#include <memory>

#include <cassert>
#include <ciso646>
#include <cmath>

namespace vletest {
inline std::string
make_id(const std::string& view,
        const std::string& simulator,
        const std::string& parent,
        const std::string& port)
{
    std::string ret;
    ret.reserve(view.size() + simulator.size() + parent.size() + port.size() +
                4);

    ret += view;
    ret += '.';
    ret += parent;
    ret += '.';
    ret += simulator;
    ret += '.';
    ret += port;

    return ret;
}

class OutputPlugin : public vle::oov::Plugin
{
    using data_type = std::map<
      std::string,
      std::vector<std::pair<double, std::unique_ptr<vle::value::Value>>>>;

    data_type ppD;

public:
    OutputPlugin(const std::string& location)
      : vle::oov::Plugin(location)
    {}

    ~OutputPlugin() override = default;

    std::unique_ptr<vle::value::Matrix> matrix() const override
    {
        assert(not ppD.empty());
        const size_t columns = ppD.size() + 1; // colums = number of
                                               // observation + a time column.
        size_t rows = 0;

        for (auto& elem : ppD)
            for (std::size_t i = 0, e = elem.second.size(); i != e; ++i)
                rows = std::max(
                  rows,
                  static_cast<std::size_t>(std::floor(elem.second[i].first)));

        auto matrix =
          std::make_unique<vle::value::Matrix>(columns, rows + 1, 100, 100);

        size_t col = 1;
        for (auto& elem : ppD) {
            for (std::size_t i = 0, e = elem.second.size(); i != e; ++i) {
                auto row =
                  static_cast<std::size_t>(std::floor(elem.second[i].first));

                if (elem.second[i].second)
                    matrix->set(col, row, elem.second[i].second->clone());

                matrix->set(
                  0, row, vle::value::Double::create(elem.second[i].first));
            }
            col++;
        }

        return matrix;
    }

    std::string name() const override
    {
        return "OutputPlugin";
    }

    bool isCairo() const override
    {
        return false;
    }

    void onParameter(const std::string& plugin,
                     const std::string& location,
                     const std::string& file,
                     std::unique_ptr<vle::value::Value> parameters,
                     const double& time) override
    {
        (void)plugin;
        (void)location;
        (void)file;
        (void)parameters;
        (void)time;
    }

    void onNewObservable(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& view,
                         const double& time) override
    {
        (void)time;

        std::string id = make_id(view, parent, simulator, port);

        assert(ppD.find(id) == ppD.cend());

        ppD[id].reserve(100);
    }

    void onDelObservable(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& view,
                         const double& time) override
    {
        (void)time;

        std::string id = make_id(view, parent, simulator, port);

        assert(ppD.find(id) != ppD.cend());
    }

    void onValue(const std::string& simulator,
                 const std::string& parent,
                 const std::string& port,
                 const std::string& view,
                 const double& time,
                 std::unique_ptr<vle::value::Value> value) override
    {
        std::string id = make_id(view, parent, simulator, port);

        assert(ppD.find(id) != ppD.cend());

        ppD[id].emplace_back(time, std::move(value));
    }

    std::unique_ptr<vle::value::Matrix> finish(const double& /*time*/) override
    {
        return matrix();
    }
};

} // namespace vletest

#endif
