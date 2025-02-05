/* 
 * kd_tree.hpp
 * 
 * Created on: Dec 31, 2018 10:27
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#ifndef KD_TREE_HPP
#define KD_TREE_HPP

#include <limits>
#include <algorithm>

#include "spatial/spatial.hpp"
#include "spatial/point_multiset.hpp"
#include "spatial/neighbor_iterator.hpp"

#include "graph/tree.hpp"
#include "sampling/base/tree_adapter.hpp"

namespace robosw
{
template <typename Space>
class KdTree : public Tree<typename Space::StateType *, double>, public TreeAdapter<Space>
{
  public:
    using TreeType = Tree<typename Space::StateType *, double>;
    using AdapterType = TreeAdapter<Space>;
    using StateType = typename TreeAdapter<Space>::StateType;
    using PathType = typename TreeAdapter<Space>::PathType;

    // inherit constructors
    using Tree<typename Space::StateType *, double>::Tree;
    using TreeAdapter<Space>::TreeAdapter;

    // setup Kd-tree types
    struct StateAccessor
    {
        double operator()(spatial::dimension_type dim, StateType *state) const
        {
            return (*state)[dim];
        }
    };

    using KdTreeType = spatial::point_multiset<Space::DimensionSize,
                                               StateType *,
                                               spatial::accessor_less<StateAccessor, StateType *>>;

  public:
    void AddTreeRootNode(StateType *sstate) final
    {
        TreeType::AddVertex(sstate);
        kdtree_.insert(sstate);
    }

    void ConnectTreeNodes(StateType *sstate, StateType *dstate, double dist) final
    {
        TreeType::AddEdge(sstate, dstate, dist);
        kdtree_.insert(sstate);
        kdtree_.insert(dstate);
    }

    void DisconnectTreeNodes(StateType *sstate, StateType *dstate) final
    {
        TreeType::RemoveEdge(sstate, dstate);
    }

    std::size_t GetTotalTreeNodeNumber() final
    {
        return TreeType::GetTotalVertexNumber();
    }

    PathType TraceBackToRoot(StateType *state) final
    {
        PathType path;
        path.push_back(state);
        auto parent = TreeType::GetParentVertex(state);
        while (parent != TreeType::vertex_end())
        {
            path.push_back(parent->state_);
            parent = TreeType::GetParentVertex(parent->vertex_id_);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    StateType *FindNearest(StateType *state) final
    {
        spatial::neighbor_iterator<KdTreeType> iter = spatial::neighbor_begin(kdtree_, state);
        return (*iter);
    }

    std::vector<StateType *> FindNear(StateType *state, double radius) final
    {
        std::vector<StateType *> near_states;

        spatial::neighbor_iterator<KdTreeType> iter = spatial::neighbor_begin(kdtree_, state);

        for (auto it = iter; it != kdtree_.end(); ++it)
        {
            if (distance(it) <= radius)
                near_states.push_back(*it);
            else
                break;
        }

        return near_states;
    }

  private:
    KdTreeType kdtree_;
};
} // namespace robosw

#endif /* KD_TREE_HPP */
