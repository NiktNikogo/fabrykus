#include "isolatedGraph.hpp"
#include "Nodes/simpleMachineNode.hpp"

auto IsolatedGraph::findSources(ImFlow::ImNodeFlow &grid) -> void
{
	for(const auto& refs : grid.getNodes()) {
		for(auto& id : ids) {
			if(id == refs.second->getUID()) {
				auto nodePtr = std::dynamic_pointer_cast<SimpleMachineNode>(refs.second);
				nodes.push_back(nodePtr);
				if(nodePtr->getNodeType() == NodeType::INGREDIENT) {
					sources.push_back(nodePtr);	
				} else if(nodePtr->getNodeType() == NodeType::PRODUCT) {
					targets.push_back(nodePtr);
				}
			}
		}
	}
	//std::cout << nodes.size() << " " << sources.size() << " " << targets.size() << '\n';
}

auto IsolatedGraph::isProductionLine() const -> bool
{
	return (nodes.size() > 0) && (sources.size() > 0) && (targets.size() > 0);
}

IsolatedGraph::IsolatedGraph()
	: ids(), sources(), nodes(), targets()
{

}

IsolatedGraph::IsolatedGraph(std::vector<Id> ids)
	:
	ids(ids), sources(), nodes(), targets()
{
	
}
