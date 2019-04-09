/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "KdTree.h"
#include "KdTree2DPoint.h"

#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/box.hpp>

#include <tuple>

BOOST_GEOMETRY_REGISTER_BOX_TEMPLATED(geopop::AABBox, lower, upper)

namespace geoaggregator_detail {

inline double RadianToDegree(double rad) { return rad / M_PI * 180.0; }

inline double DegreeToRadian(double deg) { return deg / 180.0 * M_PI; }

} // namespace geoaggregator_detail

namespace geopop {

class Location;

/// Aggregates into a vector that must should remain alive for the usage duration of the Collector.
template <typename InsertIter, typename T>
class Collector
{
public:
        explicit Collector(const InsertIter& ins) : m_ins(ins) {}

        /// Collect a new element
        void operator()(T elem) { *m_ins = std::move(elem); }

private:
        InsertIter m_ins; ///< The (back_)insert_iterator that receives new elements
};

/// Build a Collector, useful for type inference
template <typename InsertIter, typename T = typename InsertIter::container_type::value_type>
Collector<InsertIter, T> MakeCollector(const InsertIter& ins)
{
        return Collector<InsertIter, T>(ins);
}

/**
 * A GeoAggregator can either be instantiated with a functor, or be called with one every time.
 *
 * A policy should have the following:
 *  - an embedded type Args: the type of argument it should receive at construction
 *  - a constructor that takes Args
 *  - AABB<geogrid_detail::KdTree2DPoint> GetBoundingBox() const
 *      Get the bounding box for the policy region
 *  - bool Contains(const geogrid_detail::KdTree2DPoint& pt) const
 *      Does the point fall withing the policy region?
 */
template <typename Policy, typename... F>
class GeoAggregator
{
        static_assert(sizeof...(F) <= 1, "Should have at most one functor type");
};

/**
 * A GeoAggregator that has to be called with a functor.
 */
template <typename Policy>
class GeoAggregator<Policy>
{
public:
        GeoAggregator(const KdTree<geogrid_detail::KdTree2DPoint>& tree, typename Policy::Args&& args)
            : m_policy(std::forward<typename Policy::Args>(args)), m_tree(tree)
        {
        }

        /// Aggregate over the area specified by the policy with the functor `f`.
        template <typename F>
        void operator()(F f)
        {
                auto box = m_policy.GetBoundingBox();
                m_tree.Apply(
                    [&f, this](const geogrid_detail::KdTree2DPoint& pt) -> bool {
                            if (m_policy.Contains(pt)) {
                                    f(pt.GetLocation());
                            }
                            return true;
                    },
                    box);
        }

private:
        Policy                                       m_policy;
        const KdTree<geogrid_detail::KdTree2DPoint>& m_tree;
};

/**
 * A GeoAggregator constructed with a functor.
 */
template <typename Policy, typename F>
class GeoAggregator<Policy, F> : public GeoAggregator<Policy>
{
public:
        GeoAggregator(const KdTree<geogrid_detail::KdTree2DPoint>& tree, F f, typename Policy::Args&& args)
            : GeoAggregator<Policy>(tree, std::forward<typename Policy::Args&&>(args)), m_functor(std::move(f))
        {
        }

        /// Aggregate over the policy with the functor specified at construction time
        void operator()() { GeoAggregator<Policy>::operator()(m_functor); }

private:
        F m_functor;
};

/**
 * GeoAggregator Policy that aggregates locations within a radius (in km) of a center point.
 */

class RadiusPolicy
{
public:
        using Args = std::tuple<geogrid_detail::KdTree2DPoint, double>;

        explicit RadiusPolicy(Args args) : m_center(std::move(std::get<0>(args))), m_radius(std::get<1>(args)) {}

        AABBox<geogrid_detail::KdTree2DPoint> GetBoundingBox() const
        {
                using namespace geoaggregator_detail;

                AABBox<geogrid_detail::KdTree2DPoint> box{};

                // As of boost 1.66, there's seems no way to do this in Boost.Geometry
                constexpr double EARTH_RADIUS_KM = 6371.0;
                const double     scaled_radius   = m_radius / EARTH_RADIUS_KM;

                const double startlon = m_center.Get<0>();
                const double startlat = m_center.Get<1>();
                const double londiff  = RadianToDegree(scaled_radius / std::cos(DegreeToRadian(startlat)));
                const double latdiff  = RadianToDegree(scaled_radius);

                box.upper = geogrid_detail::KdTree2DPoint(startlon + londiff, startlat + latdiff);
                box.lower = geogrid_detail::KdTree2DPoint(startlon - londiff, startlat - latdiff);

                return box;
        }

        bool Contains(const geogrid_detail::KdTree2DPoint& pt) const { return pt.InRadius(m_center, m_radius); }

private:
        geogrid_detail::KdTree2DPoint m_center;
        double                        m_radius;
};

/**
 * GeoAggregator Policy that aggregates over an axis aligned bounding box.
 */
class BoxPolicy
{
public:
        using Args = std::tuple<double, double, double, double>; ///< lon1, lat1, lon2, lat2

        explicit BoxPolicy(Args args) : m_args(std::move(args)) {}

        AABBox<geogrid_detail::KdTree2DPoint> GetBoundingBox() const
        {
                using std::get;
                return {{get<0>(m_args), get<1>(m_args)}, {get<2>(m_args), get<3>(m_args)}};
        }

        bool Contains(const geogrid_detail::KdTree2DPoint&) const { return true; }

private:
        Args m_args;
};

/**
 * A GeoAggregator Policy that aggregates over a (clockwise) polygon.
 */
class PolygonPolicy
{
public:
        using Args = boost::geometry::model::polygon<Coordinate, true>;

        explicit PolygonPolicy(boost::geometry::model::polygon<Coordinate, true> args) : m_poly(std::move(args)) {}

        AABBox<geogrid_detail::KdTree2DPoint> GetBoundingBox() const
        {
                using boost::geometry::get;
                AABBox<Coordinate> boostbox;
                boost::geometry::envelope(m_poly, boostbox);
                AABBox<geogrid_detail::KdTree2DPoint> box{{get<0>(boostbox.lower), get<1>(boostbox.lower)},
                                                          {get<0>(boostbox.upper), get<1>(boostbox.upper)}};
                return box;
        }

        bool Contains(const geogrid_detail::KdTree2DPoint& pt) const
        {
                return boost::geometry::within(pt.GetPoint(), m_poly);
        }

private:
        Args m_poly;
};

} // namespace geopop
