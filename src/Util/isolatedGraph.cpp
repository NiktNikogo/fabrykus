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

auto IsolatedGraph::getBoundingBox(ImFlow::ImNodeFlow &grid) -> std::pair<ImVec2, ImVec2>
{
	ImVec2 minBound = {FLT_MAX, FLT_MAX};
	ImVec2 maxBound = {-FLT_MAX, -FLT_MAX};

	for(const auto& node : nodes) {
		auto pos = node->getPos();
		auto size = node->getSize();

		if (pos.x < minBound.x) minBound.x = pos.x;
		if (pos.y < minBound.y) minBound.y = pos.y;
		if (pos.x + size.x > maxBound.x) maxBound.x = pos.x + size.x;
		if (pos.y + size.y > maxBound.y) maxBound.y = pos.y + size.y;
	}
	
	if(minBound.x != FLT_MAX) {
		const float padding = 20.0f;
		minBound.x -= 2*padding;
		maxBound.x += 2*padding;
		minBound.y -= padding;
		maxBound.y += padding;

		auto screenMin = grid.grid2screen(minBound);
		auto screenMax = grid.grid2screen(maxBound);
		return {screenMin, screenMax};
	}
	
	return {minBound, maxBound};
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
