#include "isolatedGraph.hpp"
#include "Nodes/simpleMachineNode.hpp"
#include "Util/savedLink.hpp"
#include "Util/digraph.hpp"

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

auto IsolatedGraph::reverseFlow(ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void
{

	isReversed = true;
	std::vector<SavedLink> savedLinks;

	for(auto& node : nodes) {
		for(auto& p : node->getIns()) {
			if(p->isConnected()) {
				auto link = p->getLink().lock();
				if(link) {
					auto left = link->left();
					auto right = link->right();
					auto leftNode = dynamic_cast<SimpleMachineNode*>(left->getParent());
					auto rightNode =  dynamic_cast<SimpleMachineNode*>(right->getParent());
					SavedLink savedLink{
						leftNode->getOutPinIndex(left),
						rightNode->getInPinIndex(right),
						leftNode,
						rightNode,
						left->getPos().y - leftNode->getPos().y
					};
					savedLinks.push_back(savedLink);
				}
			}
		}
	}
	for(auto& node : nodes) {
		digraph.removeNodeEdges(node->getUID());
	}
	for(auto& node : nodes) {
		node->reverseFlow();
		node->syncPins();
	}

	std::swap(this->sources, this->targets);
	for (const auto& savedLink : savedLinks) {
		auto rightNode = savedLink.rightNode;
		auto leftNode = savedLink.leftNode;

		auto fromPin = 	rightNode->getOutListElement(savedLink.rightIdx);
		auto toPin = leftNode->getInListElement(savedLink.leftIdx);

		auto link = std::make_shared<ImFlow::Link>(fromPin, toPin, &grid);
        fromPin->createLink(toPin);
		grid.addLink(link);
		digraph.addEdge(rightNode->getUID(), leftNode->getUID(), savedLink.weight);
	}
	arrangeNodes(grid, digraph);
}

auto IsolatedGraph::arrangeNodes(ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void
{
	std::map<ImFlow::NodeUID, ImVec2> nodeSizes{};
    std::map<ImFlow::NodeUID, ImVec2> nodePositions{};
    for (const auto &[id, node] : grid.getNodes())
    {
        nodeSizes[id] = node->getSize();
        nodePositions[id] = node->getPos();
    }

    auto newPositionsOpt = digraph.calculateShiftedPositions(nodeSizes, nodePositions);

	if (!newPositionsOpt.has_value())
        return;

    auto newPositions = *newPositionsOpt;
    // auto mousePos = grid.screen2grid(ImGui::GetMousePos());
    for (auto &[id, node] : grid.getNodes())
    {
        node->setPos(newPositions[id]);
    }
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
