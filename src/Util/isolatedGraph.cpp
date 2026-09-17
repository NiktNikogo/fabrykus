#include "isolatedGraph.hpp"
#include "Nodes/simpleMachineNode.hpp"
#include "Util/savedLink.hpp"
#include "Util/digraph.hpp"

auto IsolatedGraph::groupNodes(ImFlow::ImNodeFlow &grid) -> void
{
	for (const auto &refs : grid.getNodes())
	{
		for (auto &id : ids)
		{
			if (id == refs.second->getUID())
			{
				auto nodePtr = std::dynamic_pointer_cast<SimpleMachineNode>(refs.second);
				nodes.push_back(nodePtr);
				switch (nodePtr->getNodeType())
				{
				case NodeType::INGREDIENT:
					if(!nodePtr->getIsReversed()) sources.push_back(nodePtr);
					if(nodePtr->getIsReversed()) targets.push_back(nodePtr);
					break;
				case NodeType::PRODUCT:
					if(!nodePtr->getIsReversed()) targets.push_back(nodePtr);
					if(nodePtr->getIsReversed()) sources.push_back(nodePtr);
					break;
				case NodeType::MACHINE:
					machines.push_back(nodePtr);
					break;
				default:
					break;
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

	for (const auto &node : nodes)
	{
		auto pos = node->getPos();
		auto size = node->getSize();

		if (pos.x < minBound.x)
			minBound.x = pos.x;
		if (pos.y < minBound.y)
			minBound.y = pos.y;
		if (pos.x + size.x > maxBound.x)
			maxBound.x = pos.x + size.x;
		if (pos.y + size.y > maxBound.y)
			maxBound.y = pos.y + size.y;
	}

	if (minBound.x != FLT_MAX)
	{
		const float padding = 20.0f;
		minBound.x -= 2 * padding;
		maxBound.x += 2 * padding;
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

	for (auto &node : nodes)
	{
		for (auto &p : node->getIns())
		{
			if (p->isConnected())
			{
				auto link = p->getLink().lock();
				if (link)
				{
					auto left = link->left();
					auto right = link->right();
					auto leftNode = dynamic_cast<SimpleMachineNode *>(left->getParent());
					auto rightNode = dynamic_cast<SimpleMachineNode *>(right->getParent());
					SavedLink savedLink{
						leftNode->getOutPinIndex(left),
						rightNode->getInPinIndex(right),
						leftNode,
						rightNode,
						left->getPos().y - leftNode->getPos().y};
					savedLinks.push_back(savedLink);
				}
			}
		}
	}
	for (auto &node : nodes)
	{
		digraph.removeNodeEdges(node->getUID());
	}
	for (auto &node : nodes)
	{
		node->reverseFlow();
		node->syncPins();
	}

	std::swap(sources, targets);
	for (const auto &savedLink : savedLinks)
	{
		auto rightNode = savedLink.rightNode;
		auto leftNode = savedLink.leftNode;

		auto fromPin = rightNode->getOutListElement(savedLink.rightIdx);
		auto toPin = leftNode->getInListElement(savedLink.leftIdx);

		auto link = std::make_shared<ImFlow::Link>(fromPin, toPin, &grid);
		fromPin->createLink(toPin);
		grid.addLink(link);
		digraph.addEdge(rightNode->getUID(), leftNode->getUID(), savedLink.weight);
	}
	arrangeNodes(grid, digraph);
}

auto IsolatedGraph::getOutputRatios(ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> std::map<Ingredient, std::vector<Ingredient>>
{
    auto findNode = [this](const Id& id) -> std::shared_ptr<SimpleMachineNode> {
        for (const auto &node : nodes) {
            if(node->getUID() == id) return node;
        }
        return nullptr;
    };

    std::map<Id, std::map<size_t, Id>> inPinToParent;

    for (auto &edge : grid.getLinks())
    {
        auto locked = edge.lock();
        if (!locked) continue;

        auto leftPin  = locked->left();
        auto rightPin = locked->right();

        auto leftNode  = dynamic_cast<SimpleMachineNode *>(leftPin->getParent());
        auto rightNode = dynamic_cast<SimpleMachineNode *>(rightPin->getParent());
        if (!leftNode || !rightNode) continue;

        auto &rightIns = rightNode->getIns();
        auto it = std::find_if(rightIns.begin(), rightIns.end(),
            [&](const auto &pinPtr) { return pinPtr.get() == rightPin; });
        if (it == rightIns.end()) continue;

        size_t pinIdx = std::distance(rightIns.begin(), it);
        inPinToParent[rightNode->getUID()][pinIdx] = leftNode->getUID();
    }

    struct TraverseItem
    {
        Id id;
        std::string ingName;
        Rational demand;
    };

    std::map<std::string, Ingredient> sourceTotals;

    auto traverse = [&](const Id& startId, const std::string& startIng, Rational startDemand) {
        std::vector<TraverseItem> stack;
        stack.push_back({ startId, startIng, startDemand });

        while(!stack.empty()) {
            auto item = std::move(stack.back());
            stack.pop_back();

            if(item.demand == 0) continue;

            auto node = findNode(item.id);
            if(!node) continue;

            auto parents = digraph.getParents(item.id);
            if(parents.empty()) {
                for(const auto& out : node->getOutList()) {
                    if(out.name != item.ingName || out.amount == 0) continue;
                    auto it = sourceTotals.find(out.name);
                    if(it == sourceTotals.end()) sourceTotals.emplace(out.name, Ingredient{item.demand, out.name});
                    else it->second.amount += item.demand;
                }
                continue;
            }

            NodeType nt = node->getNodeType();
            bool reversed = node->getIsReversed();

            bool isMerging   = (nt == NodeType::MERGER   && !reversed) || (nt == NodeType::SPLITTER && reversed);
            bool isSplitting = (nt == NodeType::SPLITTER && !reversed) || (nt == NodeType::MERGER   && reversed);

            if (isSplitting) {
                for (const auto &parentId : parents) {
                    auto parentNode = findNode(parentId);
                    if (!parentNode) continue;

                    bool provides = false;
                    for (const auto &pOut : parentNode->getOutList())
                        if (pOut.name == item.ingName && pOut.amount != 0) { provides = true; break; }

                    if (provides) stack.push_back({ parentId, item.ingName, item.demand });
                }
                continue;
            }

            if (isMerging) {
                auto &inList = node->getInList();
                auto pinMapIt = inPinToParent.find(item.id);

                for (size_t i = 0; i < inList.size(); ++i) {
                    if (inList[i].name != item.ingName || inList[i].amount == 0) continue;
                    if (pinMapIt == inPinToParent.end()) continue;

                    auto parentIt = pinMapIt->second.find(i);
                    if (parentIt == pinMapIt->second.end()) continue;

                    Rational required = item.demand * inList[i].amount;
                    stack.push_back({ parentIt->second, item.ingName, required });
                }
                continue;
            }

            Rational scale = Rational(1);
            if(item.id != startId) {
                Rational outAmount = Rational(0);
                bool found = false;
                for(const auto& out : node->getOutList()) {
                    if(out.name == item.ingName) { outAmount = out.amount; found = true; break; }
                }
                if(!found || outAmount == 0) continue;
                scale = item.demand / outAmount;
            }

            for(const auto &in : node->getInList()) {
                if(in.amount == 0) continue;
                Rational required = in.amount * scale;

                for(const auto &parentId : parents) {
                    auto parentNode = findNode(parentId);
                    if(!parentNode) continue;

                    bool parentProvides = false;
                    for(const auto &parentOut : parentNode->getOutList())
                        if(parentOut.name == in.name && parentOut.amount != 0) { parentProvides = true; break; }

                    if(parentProvides) stack.push_back({ parentId, in.name, required });
                }
            }
        }
    };

    std::map<Ingredient, std::vector<Ingredient>> ratios;
    for(const auto &target : getTargets()) {
        std::map<std::string, Ingredient> targetDemand;
        for(const auto &ing : target->getInList()) {
            if(ing.amount == 0) continue;
            auto it = targetDemand.find(ing.name);
            if(it == targetDemand.end()) targetDemand.emplace(ing.name, ing);
            else it->second.amount += ing.amount;
        }

        for(const auto &[name, ing] : targetDemand) {
            if(ing.amount == 0) continue;
            sourceTotals.clear();
            traverse(target->getUID(), ing.name, ing.amount);

            std::vector<Ingredient> perUnit;
            for (auto &[_, acc] : sourceTotals) perUnit.push_back(acc);
            ratios[ing] = std::move(perUnit);
        }
    }

    return ratios;
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
	for (auto &[id, node] : grid.getNodes())
	{
		node->setPos(newPositions[id]);
	}
}

IsolatedGraph::IsolatedGraph()
	: ids(), sources(), nodes(), targets(), machines()
{
}

IsolatedGraph::IsolatedGraph(std::vector<Id> ids)
	: ids(ids), sources(), nodes(), targets(), machines()
{
}
